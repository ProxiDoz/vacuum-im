#ifndef ITRAYMANAGER_H
#define ITRAYMANAGER_H

#include <QSystemTrayIcon>
#include <utils/menu.h>

#define TRAYMANAGER_UUID "{DF738BB8-B22B-4307-9EF8-F5833D7D2204}"

class ITrayManager {
public:
	virtual QObject *instance() =0;
	virtual QIcon currentIcon() const =0;
	virtual QString currentToolTip() const =0;
	virtual int currentNotify() const =0;
	virtual QIcon mainIcon() const =0;
	virtual void setMainIcon(const QIcon &AIcon) =0;
	virtual QString mainToolTip() const =0;
	virtual void setMainToolTip(const QString &AToolTip) =0;
	virtual void showMessage(const QString &ATitle, const QString &AMessage,
	                         QSystemTrayIcon::MessageIcon AIcon = QSystemTrayIcon::Information, int ATimeout = 10000) =0;
	virtual void addAction(Action *AAction, int AGroup = AG_DEFAULT, bool ASort = false) =0;
	virtual void removeAction(Action *AAction) =0;
	virtual int appendNotify(const QIcon &AIcon, const QString &AToolTip, bool ABlink) =0;
	virtual void updateNotify(int ANotifyId, const QIcon &AIcon, const QString &AToolTip, bool ABlink) =0;
	virtual void removeNotify(int ANotifyId) =0;
protected:
	virtual void messageClicked() =0;
	virtual void messageShown(const QString &ATitle, const QString &AMessage,QSystemTrayIcon::MessageIcon AIcon, int ATimeout) =0;
	virtual void notifyAdded(int ANotifyId) =0;
	virtual void notifyActivated(int ANotifyId, QSystemTrayIcon::ActivationReason AReason) =0;
	virtual void notifyRemoved(int ANotifyId) =0;
};

Q_DECLARE_INTERFACE(ITrayManager,"Vacuum.Plugin.ITrayManager/1.0")

#endif