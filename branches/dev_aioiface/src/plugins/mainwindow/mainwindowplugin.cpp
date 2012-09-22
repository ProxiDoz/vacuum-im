#include "mainwindowplugin.h"

#include <QApplication>

MainWindowPlugin::MainWindowPlugin()
{
	FPluginManager = NULL;
	FOptionsManager = NULL;
	FTrayManager = NULL;

	FActivationChanged = QTime::currentTime();
	FMainWindow = new MainWindow(NULL,Qt::Window);
	FMainWindow->installEventFilter(this);
	WidgetManager::setWindowSticky(FMainWindow,true);
}

MainWindowPlugin::~MainWindowPlugin()
{
	delete FMainWindow;
}

void MainWindowPlugin::pluginInfo(IPluginInfo *APluginInfo)
{
	APluginInfo->name = tr("Main Window");
	APluginInfo->description = tr("Allows other modules to place their widgets in the main window");
	APluginInfo->version = "1.0";
	APluginInfo->author = "Potapov S.A. aka Lion";
	APluginInfo->homePage = "http://www.vacuum-im.org";
}

bool MainWindowPlugin::initConnections(IPluginManager *APluginManager, int &AInitOrder)
{
	Q_UNUSED(AInitOrder);
	FPluginManager = APluginManager;

	IPlugin *plugin = FPluginManager->pluginInterface("IOptionsManager").value(0,NULL);
	if (plugin)
	{
		FOptionsManager = qobject_cast<IOptionsManager *>(plugin->instance());
		if (FOptionsManager)
		{
			connect(FOptionsManager->instance(), SIGNAL(profileRenamed(const QString &, const QString &)),
				SLOT(onProfileRenamed(const QString &, const QString &)));
		}
	}

	plugin = APluginManager->pluginInterface("ITrayManager").value(0,NULL);
	if (plugin)
	{
		FTrayManager = qobject_cast<ITrayManager *>(plugin->instance());
		if (FTrayManager)
		{
			connect(FTrayManager->instance(),SIGNAL(notifyActivated(int, QSystemTrayIcon::ActivationReason)),
				SLOT(onTrayNotifyActivated(int,QSystemTrayIcon::ActivationReason)));
		}
	}

	connect(Options::instance(),SIGNAL(optionsOpened()),SLOT(onOptionsOpened()));
	connect(Options::instance(),SIGNAL(optionsClosed()),SLOT(onOptionsClosed()));
	connect(Options::instance(),SIGNAL(optionsChanged(const OptionsNode &)),SLOT(onOptionsChanged(const OptionsNode &)));

	connect(FPluginManager->instance(),SIGNAL(shutdownStarted()),SLOT(onShutdownStarted()));
	connect(Shortcuts::instance(),SIGNAL(shortcutActivated(const QString, QWidget *)),SLOT(onShortcutActivated(const QString, QWidget *)));

	return true;
}

bool MainWindowPlugin::initObjects()
{
	Shortcuts::declareShortcut(SCT_GLOBAL_SHOWROSTER,tr("Show roster"),QKeySequence::UnknownKey,Shortcuts::GlobalShortcut);

	Shortcuts::declareGroup(SCTG_MAINWINDOW, tr("Main window"), SGO_MAINWINDOW);
	Shortcuts::declareShortcut(SCT_MAINWINDOW_CLOSEWINDOW,tr("Hide roster"),tr("Esc","Hide roster"));

	Action *action = new Action(this);
	action->setText(tr("Quit"));
	action->setIcon(RSR_STORAGE_MENUICONS,MNI_MAINWINDOW_QUIT);
	connect(action,SIGNAL(triggered()),FPluginManager->instance(),SLOT(quit()));
	FMainWindow->mainMenu()->addAction(action,AG_MMENU_MAINWINDOW,true);

	if (FTrayManager)
	{
		action = new Action(this);
		action->setText(tr("Show roster"));
		action->setIcon(RSR_STORAGE_MENUICONS,MNI_MAINWINDOW_SHOW_ROSTER);
		connect(action,SIGNAL(triggered(bool)),SLOT(onShowMainWindowByAction(bool)));
		FTrayManager->contextMenu()->addAction(action,AG_TMTM_MAINWINDOW,true);
	}

	Shortcuts::insertWidgetShortcut(SCT_MAINWINDOW_CLOSEWINDOW,FMainWindow);

	return true;
}

bool MainWindowPlugin::initSettings()
{
	Options::setDefaultValue(OPV_MAINWINDOW_SHOWONSTART,true);
	Options::setDefaultValue(OPV_MAINWINDOW_CENTRALVISIBLE,true);
	return true;
}

bool MainWindowPlugin::startPlugin()
{
	Shortcuts::setGlobalShortcut(SCT_GLOBAL_SHOWROSTER,true);

	updateTitle();
	return true;
}

IMainWindow *MainWindowPlugin::mainWindow() const
{
	return FMainWindow;
}

void MainWindowPlugin::updateTitle()
{
	if (FOptionsManager && FOptionsManager->isOpened())
		FMainWindow->setWindowTitle(CLIENT_NAME" - "+FOptionsManager->currentProfile());
	else
		FMainWindow->setWindowTitle(CLIENT_NAME);
}

bool MainWindowPlugin::eventFilter(QObject *AWatched, QEvent *AEvent)
{
	if (AWatched==FMainWindow && AEvent->type()==QEvent::ActivationChange)
		FActivationChanged = QTime::currentTime();
	return QObject::eventFilter(AWatched,AEvent);
}

void MainWindowPlugin::onOptionsOpened()
{
	FMainWindow->loadWindowGeometryAndState();
	onOptionsChanged(Options::node(OPV_MAINWINDOW_CENTRALVISIBLE));
	if (Options::node(OPV_MAINWINDOW_SHOWONSTART).value().toBool())
		FMainWindow->showWindow();
	updateTitle();
}

void MainWindowPlugin::onOptionsClosed()
{
	FMainWindow->saveWindowGeometryAndState();
	FMainWindow->closeWindow();
	updateTitle();
}

void MainWindowPlugin::onOptionsChanged(const OptionsNode &ANode)
{
	if (ANode.path() == OPV_MAINWINDOW_CENTRALVISIBLE)
	{
		FMainWindow->setCentralWidgetVisible(ANode.value().toBool());
	}
}

void MainWindowPlugin::onShutdownStarted()
{
	Options::node(OPV_MAINWINDOW_SHOWONSTART).setValue(FMainWindow->isVisible());
}

void MainWindowPlugin::onProfileRenamed(const QString &AProfile, const QString &ANewName)
{
	Q_UNUSED(AProfile);
	Q_UNUSED(ANewName);
	updateTitle();
}

void MainWindowPlugin::onTrayNotifyActivated(int ANotifyId, QSystemTrayIcon::ActivationReason AReason)
{
	if (ANotifyId<=0 && AReason==QSystemTrayIcon::Trigger)
	{
		if (FMainWindow->isActive() || qAbs(FActivationChanged.msecsTo(QTime::currentTime())) < qApp->doubleClickInterval())
			FMainWindow->closeWindow();
		else
			FMainWindow->showWindow();
	}
}

void MainWindowPlugin::onShowMainWindowByAction(bool)
{
	FMainWindow->showWindow();
}

void MainWindowPlugin::onShortcutActivated(const QString &AId, QWidget *AWidget)
{
	if (AWidget==NULL && AId==SCT_GLOBAL_SHOWROSTER)
	{
		FMainWindow->showWindow();
	}
	else if (AWidget==FMainWindow && AId==SCT_MAINWINDOW_CLOSEWINDOW)
	{
		//FMainWindow->closeWindow();
		FMainWindow->setCentralWidgetVisible(!FMainWindow->isCentralWidgetVisible());
	}
}

Q_EXPORT_PLUGIN2(plg_mainwindow, MainWindowPlugin)
