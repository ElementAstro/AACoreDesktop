#ifndef SYSTEMTRAYCOMPONENT_H
#define SYSTEMTRAYCOMPONENT_H

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QShortcut>
#include <QSystemTrayIcon>
#include <QTimer>

class SystemTrayComponent : public QObject {
    Q_OBJECT
public:
    explicit SystemTrayComponent(QObject *parent = nullptr);
    ~SystemTrayComponent();

    // 初始化系统托盘
    void initTray(const QString &iconPath, const QString &tooltip);

    // 添加菜单项
    void addMenuItem(const QString &itemName,
                     const std::function<void()> &callback);

    // 添加子菜单
    QMenu *addSubMenu(const QString &menuName);

    // 动态更换托盘图标
    void setTrayIcon(const QString &iconPath);

    // 设置鼠标悬停提示信息
    void setToolTip(const QString &tooltip);

    // 显示气泡消息
    void showMessage(
        const QString &title, const QString &message,
        QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information,
        int timeout = 3000);

    // 开始图标闪烁/动画
    void startIconAnimation(const QStringList &iconPaths, int interval);

    // 停止图标闪烁/动画
    void stopIconAnimation();

    // 注册全局快捷键
    void registerShortcut(const QString &keySequence,
                          const std::function<void()> &callback);

private:
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QList<QAction *> actions;

    QTimer *animationTimer;
    QStringList iconAnimationList;
    int currentIconIndex;

signals:
    // 用于托盘图标点击和双击
    void trayIconClicked();
    void trayIconDoubleClicked();
    void trayIconRightClicked();

private slots:
    // 槽函数：处理托盘图标点击事件
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

    // 槽函数：处理图标动画切换
    void onIconAnimationTimeout();
};

#endif  // SYSTEMTRAYCOMPONENT_H
