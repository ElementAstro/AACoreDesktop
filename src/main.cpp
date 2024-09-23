#include <QApplication>
#include <QScreen>

#include "ElaApplication.h"
#include "mainwindow.h"

#include "Components/SystemTray.h"

#include "ElaMenu.h"

int main(int argc, char* argv[]) {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#else
    // 根据实际屏幕缩放比例更改
    qputenv("QT_SCALE_FACTOR", "1.5");
#endif
#endif
    QApplication a(argc, argv);
    eApp->init();
    MainWindow w;
    w.show();
    // HWND handle = FindWindowA(NULL, "ElaWidgetTools");
    // if (handle != NULL)
    // {
    //     SetWindowDisplayAffinity(handle, WDA_EXCLUDEFROMCAPTURE);
    // }

    SystemTrayComponent trayComponent;

    // 初始化系统托盘图标和提示信息
    trayComponent.initTray(":/Resource/Image/icon.ico", "AACoreDesktop");

    // 添加菜单项
    trayComponent.addMenuItem("显示消息", [&trayComponent]() {
        trayComponent.showMessage("提示", "这是一个系统托盘消息");
    });

    // 添加子菜单
    ElaMenu* subMenu = trayComponent.addSubMenu("子菜单");
    subMenu->addAction("子菜单项1", []() {
        // 子菜单项1的回调函数
    });

    // 设置鼠标悬停提示信息
    trayComponent.setToolTip("新的提示信息");

    // 注册全局快捷键
    trayComponent.registerShortcut("Ctrl+Shift+T", []() {
        // 快捷键的回调函数
    });

    return a.exec();
}
