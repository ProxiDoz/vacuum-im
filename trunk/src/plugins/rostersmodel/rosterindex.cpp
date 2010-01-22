#include "rosterindex.h"

#include <QTimer>

RosterIndex::RosterIndex(int AType, const QString &AId)
{
  FParentIndex = NULL;
  FData.insert(RDR_TYPE,AType);
  FData.insert(RDR_INDEX_ID,AId);
  FFlags = (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  FRemoveOnLastChildRemoved = true;
  FRemoveChildsOnRemoved = true;
  FDestroyOnParentRemoved = true;
  FBlokedSetParentIndex = false;
}

RosterIndex::~RosterIndex()
{
  setParentIndex(NULL);
  emit indexDestroyed(this);
}

void RosterIndex::setParentIndex(IRosterIndex *AIndex)
{
  if (FBlokedSetParentIndex || FParentIndex == AIndex)
    return;

  FBlokedSetParentIndex = true;

  if (FParentIndex)
  {
    FParentIndex->removeChild(this);
    setParent(NULL);
  }

  if (AIndex)
  {
    FParentIndex = AIndex;
    FParentIndex->appendChild(this); 
    setParent(FParentIndex->instance());
  } 
  else
  {
    FParentIndex = NULL;
    if (FDestroyOnParentRemoved)
      QTimer::singleShot(0,this,SLOT(onDestroyByParentRemoved()));
  }

  FBlokedSetParentIndex = false;
}

int RosterIndex::row() const
{
  return FParentIndex ? FParentIndex->childRow(this) : -1;
}

void RosterIndex::appendChild(IRosterIndex *AIndex)
{
  if (AIndex && !FChilds.contains(AIndex))
  {
    emit childAboutToBeInserted(AIndex);
    FChilds.append(AIndex); 
    AIndex->setParentIndex(this);
    emit childInserted(AIndex);
  }
}

int RosterIndex::childRow(const IRosterIndex *AIndex) const
{
  return FChilds.indexOf((IRosterIndex * const)AIndex); 
}

bool RosterIndex::removeChild(IRosterIndex *AIndex)
{
  if (FChilds.contains(AIndex))
  {
    if (AIndex->removeChildsOnRemoved())
      AIndex->removeAllChilds();

    emit childAboutToBeRemoved(AIndex);
    FChilds.removeAt(FChilds.indexOf(AIndex));
    AIndex->setParentIndex(NULL);
    emit childRemoved(AIndex);

    if (FRemoveOnLastChildRemoved && FChilds.isEmpty())
      QTimer::singleShot(0,this,SLOT(onRemoveByLastChildRemoved()));

    return true;
  }
  return false;
}

void RosterIndex::removeAllChilds()
{
  while (FChilds.count()>0)
    removeChild(FChilds.value(0));
}

void RosterIndex::insertDataHolder(IRosterDataHolder *ADataHolder)
{
  connect(ADataHolder->instance(),SIGNAL(rosterDataChanged(IRosterIndex *, int)), SLOT(onDataHolderChanged(IRosterIndex *, int)));

  foreach(int role, ADataHolder->rosterDataRoles())
  {
    FDataHolders[role].insert(ADataHolder->rosterDataOrder(),ADataHolder);
    emit dataChanged(this,role);
  }
  emit dataHolderInserted(ADataHolder);
}

void RosterIndex::removeDataHolder(IRosterDataHolder *ADataHolder)
{
  disconnect(ADataHolder->instance(),SIGNAL(dataChanged(IRosterIndex *, int)),
    this,SLOT(onDataHolderChanged(IRosterIndex *, int)));
  
  foreach(int role, ADataHolder->rosterDataRoles())
  {
    FDataHolders[role].remove(ADataHolder->rosterDataOrder(),ADataHolder);
    if (FDataHolders[role].isEmpty())
      FDataHolders.remove(role);
    dataChanged(this,role);
  }
  emit dataHolderRemoved(ADataHolder);
}

QVariant RosterIndex::data(int ARole) const
{
  QVariant data = FData.value(ARole);
  if (!data.isValid())
  {
    QList<IRosterDataHolder *> dataHolders = FDataHolders.value(ARole).values();
    for (int i=0; !data.isValid() && i<dataHolders.count(); i++)
      data = dataHolders.at(i)->rosterData(this,ARole);
  }
  return data;
}

QMap<int,QVariant> RosterIndex::data() const
{
  QMap<int,QVariant> values = FData;
  foreach(int role, FDataHolders.keys())
  {
    if (!values.contains(role))
    {
      QList<IRosterDataHolder *> dataHolders = FDataHolders.value(role).values();
      for (int i=0; i<dataHolders.count(); i++)
      {
        QVariant roleData = dataHolders.at(i)->rosterData(this,role);
        if (roleData.isValid())
        {
          values.insert(role,roleData);
          break;
        }
      }
    }
  }
  return values;
}

void RosterIndex::setData(int ARole, const QVariant &AData)
{
  int i = 0;
  bool dataSeted = false;
  QList<IRosterDataHolder *> dataHolders = FDataHolders.value(ARole).values();
  while (i < dataHolders.count() && !dataSeted)
    dataSeted = dataHolders.value(i++)->setRosterData(this,ARole,AData);
  
  if (!dataSeted)
  {
    if (AData.isValid())
      FData.insert(ARole,AData);
    else
      FData.remove(ARole);
    emit dataChanged(this, ARole);
  }
}

QList<IRosterIndex *> RosterIndex::findChild(const QMultiHash<int, QVariant> AData, bool ASearchInChilds) const
{
  QList<IRosterIndex *> indexes;
  foreach (IRosterIndex *index, FChilds)
  {
    bool cheked = true;
    QList<int> dataRoles = AData.keys();
    for (int i=0; cheked && i<dataRoles.count(); i++)
      cheked = AData.values(dataRoles.at(i)).contains(index->data(dataRoles.at(i)));
    if (cheked)
      indexes.append(index);
    if (ASearchInChilds)
      indexes += index->findChild(AData,ASearchInChilds); 
  }
  return indexes;  
}

void RosterIndex::onDataHolderChanged(IRosterIndex *AIndex, int ARole)
{
 if (AIndex == this || AIndex == NULL)
   emit dataChanged(this, ARole);
}

void RosterIndex::onRemoveByLastChildRemoved()
{
  if (FChilds.isEmpty())
    setParentIndex(NULL);
}

void RosterIndex::onDestroyByParentRemoved()
{
  if (!FParentIndex)
    deleteLater();
}

