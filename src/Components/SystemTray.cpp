#include "SystemTray.h"

#include <QApplication>
#include <QMessageBox>
#include <QShortcut>

#include "ElaMenu.h"

SystemTrayComponent::SystemTrayComponent(QObject *parent)
    : QObject(parent),
      trayIcon(new QSystemTrayIcon(this)),
      trayMenu(new ElaMenu()),
      animationTimer(new QTimer(this)),
      currentIconIndex(0) {
    connect(trayIcon, &QSystemTrayIcon::activated, this,
            &SystemTrayComponent::onTrayIconActivated);
    connect(animationTimer, &QTimer::timeout, this,
            &SystemTrayComponent::onIconAnimationTimeout);
}

SystemTrayComponent::~SystemTrayComponent() {
    delete trayMenu;
    delete trayIcon;
}

void SystemTrayComponent::initTray(const QString &iconPath,
                                   const QString &tooltip) {
    setTrayIcon(iconPath);
    setToolTip(tooltip);
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();
}

void SystemTrayComponent::addMenuItem(const QString &itemName,
                                      const std::function<void()> &callback) {
    QAction *action = new QAction(itemName, this);
    trayMenu->addAction(action);
    connect(action, &QAction::triggered, this, [callback]() { callback(); });
    actions.append(action);
}

ElaMenu *SystemTrayComponent::addSubMenu(const QString &menuName) {
    ElaMenu *subMenu = new ElaMenu(menuName, trayMenu);
    trayMenu->addMenu(subMenu);
    return subMenu;
}

void SystemTrayComponent::showMessage(const QString &title,
                                      const QString &message,
                                      QSystemTrayIcon::MessageIcon icon,
                                      int timeout) {
    trayIcon->showMessage(title, message, icon, timeout);
}

void SystemTrayComponent::setTrayIcon(const QString &iconPath) {
    trayIcon->setIcon(QIcon(iconPath));
}

void SystemTrayComponent::setToolTip(const QString &tooltip) {
    trayIcon->setToolTip(tooltip);
}

void SystemTrayComponent::startIconAnimation(const QStringList &iconPaths,
                                             int interval) {
    if (!iconPaths.isEmpty()) {
        iconAnimationList = iconPaths;
        currentIconIndex = 0;
        animationTimer->start(interval);
    }
}

void SystemTrayComponent::stopIconAnimation() { animationTimer->stop(); }

void SystemTrayComponent::registerShortcut(
    const QString &keySequence, const std::function<void()> &callback) {
    QShortcut *shortcut =
        new QShortcut(QKeySequence(keySequence), trayIcon->parent());
    connect(shortcut, &QShortcut::activated, this,
            [callback]() { callback(); });
}

void SystemTrayComponent::onTrayIconActivated(
    QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
        case QSystemTrayIcon::Trigger:  // 单击
            emit trayIconClicked();
            break;
        case QSystemTrayIcon::DoubleClick:  // 双击
            emit trayIconDoubleClicked();
            break;
        case QSystemTrayIcon::Context:  // 右击
            emit trayIconRightClicked();
            break;
        default:
            break;
    }
}

void SystemTrayComponent::onIconAnimationTimeout() {
    if (!iconAnimationList.isEmpty()) {
        setTrayIcon(iconAnimationList.at(currentIconIndex));
        currentIconIndex = (currentIconIndex + 1) % iconAnimationList.size();
    }
}
