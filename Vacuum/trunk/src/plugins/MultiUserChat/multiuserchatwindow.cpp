#include "multiuserchatwindow.h"

#include <QTimer>
#include <QInputDialog>
#include <QContextMenuEvent>

#define IN_EXIT                     "psi/cancel"
#define IN_CHAT_MESSAGE             "psi/start-chat"
#define IN_MULTICHAT_MESSAGE        "psi/groupChat"
#define IN_DATA_FORM_MESSAGE        "psi/events"

#define SVN_WINDOW                  "windows:window[]"
#define SVN_WINDOW_GEOMETRY         SVN_WINDOW ":geometry"
#define SVN_WINDOW_HSPLITTER        SVN_WINDOW ":hsplitter"  
#define SVN_WINDOW_VSPLITTER        SVN_WINDOW ":vsplitter"  

#define ADR_USER_NICK               Action::DR_Parametr1

#define NICK_MENU_KEY               Qt::ControlModifier+Qt::Key_Space

MultiUserChatWindow::MultiUserChatWindow(IMessenger *AMessenger, IMultiUserChat *AMultiChat)
{
  ui.setupUi(this);
  ui.lblRoom->setText(AMultiChat->roomJid().hFull());
  ui.lblNick->setText(AMultiChat->nickName());
  ui.ltwUsers->installEventFilter(this);

  FSettings = NULL;
  FStatusIcons = NULL;

  FMessenger = AMessenger;
  FMultiChat = AMultiChat;
  FMultiChat->instance()->setParent(this);

  FSplitterLoaded = false;
  FExitOnChatClosed = false;

  FViewWidget = FMessenger->newViewWidget(FMultiChat->streamJid(),FMultiChat->roomJid());
  FViewWidget->setShowKind(IViewWidget::GroupChatMessage);
  FViewWidget->document()->setDefaultFont(FMessenger->defaultChatFont());
  ui.wdtView->setLayout(new QVBoxLayout);
  ui.wdtView->layout()->addWidget(FViewWidget);
  ui.wdtView->layout()->setMargin(0);
  ui.wdtView->layout()->setSpacing(0);

  FEditWidget = FMessenger->newEditWidget(FMultiChat->streamJid(),FMultiChat->roomJid());
  FEditWidget->document()->setDefaultFont(FMessenger->defaultChatFont());
  ui.wdtEdit->setLayout(new QVBoxLayout);
  ui.wdtEdit->layout()->addWidget(FEditWidget);
  ui.wdtEdit->layout()->setMargin(0);
  ui.wdtEdit->layout()->setSpacing(0);
  connect(FEditWidget,SIGNAL(messageReady()),SLOT(onMessageSend()));
  connect(FEditWidget,SIGNAL(keyEventReceived(QKeyEvent *,bool &)),SLOT(onEditWidgetKeyEvent(QKeyEvent *,bool &)));

  FToolBarWidget = FMessenger->newToolBarWidget(NULL,FViewWidget,FEditWidget,NULL);
  ui.wdtToolBar->setLayout(new QVBoxLayout);
  ui.wdtToolBar->layout()->addWidget(FToolBarWidget);
  ui.wdtToolBar->layout()->setMargin(0);
  ui.wdtToolBar->layout()->setSpacing(0);

  connect(FMultiChat->instance(),SIGNAL(chatOpened()),SLOT(onChatOpened()));
  connect(FMultiChat->instance(),SIGNAL(chatNotify(const QString &, const QString &)),
    SLOT(onChatNotify(const QString &, const QString &)));
  connect(FMultiChat->instance(),SIGNAL(chatError(const QString &, const QString &)),
    SLOT(onChatError(const QString &, const QString &)));
  connect(FMultiChat->instance(),SIGNAL(chatClosed()),SLOT(onChatClosed()));
  connect(FMultiChat->instance(),SIGNAL(streamJidChanged(const Jid &, const Jid &)),SLOT(onStreamJidChanged(const Jid &, const Jid &)));
  connect(FMultiChat->instance(),SIGNAL(userPresence(IMultiUser *,int,const QString &)),
    SLOT(onUserPresence(IMultiUser *,int,const QString &)));
  connect(FMultiChat->instance(),SIGNAL(userDataChanged(IMultiUser *, int, const QVariant, const QVariant &)),
    SLOT(onUserDataChanged(IMultiUser *, int, const QVariant, const QVariant &)));
  connect(FMultiChat->instance(),SIGNAL(userNickChanged(IMultiUser *, const QString &, const QString &)),
    SLOT(onUserNickChanged(IMultiUser *, const QString &, const QString &)));
  connect(FMultiChat->instance(),SIGNAL(presenceChanged(int, const QString &)),
    SLOT(onPresenceChanged(int, const QString &)));
  connect(FMultiChat->instance(),SIGNAL(topicChanged(const QString &)),SLOT(onTopicChanged(const QString &)));
  connect(FMultiChat->instance(),SIGNAL(serviceMessageReceived(const Message &)),
    SLOT(onServiceMessageReceived(const Message &)));
  connect(FMultiChat->instance(),SIGNAL(messageReceived(const QString &, const Message &)),
    SLOT(onMessageReceived(const QString &, const Message &)));
  connect(FMultiChat->instance(),SIGNAL(userKicked(const QString &, const QString &, const QString &)),
    SLOT(onUserKicked(const QString &, const QString &, const QString &)));
  connect(FMultiChat->instance(),SIGNAL(userBanned(const QString &, const QString &, const QString &)),
    SLOT(onUserBanned(const QString &, const QString &, const QString &)));
  connect(FMultiChat->instance(),SIGNAL(affiliationListReceived(const QString &,const QList<IMultiUserListItem> &)),
    SLOT(onAffiliationListReceived(const QString &,const QList<IMultiUserListItem> &)));
  connect(FMultiChat->instance(),SIGNAL(configFormReceived(const QDomElement &)), SLOT(onConfigFormReceived(const QDomElement &)));

  connect(FMessenger->instance(),SIGNAL(defaultChatFontChanged(const QFont &)), SLOT(onDefaultChatFontChanged(const QFont &)));
  
  connect(ui.ltwUsers,SIGNAL(itemActivated(QListWidgetItem *)),SLOT(onListItemActivated(QListWidgetItem *)));

  connect(this,SIGNAL(windowActivated()),SLOT(onWindowActivated()));

  initialize();
  createMenuBarActions();
  createRoomUtilsActions();
}

MultiUserChatWindow::~MultiUserChatWindow()
{
  exitMultiUserChat();
  foreach(IChatWindow *window,FChatWindows)
    window->deleteLater();
  FMessenger->removeMessageHandler(this,MHO_MULTIUSERCHAT);
  emit windowDestroyed();
}

void MultiUserChatWindow::showWindow()
{
  if (isWindow() && !isVisible())
  {
    if (FMessenger->checkOption(IMessenger::OpenChatInTabWindow))
    {
      ITabWindow *tabWindow = FMessenger->openTabWindow();
      tabWindow->addWidget(this);
    }
  }
  if (isWindow())
    isVisible() ? (isMinimized() ? showNormal() : activateWindow()) : show();
  else
    emit windowShow();
}

void MultiUserChatWindow::closeWindow()
{
  if (isWindow())
    close();
  else
    emit windowClose();
}

bool MultiUserChatWindow::openWindow(IRosterIndex * /*AIndex*/)
{
  return false;
}

bool MultiUserChatWindow::openWindow(const Jid &AStreamJid, const Jid &AContactJid, Message::MessageType AType)
{
  if ((streamJid() == AStreamJid) && (roomJid() && AContactJid))
  {
    if (AType == Message::GroupChat)
    {
      showWindow();
    }
    else
    {
      openChatWindow(AContactJid);
    }
    return true;
  }
  return false;
}

bool MultiUserChatWindow::checkMessage(const Message &AMessage)
{
  return (streamJid() == AMessage.to()) && (roomJid() && AMessage.from());
}

bool MultiUserChatWindow::notifyOptions(const Message &AMessage, QIcon &AIcon, QString &AToolTip, int &AFlags)
{
  Jid contactJid = AMessage.from();
  if (AMessage.type() == Message::Error)
  {
    return false;
  }
  else if (!contactJid.resource().isEmpty())
  {
    if (AMessage.type() == Message::GroupChat)
    {
      if (!isActive())
      {
        SkinIconset *iconset = Skin::getSkinIconset(SYSTEM_ICONSETFILE);
        AIcon = iconset->iconByName(IN_MULTICHAT_MESSAGE);
        AToolTip = tr("New groupchat message in: %1").arg(contactJid.node());
        AFlags = 0;
        return true;
      }
    }
    else
    {
      IChatWindow *window = findChatWindow(AMessage.from());
      if (window == NULL || !window->isActive())
      {
        SkinIconset *iconset = Skin::getSkinIconset(SYSTEM_ICONSETFILE);
        AIcon = iconset->iconByName(IN_CHAT_MESSAGE);
        AToolTip = tr("New private message from: [%1]").arg(contactJid.resource());
        AFlags = 0;
        return true;
      }
    }
  }
  else
  {
    if (!AMessage.stanza().firstElement("x",NS_JABBER_DATA).isNull())
    {
      SkinIconset *iconset = Skin::getSkinIconset(SYSTEM_ICONSETFILE);
      AIcon = iconset->iconByName(IN_DATA_FORM_MESSAGE);
      AToolTip = tr("Data form received from: %1").arg(contactJid.node());
      AFlags = 0;
      return true;
    }
  }
  return false;
}

void MultiUserChatWindow::receiveMessage(int AMessageId)
{
  Message message = FMessenger->messageById(AMessageId);
  Jid contactJid = message.from();

  if (message.type() == Message::Error)
  {
    FMessenger->removeMessage(AMessageId);
  }
  else if (contactJid.resource().isEmpty() && !message.stanza().firstElement("x",NS_JABBER_DATA).isNull())
  {
    QDomElement formElem = message.stanza().firstElement("x",NS_JABBER_DATA);
    IDataDialog *dialog = FDataForms->newDataDialog(formElem,this);
    connect(dialog,SIGNAL(accepted()),SLOT(onDataFormMessageDialogAccepted()));
    showServiceMessage(tr("Data form received: %1").arg(dialog->dataForm()->title()));
    FDataFormMessages.insert(AMessageId,dialog);
  }
  else if (message.type() == Message::GroupChat)
  {
    if (!isActive())
    {
      FActiveMessages.append(AMessageId);
      updateWindow();
    }
    else
      FMessenger->removeMessage(AMessageId);
  }
  else 
  {
    IChatWindow *window = getChatWindow(contactJid);
    if (window)
    {
      if (!window->isActive())
      {
        FActiveChatMessages.insertMulti(window, AMessageId);
        updateChatWindow(window);
        updateListItem(contactJid);
      }
      else
        FMessenger->removeMessage(AMessageId);
    }
  }
}

void MultiUserChatWindow::showMessage(int AMessageId)
{
  if (FDataFormMessages.contains(AMessageId))
  {
    IDataDialog *dialog = FDataFormMessages.take(AMessageId);
    if(dialog)
    {
      dialog->show();
      FMessenger->removeMessage(AMessageId);
    }
  }
  else
  {
    Message message = FMessenger->messageById(AMessageId);
    openWindow(message.to(),message.from(),message.type());
  }
}

IChatWindow *MultiUserChatWindow::openChatWindow(const Jid &AContactJid)
{
  IChatWindow *window = getChatWindow(AContactJid);
  if (window)
    showChatWindow(window);
  return window;
}

IChatWindow *MultiUserChatWindow::findChatWindow(const Jid &AContactJid) const
{
  foreach(IChatWindow *window,FChatWindows)
    if (window->contactJid() == AContactJid)
      return window;
  return NULL;
}

void MultiUserChatWindow::exitMultiUserChat()
{
  if (FMultiChat->isOpen())
  {
    FExitOnChatClosed = true;
    FMultiChat->setPresence(IPresence::Offline,tr("Disconnected"));
    closeWindow();
    QTimer::singleShot(5000,this,SLOT(deleteLater()));
  }
  else
    deleteLater();
}

void MultiUserChatWindow::initialize()
{
  IPlugin *plugin = FMessenger->pluginManager()->getPlugins("ISettingsPlugin").value(0,NULL);
  if (plugin)
  {
    ISettingsPlugin *settingsPlugin = qobject_cast<ISettingsPlugin *>(plugin->instance());
    if (settingsPlugin)
    {
      FSettings = settingsPlugin->settingsForPlugin(MULTIUSERCHAT_UUID);
    }
  }

  plugin = FMessenger->pluginManager()->getPlugins("IStatusIcons").value(0,NULL);
  if (plugin)
  {
    FStatusIcons = qobject_cast<IStatusIcons *>(plugin->instance());
    if (FStatusIcons)
    {
      connect(FStatusIcons->instance(),SIGNAL(statusIconsChanged()),SLOT(onStatusIconsChanged()));
    }
  }

  plugin = FMessenger->pluginManager()->getPlugins("IAccountManager").value(0,NULL);
  if (plugin)
  {
    IAccountManager *accountManager = qobject_cast<IAccountManager *>(plugin->instance());
    if (accountManager)
    {
      IAccount *account = accountManager->accountByStream(streamJid());
      if (account)
      {
        ui.lblAccount->setText(account->name());
        connect(account->instance(),SIGNAL(changed(const QString &, const QVariant &)),
          SLOT(onAccountChanged(const QString &, const QVariant &)));
      }
    }
  }

  plugin = FMessenger->pluginManager()->getPlugins("IDataForms").value(0,NULL);
  if (plugin)
  {
    FDataForms = qobject_cast<IDataForms *>(plugin->instance());
  }

  FMessenger->insertMessageHandler(this,MHO_MULTIUSERCHAT);
}

void MultiUserChatWindow::createMenuBarActions()
{
  FRoomMenu = new Menu(this);
  FRoomMenu->setTitle(tr("Groupchat"));
  ui.mnbMenuBar->addMenu(FRoomMenu);

  FChangeNick = new Action(FRoomMenu);
  FChangeNick->setText(tr("Change room nick"));
  connect(FChangeNick,SIGNAL(triggered(bool)),SLOT(onMenuBarActionTriggered(bool)));
  FRoomMenu->addAction(FChangeNick,AG_DEFAULT,true);

  FChangeSubject = new Action(FRoomMenu);
  FChangeSubject->setText(tr("Change subject"));
  connect(FChangeSubject,SIGNAL(triggered(bool)),SLOT(onMenuBarActionTriggered(bool)));
  FRoomMenu->addAction(FChangeSubject,AG_DEFAULT,true);

  FClearChat = new Action(FRoomMenu);
  FClearChat->setText(tr("Clear chat window"));
  connect(FClearChat,SIGNAL(triggered(bool)),SLOT(onMenuBarActionTriggered(bool)));
  FRoomMenu->addAction(FClearChat,AG_DEFAULT,true);

  FQuitRoom = new Action(FRoomMenu);
  FQuitRoom->setIcon(SYSTEM_ICONSETFILE,IN_EXIT);
  FQuitRoom->setText(tr("Exit"));
  FQuitRoom->setShortcut(tr("Ctrl+X"));
  connect(FQuitRoom,SIGNAL(triggered(bool)),SLOT(onMenuBarActionTriggered(bool)));
  FRoomMenu->addAction(FQuitRoom,AG_MAINWINDOW_MMENU_QUIT,true);


  FToolsMenu = new Menu(this);
  FToolsMenu->setTitle("Tools");
  ui.mnbMenuBar->addMenu(FToolsMenu);

  FRequestVoice = new Action(FToolsMenu);
  FRequestVoice->setText(tr("Request voice"));
  connect(FRequestVoice,SIGNAL(triggered(bool)),SLOT(onMenuBarActionTriggered(bool)));
  FToolsMenu->addAction(FRequestVoice,AG_DEFAULT,false);

  FBanList = new Action(FToolsMenu);
  FBanList->setText(tr("Edit ban list"));
  connect(FBanList,SIGNAL(triggered(bool)),SLOT(onMenuBarActionTriggered(bool)));
  FToolsMenu->addAction(FBanList,AG_DEFAULT,false);

  FMembersList = new Action(FToolsMenu);
  FMembersList->setText(tr("Edit members list"));
  connect(FMembersList,SIGNAL(triggered(bool)),SLOT(onMenuBarActionTriggered(bool)));
  FToolsMenu->addAction(FMembersList,AG_DEFAULT,false);

  FAdminsList = new Action(FToolsMenu);
  FAdminsList->setText(tr("Edit administrators list"));
  connect(FAdminsList,SIGNAL(triggered(bool)),SLOT(onMenuBarActionTriggered(bool)));
  FToolsMenu->addAction(FAdminsList,AG_DEFAULT,false);

  FOwnersList = new Action(FToolsMenu);
  FOwnersList->setText(tr("Edit owners list"));
  connect(FOwnersList,SIGNAL(triggered(bool)),SLOT(onMenuBarActionTriggered(bool)));
  FToolsMenu->addAction(FOwnersList,AG_DEFAULT,false);

  FConfigRoom = new Action(FToolsMenu);
  FConfigRoom->setText(tr("Configure room"));
  connect(FConfigRoom,SIGNAL(triggered(bool)),SLOT(onMenuBarActionTriggered(bool)));
  FToolsMenu->addAction(FConfigRoom,AG_DEFAULT,false);
}

void MultiUserChatWindow::updateMenuBarActions()
{
  QString role = FMultiChat->isOpen() ? FMultiChat->mainUser()->role() : MUC_ROLE_NONE;
  QString affiliation = FMultiChat->isOpen() ? FMultiChat->mainUser()->affiliation() : MUC_AFFIL_NONE;
  if (affiliation == MUC_AFFIL_OWNER)
  {
    FChangeSubject->setVisible(true);
    FRequestVoice->setVisible(false);
    FBanList->setVisible(true);
    FMembersList->setVisible(true);
    FAdminsList->setVisible(true);
    FOwnersList->setVisible(true);
    FConfigRoom->setVisible(true);
    FToolsMenu->setEnabled(true);
  }
  else if (affiliation == MUC_AFFIL_ADMIN)
  {
    FChangeSubject->setVisible(true);
    FRequestVoice->setVisible(false);
    FBanList->setVisible(true);
    FMembersList->setVisible(true);
    FAdminsList->setVisible(true);
    FOwnersList->setVisible(true);
    FConfigRoom->setVisible(false);
    FToolsMenu->setEnabled(true);
  }
  else if (role == MUC_ROLE_VISITOR)
  {
    FChangeSubject->setVisible(false);
    FRequestVoice->setVisible(true);
    FBanList->setVisible(false);
    FMembersList->setVisible(false);
    FAdminsList->setVisible(false);
    FOwnersList->setVisible(false);
    FConfigRoom->setVisible(false);
    FToolsMenu->setEnabled(true);
  }
  else
  {
    FChangeSubject->setVisible(true);
    FRequestVoice->setVisible(false);
    FBanList->setVisible(false);
    FMembersList->setVisible(false);
    FAdminsList->setVisible(false);
    FOwnersList->setVisible(false);
    FConfigRoom->setVisible(false);
    FToolsMenu->setEnabled(false);
  }
}

void MultiUserChatWindow::createRoomUtilsActions()
{
  FRoomUtilsMenu = new Menu(this);
  FRoomUtilsMenu->setTitle(tr("Room Utilities"));

  FSetRoleNode = new Action(FRoomUtilsMenu);
  FSetRoleNode->setText(tr("Kick user"));
  connect(FSetRoleNode,SIGNAL(triggered(bool)),SLOT(onRoomUtilsActionTriggered(bool)));
  FRoomUtilsMenu->addAction(FSetRoleNode,AG_MULTIUSERCHAT_ROOM_UTILS,false);

  FSetAffilOutcast = new Action(FRoomUtilsMenu);
  FSetAffilOutcast->setText(tr("Ban user"));
  connect(FSetAffilOutcast,SIGNAL(triggered(bool)),SLOT(onRoomUtilsActionTriggered(bool)));
  FRoomUtilsMenu->addAction(FSetAffilOutcast,AG_MULTIUSERCHAT_ROOM_UTILS,false);

  FChangeRole = new Menu(FRoomUtilsMenu);
  FChangeRole->setTitle(tr("Change Role"));
  {
    FSetRoleVisitor = new Action(FChangeRole);
    FSetRoleVisitor->setCheckable(true);
    FSetRoleVisitor->setText(tr("Visitor"));
    connect(FSetRoleVisitor,SIGNAL(triggered(bool)),SLOT(onRoomUtilsActionTriggered(bool)));
    FChangeRole->addAction(FSetRoleVisitor,AG_DEFAULT,false);

    FSetRoleParticipant = new Action(FChangeRole);
    FSetRoleParticipant->setCheckable(true);
    FSetRoleParticipant->setText(tr("Participant"));
    connect(FSetRoleParticipant,SIGNAL(triggered(bool)),SLOT(onRoomUtilsActionTriggered(bool)));
    FChangeRole->addAction(FSetRoleParticipant,AG_DEFAULT,false);

    FSetRoleModerator = new Action(FChangeRole);
    FSetRoleModerator->setCheckable(true);
    FSetRoleModerator->setText(tr("Moderator"));
    connect(FSetRoleModerator,SIGNAL(triggered(bool)),SLOT(onRoomUtilsActionTriggered(bool)));
    FChangeRole->addAction(FSetRoleModerator,AG_DEFAULT,false);
  }
  FRoomUtilsMenu->addAction(FChangeRole->menuAction(),AG_MULTIUSERCHAT_ROOM_UTILS,false);
  
  FChangeAffiliation = new Menu(FRoomUtilsMenu);
  FChangeAffiliation->setTitle(tr("Change Affiliation"));
  {
    FSetAffilNone = new Action(FChangeAffiliation);
    FSetAffilNone->setCheckable(true);
    FSetAffilNone->setText("None");
    connect(FSetAffilNone,SIGNAL(triggered(bool)),SLOT(onRoomUtilsActionTriggered(bool)));
    FChangeAffiliation->addAction(FSetAffilNone,AG_DEFAULT,false);

    FSetAffilMember = new Action(FChangeAffiliation);
    FSetAffilMember->setCheckable(true);
    FSetAffilMember->setText("Member");
    connect(FSetAffilMember,SIGNAL(triggered(bool)),SLOT(onRoomUtilsActionTriggered(bool)));
    FChangeAffiliation->addAction(FSetAffilMember,AG_DEFAULT,false);

    FSetAffilAdmin = new Action(FChangeAffiliation);
    FSetAffilAdmin->setCheckable(true);
    FSetAffilAdmin->setText("Administrator");
    connect(FSetAffilAdmin,SIGNAL(triggered(bool)),SLOT(onRoomUtilsActionTriggered(bool)));
    FChangeAffiliation->addAction(FSetAffilAdmin,AG_DEFAULT,false);

    FSetAffilOwner = new Action(FChangeAffiliation);
    FSetAffilOwner->setCheckable(true);
    FSetAffilOwner->setText("Owner");
    connect(FSetAffilOwner,SIGNAL(triggered(bool)),SLOT(onRoomUtilsActionTriggered(bool)));
    FChangeAffiliation->addAction(FSetAffilOwner,AG_DEFAULT,false);
  }
  FRoomUtilsMenu->addAction(FChangeAffiliation->menuAction(),AG_MULTIUSERCHAT_ROOM_UTILS,false);
}

void MultiUserChatWindow::insertRoomUtilsActions(Menu *AMenu, IMultiUser *AUser)
{
  IMultiUser *muser = FMultiChat->mainUser();
  if (muser && muser->role() == MUC_ROLE_MODERATOR)
  {
    FRoomUtilsMenu->menuAction()->setData(ADR_USER_NICK,AUser->nickName());

    FSetRoleVisitor->setChecked(AUser->role() == MUC_ROLE_VISITOR);
    FSetRoleParticipant->setChecked(AUser->role() == MUC_ROLE_PARTICIPANT);
    FSetRoleModerator->setChecked(AUser->role() == MUC_ROLE_MODERATOR);

    FSetAffilNone->setChecked(AUser->affiliation() == MUC_AFFIL_NONE);
    FSetAffilMember->setChecked(AUser->affiliation() == MUC_AFFIL_MEMBER);
    FSetAffilAdmin->setChecked(AUser->affiliation() == MUC_AFFIL_ADMIN);
    FSetAffilOwner->setChecked(AUser->affiliation() == MUC_AFFIL_OWNER);

    AMenu->addAction(FSetRoleNode,AG_MULTIUSERCHAT_ROOM_UTILS,false);
    AMenu->addAction(FSetAffilOutcast,AG_MULTIUSERCHAT_ROOM_UTILS,false);
    AMenu->addAction(FChangeRole->menuAction(),AG_MULTIUSERCHAT_ROOM_UTILS,false);
    AMenu->addAction(FChangeAffiliation->menuAction(),AG_MULTIUSERCHAT_ROOM_UTILS,false);
  }
}

void MultiUserChatWindow::saveWindowState()
{
  if (FSettings)
  {
    QString valueNameNS = roomJid().pBare();
    if (isWindow() && isVisible())
      FSettings->setValueNS(SVN_WINDOW_GEOMETRY,valueNameNS,saveGeometry());
    FSettings->setValueNS(SVN_WINDOW_HSPLITTER,valueNameNS,ui.sprHSplitter->saveState());
    FSettings->setValueNS(SVN_WINDOW_VSPLITTER,valueNameNS,ui.sprVSplitter->saveState());
  }
}

void MultiUserChatWindow::loadWindowState()
{
  QString valueNameNS = roomJid().pBare();
  if (isWindow())
    restoreGeometry(FSettings->valueNS(SVN_WINDOW_GEOMETRY,valueNameNS).toByteArray());
  if (!FSplitterLoaded)
  {
    ui.sprHSplitter->restoreState(FSettings->valueNS(SVN_WINDOW_HSPLITTER,valueNameNS).toByteArray());
    ui.sprVSplitter->restoreState(FSettings->valueNS(SVN_WINDOW_VSPLITTER,valueNameNS).toByteArray());
    FSplitterLoaded = true;
  }
  FEditWidget->textEdit()->setFocus();
}

bool MultiUserChatWindow::showStatusCodes(const QString &ANick, const QList<int> &ACodes)
{
  if (ACodes.isEmpty())
  {
    return false;
  }

  bool shown = false;
  if (ACodes.contains(MUC_SC_NON_ANONYMOUS))
  {
    showServiceMessage(tr("Any occupant is allowed to see the user's full JID"));
    shown = true;
  }
  if (ACodes.contains(MUC_SC_AFFIL_CHANGED))
  {
    showServiceMessage(tr("%1 affiliation changed while not in the room").arg(ANick));
    shown = true;
  }
  if (ACodes.contains(MUC_SC_CONFIG_CHANGED))
  {
    showServiceMessage(tr("Room configuration change has occurred"));
    shown = true;
  }
  if (ACodes.contains(MUC_SC_NOW_LOGGING_ENABLED))
  {
    showServiceMessage(tr("Room logging is now enabled"));
    shown = true;
  }
  if (ACodes.contains(MUC_SC_NOW_LOGGING_DISABLED))
  {
    showServiceMessage(tr("Room logging is now disabled"));
    shown = true;
  }
  if (ACodes.contains(MUC_SC_NOW_NON_ANONYMOUS))
  {
    showServiceMessage(tr("The room is now non-anonymous"));
    shown = true;
  }
  if (ACodes.contains(MUC_SC_NOW_SEMI_ANONYMOUS))
  {
    showServiceMessage(tr("The room is now semi-anonymous"));
    shown = true;
  }
  if (ACodes.contains(MUC_SC_NOW_FULLY_ANONYMOUS))
  {
    showServiceMessage(tr("The room is now fully-anonymous"));
    shown = true;
  }
  if (ACodes.contains(MUC_SC_ROOM_CREATED))
  {
    showServiceMessage(tr("A new room has been created"));
    shown = true;
  }
  if (ACodes.contains(MUC_SC_NICK_CHANGED))
  {
    shown = true;
  }
  if (ACodes.contains(MUC_SC_USER_BANNED))
  {
    shown = true;
  }
  if (ACodes.contains(MUC_SC_ROOM_ENTER))
  {
    shown = true;
  }
  if (ACodes.contains(MUC_SC_USER_KICKED))
  {
    shown = true;
  }
  if (ACodes.contains(MUC_SC_AFFIL_CHANGE))
  {
    showServiceMessage(tr("%1 has been removed from the room because of an affiliation change").arg(ANick));
    shown = true;
  }
  if (ACodes.contains(MUC_SC_MEMBERS_ONLY))
  {
    showServiceMessage(tr("%1 has been removed from the room because the room has been changed to members-only").arg(ANick));
    shown = true;
  }
  if (ACodes.contains(MUC_SC_SYSTEM_SHUTDOWN))
  {
    showServiceMessage(tr("%1 is being removed from the room because of a system shutdown").arg(ANick));
    shown = true;
  }
  return shown;
}

void MultiUserChatWindow::showServiceMessage(const QString &AMessage)
{
  QString html = QString("<span style='color:green;'>*** %1</span>").arg(AMessage);
  FViewWidget->showCustomMessage(html,QDateTime::currentDateTime());
}

void MultiUserChatWindow::setViewColorForUser(IMultiUser *AUser)
{
  if (FColorQueue.isEmpty())
  {
    FColorQueue << Qt::blue << Qt::darkBlue << Qt::darkGreen << Qt::darkCyan << Qt::darkMagenta << Qt::darkYellow 
                << Qt::green << Qt::cyan << Qt::magenta << Qt::darkRed;
  }

  QColor color;
  if (AUser != FMultiChat->mainUser())
  {
    if (!FColorLastOwner.values().contains(AUser->nickName()))
    {
      color = FColorQueue.takeFirst();
      FColorQueue.append(color);
    }
    else
      color = FColorLastOwner.key(AUser->nickName());
  }
  else
    color = Qt::red;

  FViewWidget->setColorForJid(AUser->contactJid(),color);
  AUser->setData(MUDR_VIEW_COLOR,color);
  FColorLastOwner.insert(color.name(),AUser->nickName());
}

void MultiUserChatWindow::setRoleColorForUser(IMultiUser *AUser)
{
  QListWidgetItem *listItem = FUsers.value(AUser);
  if (listItem)
  {
    QColor color;
    QString role = AUser->data(MUDR_ROLE).toString();
    if (role == MUC_ROLE_PARTICIPANT)
      color = Qt::black;
    else if (role == MUC_ROLE_MODERATOR)
      color = Qt::darkRed;
    else
      color = Qt::gray;
    listItem->setForeground(color);
    AUser->setData(MUDR_ROLE_COLOR,color);
  }
}

void MultiUserChatWindow::setAffilationLineForUser(IMultiUser *AUser)
{
  QListWidgetItem *listItem = FUsers.value(AUser);
  if (listItem)
  {
    QFont itemFont = listItem->font();
    QString affilation = AUser->data(MUDR_AFFILIATION).toString();
    if (affilation == MUC_AFFIL_OWNER)
    {
      itemFont.setStrikeOut(false);
      itemFont.setUnderline(true);
    }
    else if (affilation == MUC_AFFIL_OUTCAST)
    {
      itemFont.setStrikeOut(true);
      itemFont.setUnderline(false);
    }
    else
    {
      itemFont.setStrikeOut(false);
      itemFont.setUnderline(false);
    }
    listItem->setFont(itemFont);
  }
}

void MultiUserChatWindow::setToolTipForUser(IMultiUser *AUser)
{
  QListWidgetItem *listItem = FUsers.value(AUser);
  if (listItem)
  {
    QString toolTip;
    toolTip += AUser->nickName()+"<br>";
    if (!AUser->data(MUDR_REALJID).toString().isEmpty())
      toolTip += AUser->data(MUDR_REALJID).toString()+"<br>";
    toolTip += tr("Role: %1<br>").arg(AUser->data(MUDR_ROLE).toString());
    toolTip += tr("Affiliation: %1<br>").arg(AUser->data(MUDR_AFFILIATION).toString());
    toolTip += tr("Status: %1").arg(AUser->data(MUDR_STATUS).toString());
    listItem->setToolTip(toolTip);
  }
}

void MultiUserChatWindow::updateWindow()
{
  QIcon icon;
  SkinIconset *iconset = Skin::getSkinIconset(SYSTEM_ICONSETFILE);
  if (!FActiveMessages.isEmpty())
    icon = iconset->iconByName(IN_CHAT_MESSAGE);
  else
    icon = iconset->iconByName(IN_MULTICHAT_MESSAGE);

  QString roomName = tr("%1 (%2)").arg(FMultiChat->roomJid().node()).arg(FUsers.count());
  setWindowIcon(icon);
  setWindowIconText(roomName);
  setWindowTitle(tr("%1 - Groupchat").arg(roomName));

  emit windowChanged();
}

void MultiUserChatWindow::updateListItem(const Jid &AContactJid)
{
  IMultiUser *user = FMultiChat->userByNick(AContactJid.resource());
  QListWidgetItem *listItem = FUsers.value(user);
  if (listItem)
  {
    IChatWindow *window = findChatWindow(AContactJid);
    if (FActiveChatMessages.contains(window))
    {
      SkinIconset *iconset = Skin::getSkinIconset(SYSTEM_ICONSETFILE);
      listItem->setIcon(iconset->iconByName(IN_CHAT_MESSAGE));
    }
    else if (FStatusIcons)
      listItem->setIcon(FStatusIcons->iconByJidStatus(AContactJid,user->data(MUDR_SHOW).toInt(),"",false));
  }
}

void MultiUserChatWindow::removeActiveMessages()
{
  foreach(int messageId, FActiveMessages)
    FMessenger->removeMessage(messageId);
  FActiveMessages.clear();
  updateWindow();
}

IChatWindow *MultiUserChatWindow::getChatWindow(const Jid &AContactJid)
{
  IChatWindow *window = NULL;
  IMultiUser *user = FMultiChat->userByNick(AContactJid.resource());
  if (user && user != FMultiChat->mainUser())
  {
    window = FMessenger->newChatWindow(streamJid(),AContactJid);
    if (window)
    {
      connect(window,SIGNAL(messageReady()),SLOT(onChatMessageSend()));
      connect(window,SIGNAL(windowActivated()),SLOT(onChatWindowActivated()));
      connect(window,SIGNAL(windowClosed()),SLOT(onChatWindowClosed()));
      connect(window,SIGNAL(windowDestroyed()),SLOT(onChatWindowDestroyed()));
      window->viewWidget()->setNickForJid(user->contactJid(),user->nickName());
      window->viewWidget()->setColorForJid(user->contactJid(),user->data(MUDR_VIEW_COLOR).value<QColor>());
      window->infoWidget()->setFieldAutoUpdated(IInfoWidget::ContactName,false);
      window->infoWidget()->setFieldAutoUpdated(IInfoWidget::ContactShow,false);
      window->infoWidget()->setFieldAutoUpdated(IInfoWidget::ContactStatus,false);
      window->infoWidget()->setField(IInfoWidget::ContactName,user->nickName());
      window->infoWidget()->setField(IInfoWidget::ContactShow,user->data(MUDR_SHOW));
      window->infoWidget()->setField(IInfoWidget::ContactStatus,user->data(MUDR_STATUS));
      window->infoWidget()->autoUpdateFields();
      FChatWindows.append(window);
      updateChatWindow(window);
      emit chatWindowCreated(window);
    }
    else
      window = findChatWindow(AContactJid);
  }
  return window;
}

void MultiUserChatWindow::showChatWindow(IChatWindow *AWindow)
{
  if (AWindow->isWindow() && FMessenger->checkOption(IMessenger::OpenChatInTabWindow))
  {
    ITabWindow *tabWindow = FMessenger->openTabWindow();
    tabWindow->addWidget(AWindow);
  }
  AWindow->showWindow();
}

void MultiUserChatWindow::removeActiveChatMessages(IChatWindow *AWindow)
{
  if (FActiveChatMessages.contains(AWindow))
  {
    QList<int> messageIds = FActiveChatMessages.values(AWindow);
    foreach(int messageId, messageIds)
      FMessenger->removeMessage(messageId);
    FActiveChatMessages.remove(AWindow);
    updateChatWindow(AWindow);
    updateListItem(AWindow->contactJid());
  }
}

void MultiUserChatWindow::updateChatWindow(IChatWindow *AWindow)
{
  QIcon icon;
  if (FActiveChatMessages.contains(AWindow))
  {
    SkinIconset *iconset = Skin::getSkinIconset(SYSTEM_ICONSETFILE);
    icon = iconset->iconByName(IN_CHAT_MESSAGE);
  }
  else if (FStatusIcons)
    icon = FStatusIcons->iconByStatus(AWindow->infoWidget()->field(IInfoWidget::ContactShow).toInt(),"both",false);

  QString contactName = AWindow->infoWidget()->field(IInfoWidget::ContactName).toString();
  QString tabTitle = QString("[%1]").arg(contactName);
  AWindow->updateWindow(icon,tabTitle,tr("%1 - Private chat").arg(tabTitle));
}

bool MultiUserChatWindow::event(QEvent *AEvent)
{
  if (AEvent->type() == QEvent::WindowActivate)
    emit windowActivated();
  return QMainWindow::event(AEvent);
}

void MultiUserChatWindow::showEvent(QShowEvent *AEvent)
{
  loadWindowState();
  emit windowActivated();
  QMainWindow::showEvent(AEvent);
}

void MultiUserChatWindow::closeEvent(QCloseEvent *AEvent)
{
  saveWindowState();
  emit windowClosed();
  QMainWindow::closeEvent(AEvent);
}

bool MultiUserChatWindow::eventFilter(QObject *AObject, QEvent *AEvent)
{
  if (AObject == ui.ltwUsers)
  {
    if (AEvent->type() == QEvent::ContextMenu)
    {
      QContextMenuEvent *menuEvent = static_cast<QContextMenuEvent *>(AEvent);
      QListWidgetItem *listItem = ui.ltwUsers->itemAt(menuEvent->pos());
      IMultiUser *user = FUsers.key(listItem,NULL);
      if (user && user != FMultiChat->mainUser())
      {
        Menu *menu = new Menu(this);
        menu->setAttribute(Qt::WA_DeleteOnClose,true);
        insertRoomUtilsActions(menu,user);
        emit multiUserContextMenu(user,menu);
        if (!menu->isEmpty())
          menu->popup(menuEvent->globalPos());
        else
          delete menu;
      }
    }
  }
  return IMultiUserChatWindow::eventFilter(AObject,AEvent);
}

void MultiUserChatWindow::onChatOpened()
{
  FViewWidget->textBrowser()->clear();
  if (FMultiChat->statusCodes().contains(201))
    FMultiChat->requestConfigForm();
}

void MultiUserChatWindow::onChatNotify(const QString &ANick, const QString &ANotify)
{
  if (ANick.isEmpty())
    showServiceMessage(tr("Notify: %1").arg(ANotify));
  else
    showServiceMessage(tr("Notify from %1: %2").arg(ANick).arg(ANotify));
}

void MultiUserChatWindow::onChatError(const QString &ANick, const QString &AError)
{
  if (ANick.isEmpty())
    showServiceMessage(tr("Error: %1").arg(AError));
  else
    showServiceMessage(tr("Error from %1: %2").arg(ANick).arg(AError));
}

void MultiUserChatWindow::onChatClosed()
{
  FExitOnChatClosed ? deleteLater() : showServiceMessage(tr("Disconnected"));
}

void MultiUserChatWindow::onStreamJidChanged(const Jid &/*ABefour*/, const Jid &AAfter)
{
  FViewWidget->setStreamJid(AAfter);
  FEditWidget->setStreamJid(AAfter);
}

void MultiUserChatWindow::onUserPresence(IMultiUser *AUser, int AShow, const QString &AStatus)
{
  QListWidgetItem *listItem = FUsers.value(AUser);
  if (AShow!=IPresence::Offline && AShow!=IPresence::Error)
  {
    if (listItem == NULL)
    {
      listItem = new QListWidgetItem(ui.ltwUsers);
      listItem->setText(AUser->nickName());
      ui.ltwUsers->addItem(listItem);
      FUsers.insert(AUser,listItem);
      setViewColorForUser(AUser);
      setRoleColorForUser(AUser);
      setAffilationLineForUser(AUser);
      updateWindow();
      
      QString message = tr("%1 (%2) has joined the room. %3");
      message = message.arg(AUser->nickName());
      message = message.arg(AUser->data(MUDR_REALJID).toString());
      message = message.arg(AStatus);
      showServiceMessage(message);
    }
    showStatusCodes(AUser->nickName(),FMultiChat->statusCodes());
    setToolTipForUser(AUser);
    updateListItem(AUser->contactJid());
  }
  else if (listItem)
  {
    if (!showStatusCodes(AUser->nickName(),FMultiChat->statusCodes()))
    {
      QString message = tr("%1 (%2) has left the room. %3");
      message = message.arg(AUser->nickName());
      message = message.arg(AUser->data(MUDR_REALJID).toString());
      message = message.arg(AStatus.isEmpty() ? tr("Disconnected") : AStatus);
      showServiceMessage(message);
    }
    FUsers.remove(AUser);
    ui.ltwUsers->takeItem(ui.ltwUsers->row(listItem));
    delete listItem;
  }

  updateWindow();
  IChatWindow *window = findChatWindow(AUser->contactJid());
  if (window)
  {
    window->infoWidget()->setField(IInfoWidget::ContactShow,AShow);
    window->infoWidget()->setField(IInfoWidget::ContactStatus,AStatus);
    updateChatWindow(window);
  }
}

void MultiUserChatWindow::onUserDataChanged(IMultiUser *AUser, int ARole, const QVariant &ABefour, const QVariant &AAfter)
{
  if (ARole == MUDR_ROLE)
  {
    if (AAfter!=MUC_ROLE_NONE && ABefour!=MUC_ROLE_NONE)
      showServiceMessage(tr("%1 role changed from %2 to %3").arg(AUser->nickName()).arg(ABefour.toString()).arg(AAfter.toString()));
    setRoleColorForUser(AUser);
  }
  else if (ARole==MUDR_AFFILIATION)
  {
    if (FUsers.contains(AUser))
      showServiceMessage(tr("%1 affiliation changed from %2 to %3").arg(AUser->nickName()).arg(ABefour.toString()).arg(AAfter.toString()));
    setAffilationLineForUser(AUser);
  }
}

void MultiUserChatWindow::onUserNickChanged(IMultiUser *AUser, const QString &AOldNick, const QString &ANewNick)
{
  QListWidgetItem *listItem = FUsers.value(AUser);
  if (listItem)
  {
    listItem->setText(ANewNick);
    QColor color = AUser->data(MUDR_VIEW_COLOR).value<QColor>();
    FColorLastOwner.insert(color.name(),ANewNick);
    FViewWidget->setColorForJid(AUser->contactJid(),color);

    Jid userOldJid = AUser->contactJid();
    userOldJid.setResource(AOldNick);
    IChatWindow *window = findChatWindow(userOldJid);
    if (window)
    {
      window->setContactJid(AUser->contactJid());
      window->infoWidget()->setField(IInfoWidget::ContactName,ANewNick);
      window->viewWidget()->setNickForJid(AUser->contactJid(),ANewNick);
      window->viewWidget()->setColorForJid(AUser->contactJid(),color);
    }
  }
  if (AUser == FMultiChat->mainUser())
    ui.lblNick->setText(ANewNick);
  showServiceMessage(tr("%1 changed nick to %2").arg(AOldNick).arg(ANewNick));
}

void MultiUserChatWindow::onPresenceChanged(int /*AShow*/, const QString &/*AStatus*/)
{
  updateMenuBarActions();
}

void MultiUserChatWindow::onTopicChanged(const QString &ATopic)
{
  QString html = QString("<span style='color:gray;'>The topic has been set to: %1</span>").arg(ATopic);
  FViewWidget->showCustomMessage(html,QDateTime::currentDateTime());
}

void MultiUserChatWindow::onServiceMessageReceived(const Message &AMessage)
{
  if (!showStatusCodes("",FMultiChat->statusCodes()) && !AMessage.body().isEmpty())
    onMessageReceived("",AMessage);
}

void MultiUserChatWindow::onMessageReceived(const QString &ANick, const Message &AMessage)
{
  if (AMessage.type() == Message::GroupChat || ANick.isEmpty())
    FViewWidget->showMessage(AMessage);
  else
  {
    IChatWindow *window = getChatWindow(AMessage.from());
    if (window)
      window->viewWidget()->showMessage(AMessage);
  }
}

void MultiUserChatWindow::onUserKicked(const QString &ANick, const QString &AReason, const QString &AByUser)
{
  showServiceMessage(tr("%1 has been kicked from the room%2. %3").arg(ANick)
    .arg(AByUser.isEmpty() ? "" : tr(" by %1").arg(AByUser)).arg(AReason));
}

void MultiUserChatWindow::onUserBanned(const QString &ANick, const QString &AReason, const QString &AByUser)
{
  showServiceMessage(tr("%1 has been banned from the room%2. %3").arg(ANick)
    .arg(AByUser.isEmpty() ? "" : tr(" by %1").arg(AByUser)).arg(AReason));
}

void MultiUserChatWindow::onAffiliationListReceived(const QString &AAffiliation, const QList<IMultiUserListItem> &AList)
{
  EditUsersListDialog *dialog = new EditUsersListDialog(AAffiliation,AList,this);
  QString listName;
  if (AAffiliation == MUC_AFFIL_OUTCAST)
    listName = tr("Edit ban list - %1");
  else if (AAffiliation == MUC_AFFIL_MEMBER)
    listName = tr("Edit members list - %1");
  else if (AAffiliation == MUC_AFFIL_ADMIN)
    listName = tr("Edit administrators list - %1");
  else if (AAffiliation == MUC_AFFIL_OWNER)
    listName = tr("Edit owners list - %1");
  dialog->setTitle(listName.arg(roomJid().bare()));
  connect(dialog,SIGNAL(accepted()),SLOT(onAffiliationListDialogAccepted()));
  connect(FMultiChat->instance(),SIGNAL(chatClosed()),dialog,SLOT(reject()));
  dialog->show();
}

void MultiUserChatWindow::onConfigFormReceived(const QDomElement &AForm)
{
  if (FDataForms)
  {
    IDataDialog *dialog = FDataForms->newDataDialog(AForm,this);
    connect(dialog,SIGNAL(accepted()),SLOT(onConfigFormDialogAccepted()));
    connect(FMultiChat->instance(),SIGNAL(chatClosed()),dialog,SLOT(reject()));
    connect(FMultiChat->instance(),SIGNAL(configFormReceived(const QDomElement &)),dialog,SLOT(reject()));
    dialog->show();
  }
}

void MultiUserChatWindow::onMessageSend()
{
  if (FMultiChat->isOpen())
  {
    Message message;
    FMessenger->textToMessage(message,FEditWidget->document());
    if (!message.body().isEmpty() && FMultiChat->sendMessage(message))
      FEditWidget->clearEditor();
  }
}

void MultiUserChatWindow::onEditWidgetKeyEvent(QKeyEvent *AKeyEvent, bool &AHook)
{
  if (FMultiChat->isOpen() && AKeyEvent->modifiers()+AKeyEvent->key() == NICK_MENU_KEY)
  {
    Menu *nickMenu = new Menu(this);
    nickMenu->setAttribute(Qt::WA_DeleteOnClose,true);
    foreach(QListWidgetItem *listItem, FUsers)
    {
      if (listItem->text() != FMultiChat->mainUser()->nickName())
      {
        Action *action = new Action(nickMenu);
        action->setText(listItem->text());
        action->setIcon(listItem->icon());
        action->setData(ADR_USER_NICK,listItem->text());
        connect(action,SIGNAL(triggered(bool)),SLOT(onNickMenuActionTriggered(bool)));
        nickMenu->addAction(action,AG_DEFAULT,true);
      }
    }
    if (!nickMenu->isEmpty())
    {
      QTextEdit *textEdit = FEditWidget->textEdit();
      nickMenu->popup(textEdit->viewport()->mapToGlobal(textEdit->cursorRect().topLeft()));
    }
    else
      delete nickMenu;
    AHook = true;
  }
}

void MultiUserChatWindow::onWindowActivated()
{
  removeActiveMessages();
}

void MultiUserChatWindow::onChatMessageSend()
{
  IChatWindow *window = qobject_cast<IChatWindow *>(sender());
  if (window && FMultiChat->isOpen())
  {
    Message message;
    message.setType(Message::Chat);
    FMessenger->textToMessage(message,window->editWidget()->document());
    if (!message.body().isEmpty() && FMultiChat->sendMessage(message,window->contactJid().resource()))
    {
      window->viewWidget()->showMessage(message);
      window->editWidget()->clearEditor();
    }
  }
}

void MultiUserChatWindow::onChatWindowActivated()
{
  IChatWindow *window = qobject_cast<IChatWindow *>(sender());
  if (window)
    removeActiveChatMessages(window);
}

void MultiUserChatWindow::onChatWindowClosed()
{
  IChatWindow *window = qobject_cast<IChatWindow *>(sender());
  if (FChatWindows.contains(window) && !FMultiChat->userByNick(window->contactJid().resource()))
    window->deleteLater();
}

void MultiUserChatWindow::onChatWindowDestroyed()
{
  IChatWindow *window = qobject_cast<IChatWindow *>(sender());
  if (FChatWindows.contains(window))
  {
    removeActiveChatMessages(window);
    FChatWindows.removeAt(FChatWindows.indexOf(window));
    emit chatWindowDestroyed(window);
  }
}

void MultiUserChatWindow::onNickMenuActionTriggered(bool)
{
  Action *action = qobject_cast<Action *>(sender());
  if (action)
  {
    QString nick = action->data(ADR_USER_NICK).toString();
    FEditWidget->textEdit()->textCursor().insertText(tr("%1: ").arg(nick));
  }
}

void MultiUserChatWindow::onMenuBarActionTriggered(bool)
{
  Action *action = qobject_cast<Action *>(sender());
  if (action == FChangeNick)
  {
    QString nick = QInputDialog::getText(this,tr("Change nick name"),tr("Enter your new nick name in room %1").arg(roomJid().node()),
      QLineEdit::Normal,FMultiChat->nickName());
    if (!nick.isEmpty())
      FMultiChat->setNickName(nick);
  }
  else if (action == FChangeSubject)
  {
    if (FMultiChat->isOpen())
    {
      bool ok = false;
      QString subject = QInputDialog::getText(this,tr("Change subject"),tr("Enter new subject for room %1").arg(roomJid().node()),
        QLineEdit::Normal,FMultiChat->subject(),&ok);
      if (ok)
        FMultiChat->setSubject(subject);
    }
  }
  else if (action == FClearChat)
  {
    FViewWidget->textBrowser()->clear();
  }
  else if (action == FQuitRoom)
  {
    exitMultiUserChat();
  }
  else if (action == FRequestVoice)
  {
    FMultiChat->requestVoice();
  }
  else if (action == FBanList)
  {
    FMultiChat->requestAffiliationList(MUC_AFFIL_OUTCAST);
  }
  else if (action == FMembersList)
  {
    FMultiChat->requestAffiliationList(MUC_AFFIL_MEMBER);
  }
  else if (action == FAdminsList)
  {
    FMultiChat->requestAffiliationList(MUC_AFFIL_ADMIN);
  }
  else if (action == FOwnersList)
  {
    FMultiChat->requestAffiliationList(MUC_AFFIL_OWNER);
  }
  else if (action == FConfigRoom)
  {
    FMultiChat->requestConfigForm();
  }
}

void MultiUserChatWindow::onRoomUtilsActionTriggered(bool)
{
  Action *action =qobject_cast<Action *>(sender());
  if (action == FSetRoleNode)
  {
    bool ok;
    QString reason = QInputDialog::getText(this,tr("Kick reason"),tr("Enter reason for kick"),QLineEdit::Normal,"",&ok);
    if (ok)
      FMultiChat->setRole(FRoomUtilsMenu->menuAction()->data(ADR_USER_NICK).toString(),MUC_ROLE_NONE,reason);
  }
  else if (action == FSetRoleVisitor)
  {
    FMultiChat->setRole(FRoomUtilsMenu->menuAction()->data(ADR_USER_NICK).toString(),MUC_ROLE_VISITOR);
  }
  else if (action == FSetRoleParticipant)
  {
    FMultiChat->setRole(FRoomUtilsMenu->menuAction()->data(ADR_USER_NICK).toString(),MUC_ROLE_PARTICIPANT);
  }
  else if (action == FSetRoleModerator)
  {
    FMultiChat->setRole(FRoomUtilsMenu->menuAction()->data(ADR_USER_NICK).toString(),MUC_ROLE_MODERATOR);
  }
  else if (action == FSetAffilNone)
  {
    FMultiChat->setAffiliation(FRoomUtilsMenu->menuAction()->data(ADR_USER_NICK).toString(),MUC_AFFIL_NONE);
  }
  else if (action == FSetAffilOutcast)
  {
    bool ok;
    QString reason = QInputDialog::getText(this,tr("Ban reason"),tr("Enter reason for ban"),QLineEdit::Normal,"",&ok);
    if (ok)
      FMultiChat->setAffiliation(FRoomUtilsMenu->menuAction()->data(ADR_USER_NICK).toString(),MUC_AFFIL_OUTCAST,reason);
  }
  else if (action == FSetAffilMember)
  {
    FMultiChat->setAffiliation(FRoomUtilsMenu->menuAction()->data(ADR_USER_NICK).toString(),MUC_AFFIL_MEMBER);
  }
  else if (action == FSetAffilAdmin)
  {
    FMultiChat->setAffiliation(FRoomUtilsMenu->menuAction()->data(ADR_USER_NICK).toString(),MUC_AFFIL_ADMIN);
  }
  else if (action == FSetAffilOwner)
  { 
    FMultiChat->setAffiliation(FRoomUtilsMenu->menuAction()->data(ADR_USER_NICK).toString(),MUC_AFFIL_OWNER);
  }
}

void MultiUserChatWindow::onDataFormMessageDialogAccepted()
{
  IDataDialog *dialog = qobject_cast<IDataDialog *>(sender());
  if (dialog)
    FMultiChat->submitDataFormMessage(dialog->dataForm());
}

void MultiUserChatWindow::onAffiliationListDialogAccepted()
{
  EditUsersListDialog *dialog = qobject_cast<EditUsersListDialog *>(sender());
  if (dialog)
    FMultiChat->setAffiliationList(dialog->deltaList());
}

void MultiUserChatWindow::onConfigFormDialogAccepted()
{
  IDataDialog *dialog = qobject_cast<IDataDialog *>(sender());
  if (dialog)
    FMultiChat->submitConfigForm(dialog->dataForm());
}

void MultiUserChatWindow::onListItemActivated(QListWidgetItem *AItem)
{
  IMultiUser *user = FUsers.key(AItem);
  if (user)
    openWindow(streamJid(),user->contactJid(),Message::Chat);
}

void MultiUserChatWindow::onDefaultChatFontChanged(const QFont &AFont)
{
  FViewWidget->document()->setDefaultFont(AFont);
  FEditWidget->document()->setDefaultFont(AFont);
}

void MultiUserChatWindow::onStatusIconsChanged()
{
  foreach(IChatWindow *window,FChatWindows)
    updateChatWindow(window);
  foreach(IMultiUser *user, FUsers.keys())
    updateListItem(user->contactJid());
  updateWindow();
}

void MultiUserChatWindow::onAccountChanged(const QString &AName, const QVariant &AValue)
{
  if (AName == "name")
    ui.lblAccount->setText(Qt::escape(AValue.toString()));
}

