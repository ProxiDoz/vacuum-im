#include <QtDebug>
#include "menu.h"

Menu::Menu(QWidget *AParent)
  : QMenu(AParent)
{
  FMenuAction = NULL;
  FIconStorage = NULL;
  setSeparatorsCollapsible(true);
}

Menu::~Menu()
{
  if (FIconStorage)
    FIconStorage->removeAutoIcon(this);
  emit menuDestroyed(this);
}

void Menu::addAction(Action *AAction, int AGroup, bool ASort)
{
  QAction *befour = NULL;
  QAction *separator = NULL;
  QMultiMap<int,Action *>::iterator it = qFind(FActions.begin(),FActions.end(),AAction);
  if (it != FActions.end())
  {
    if (FActions.values(it.key()).count() == 1)
      FSeparators.remove(it.key());
    FActions.erase(it);
    QMenu::removeAction(AAction);  
  }

  it = FActions.find(AGroup);  
  if (it == FActions.end())  
  {
    befour = nextGroupSeparator(AGroup);
    befour != NULL ? QMenu::insertAction(befour,AAction) : QMenu::addAction(AAction);
    separator = insertSeparator(AAction);
    FSeparators.insert(AGroup,separator);
  }
  else
  {
    if (ASort)
    {
      QList<QAction *> actionList = QMenu::actions();
      
      bool sortRole = true;
      QString sortString = AAction->data(Action::DR_SortString).toString();
      if (sortString.isEmpty())
      {
        sortString = AAction->text();
        sortRole = false;
      }
      
      for (int i = 0; !befour && i<actionList.count(); ++i)
      {
        QAction *qaction = actionList.at(i);
        if (FActions.key((Action *)qaction)==AGroup)
        {
          QString curSortString = qaction->text();
          if (sortRole)
          {
            Action *action = qobject_cast<Action *>(qaction);
            if (action)
              curSortString = action->data(Action::DR_SortString).toString();
          }
          if (QString::localeAwareCompare(curSortString,sortString) > 0)
            befour = actionList.at(i);
        }
      }
    }

    if (!befour)
    {
      QMap<int,QAction *>::const_iterator sepIt= FSeparators.upperBound(AGroup);
      if (sepIt != FSeparators.constEnd())
        befour = sepIt.value();
    }

    if (befour)
      QMenu::insertAction(befour,AAction); 
    else
      QMenu::addAction(AAction); 
  }

  FActions.insertMulti(AGroup,AAction);
  connect(AAction,SIGNAL(actionDestroyed(Action *)),SLOT(onActionDestroyed(Action *)));
  emit actionInserted(befour,AAction);
  if (separator) emit separatorInserted(AAction,separator);
}

void Menu::addMenuActions(const Menu *AMenu, int AGroup, bool ASort)
{
  Action *action;
  QList<Action *> actionList = AMenu->actions(AGroup);
  foreach(action,actionList)
    addAction(action,AMenu->actionGroup(action),ASort);
}

Action *Menu::menuAction()
{
  if (!FMenuAction)
  {
    FMenuAction = new Action(this);
    FMenuAction->setMenu(this);
  }
  return FMenuAction;
}

void Menu::setIcon(const QIcon &AIcon)
{
  if (FMenuAction)
    FMenuAction->setIcon(AIcon);
  QMenu::setIcon(AIcon);
}

void Menu::setIcon(const QString &AStorageName, const QString &AIconKey, int AIconIndex)
{
  if (!AStorageName.isEmpty() && !AIconKey.isEmpty())
  {
    FIconStorage = IconStorage::staticStorage(AStorageName);
    FIconStorage->insertAutoIcon(this,AIconKey,AIconIndex);
  }
  else if (FIconStorage)
  {
    FIconStorage->removeAutoIcon(this);
    FIconStorage = NULL;
  }
}

void Menu::setTitle(const QString &ATitle)
{
  if (FMenuAction)
    FMenuAction->setText(ATitle);
  QMenu::setTitle(ATitle);
}

void Menu::removeAction(Action *AAction)
{
  QMultiMap<int,Action *>::iterator it = qFind(FActions.begin(),FActions.end(),AAction);
  if (it != FActions.end())
  {
    disconnect(AAction,SIGNAL(actionDestroyed(Action *)),this,SLOT(onActionDestroyed(Action *)));

    if (FActions.values(it.key()).count() == 1)
    {
      QAction *separator = FSeparators.value(it.key());
      FSeparators.remove(it.key());
      QMenu::removeAction(separator);  
      emit separatorRemoved(separator);
    }

    FActions.erase(it);
    QMenu::removeAction(AAction);

    emit actionRemoved(AAction);

    Menu *menu = AAction->menu();
    if (menu && menu->parent() == this)
      menu->deleteLater();
    else if (AAction->parent() == this)
      AAction->deleteLater();
  }
}

int Menu::actionGroup(const Action *AAction) const
{
  QMultiMap<int,Action *>::const_iterator it = qFind(FActions.begin(),FActions.end(),AAction);
  if (it != FActions.constEnd())
    return it.key();
  return AG_NULL;
}

QAction *Menu::nextGroupSeparator(int AGroup) const
{
  QMultiMap<int,Action *>::const_iterator it = FActions.lowerBound(AGroup);
  if (it != FActions.end())
    return FSeparators.value(it.key());
  return NULL;
}

QList<Action *> Menu::actions(int AGroup) const
{
  if (AGroup == AG_NULL)
    return FActions.values();
  return FActions.values(AGroup);
}

QList<Action *> Menu::findActions(const QMultiHash<int, QVariant> AData, bool ASearchInSubMenu /*= false*/) const
{
  QList<Action *> actionList;
  QList<int> keys = AData.keys();
  foreach(Action *action,FActions)
  {
    foreach (int key, keys)
      if (AData.values(key).contains(action->data(key)))
      {
        actionList.append(action);
        break;
      }
    if (ASearchInSubMenu && action->menu())
      actionList += action->menu()->findActions(AData,ASearchInSubMenu);
  }

  return actionList;
}

void Menu::clear() 
{
  foreach(Action *action,FActions)
    removeAction(action);
  QMenu::clear(); 
}

void Menu::onActionDestroyed(Action *AAction)
{
  removeAction(AAction);
}