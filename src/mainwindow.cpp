#include "mainwindow.h"
#include <qobject.h>

#include <QDebug>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QSettings>
#include <QStackedWidget>
#include <QStatusBar>
#include <QVBoxLayout>

#include "Def.h"
#include "ElaContentDialog.h"
#include "ElaEventBus.h"
#include "ElaGraphicsItem.h"
#include "ElaGraphicsScene.h"
#include "ElaGraphicsView.h"
#include "ElaStatusBar.h"
#include "ElaText.h"

#include "Page/Config/T_ConfigPanel.h"

#include "Page/Debug/T_HttpClient.h"
#include "Page/Debug/T_TcpClient.h"
#include "Page/Debug/T_WebSocketClient.h"

#include "Page/Equipment/T_Camera.h"
#include "Page/Equipment/T_DeviceConnection.h"
#include "Page/Equipment/T_FilterWheel.h"
#include "Page/Equipment/T_Focuser.h"
#include "Page/Equipment/T_Guider.h"
#include "Page/Equipment/T_Switch.h"
#include "Page/Equipment/T_Telescope.h"

#include "Page/Helper/HelpWindow.h"

#include "Page/Image/T_ImageViewer.h"

#include "Page/Log/T_LogPanel.h"

#include "Plugin/PluginManagerPage.h"

#include "Page/Sequencer/T_SimpleSequencer.h"

#include "Page/Serial/T_SerialConfig.h"
#include "Page/Serial/T_SerialDebug.h"

#include "Page/Skymap/T_TargetSearch.h"

#include "Page/System/T_Process.hpp"
#include "Page/System/T_Software.hpp"
#include "Page/System/T_SystemInfo.h"
#include "Page/System/T_Terminal.h"

#include "Page/Utils/T_About.h"
#include "Page/Utils/T_I18n.h"

#include "Page/T_Home.h"

#include "Page/T_Setting.h"

namespace {
const int WindowWidth = 1200;
const int WindowHeight = 740;
const int StatusTextPixelSize = 14;
const int SceneRectSize = 1500;
const int ItemSize = 100;
const int AboutPageSize = 400;
}  // namespace

MainWindow::MainWindow(QWidget *parent)
    : ElaWindow(parent), i18nManager(new T_I18NPage(this)) {
    initWindow();

    // 额外布局
    initEdgeLayout();

    // 中心窗口
    initContent();

    // 拦截默认关闭事件
    _closeDialog = new ElaContentDialog(this);
    _closeDialog->setWindowTitle("关闭");
    _closeDialog->setLeftButtonText("取消");
    _closeDialog->setMiddleButtonText("最小化");
    _closeDialog->setRightButtonText("关闭");
    connect(_closeDialog, &ElaContentDialog::rightButtonClicked, this,
            &MainWindow::closeWindow);
    connect(_closeDialog, &ElaContentDialog::middleButtonClicked, this,
            &MainWindow::showMinimized);
    this->setIsDefaultClosed(false);
    connect(this, &MainWindow::closeButtonClicked, this,
            [=]() { _closeDialog->exec(); });

    // 移动到中心
    moveToCenter();
}

MainWindow::~MainWindow() = default;

void MainWindow::onCloseButtonClicked() {
    ElaContentDialog dialog(this);
    connect(&dialog, &ElaContentDialog::rightButtonClicked, this,
            &MainWindow::closeWindow);
    connect(&dialog, &ElaContentDialog::middleButtonClicked, this,
            &MainWindow::showMinimized);
    dialog.exec();
}

void MainWindow::initWindow() {
    setWindowIcon(QIcon(":/Resource/Image/icon.ico"));
    resize(WindowWidth, WindowHeight);
    // ElaLog::getInstance()->initMessageLog(true);
    // eApp->setThemeMode(ElaThemeType::Dark);
    // setIsNavigationBarEnable(false);
    // setNavigationBarDisplayMode(ElaNavigationType::Compact);
    // setWindowButtonFlag(ElaAppBarType::MinimizeButtonHint, false);
    setUserInfoCardPixmap(QPixmap(":/Resource/Image/icon.ico"));
    setUserInfoCardTitle("AACore Desktop");
    setUserInfoCardSubTitle("1.0.0");
    setWindowTitle("AACore Desktop");
    // setIsStayTop(true);
    // setUserInfoCardVisible(false);
}

void MainWindow::initEdgeLayout() {
    // 状态栏
    auto statusBar = new ElaStatusBar(this);
    auto statusText = new ElaText("初始化成功！", this);
    statusText->setTextPixelSize(StatusTextPixelSize);
    statusBar->addWidget(statusText);
    this->setStatusBar(statusBar);
}

void MainWindow::initContent() {
    _homePage = new T_Home(this);
    _deviceConnectionPage = new T_DeviceConnection(this);
    _cameraPage = new T_CameraPage(this);
    _telescopePage = new T_TelescopePage(this);
    _focuserPage = new T_FocuserPage(this);
    _filterWheelPage = new T_FilterWheelPage(this);
    _guiderPage = new T_GuiderPage(this);
    _simpleSequencerPage = new T_SimpleSequencerPage(this);
    _targetSearchPage = new T_TargetSearchPage(this);
    _configPanel = new T_ConfigPanel(this);
    _serialConfigPage = new T_SerialConfig(this);
    _serialDebugPage = new T_SerialDebugPage(this);
    _softwarePage = new T_SoftwarePage(this);
    _processPage = new T_ProcessPage(this);
    _systemInfoPage = new T_SystemInfoPage(this);
    _logPanelPage = new T_LogPanelPage(this);
    _settingPage = new T_Setting(this);
    _terminalPage = new T_TerminalPage(this);
    _imageViewerPage = new T_ImageViewerPage(this);
    _httpClientPage = new T_HttpClientPage(this);
    _webSocketClientPage = new T_WebSocketClientPage(this);
    _tcpClientPage = new T_TcpClientPage(this);
    _pluginManagerPage = new PluginManagerPage(this);
    _helpWindow = new HelpWindow(this);

    // GraphicsView
    auto scene = new ElaGraphicsScene(this);
    scene->setSceneRect(0, 0, SceneRectSize, SceneRectSize);
    auto item1 = new ElaGraphicsItem();
    item1->setWidth(ItemSize);
    item1->setHeight(ItemSize);
    item1->setMaxLinkPortCount(ItemSize);
    item1->setMaxLinkPortCount(1);
    auto item2 = new ElaGraphicsItem();
    item2->setWidth(ItemSize);
    item2->setHeight(ItemSize);
    scene->addItem(item1);
    scene->addItem(item2);
    auto view = new ElaGraphicsView(scene);
    view->setScene(scene);

    QString testKey2;
    QString testKey1;
    addPageNode("HOME", _homePage, ElaIconType::House);
    addPageNode("DeviceConnection", _deviceConnectionPage,
                ElaIconType::HardDrive);

    addExpanderNode("DeviceControl", testKey2, ElaIconType::ScrewdriverWrench);
    QString cameraKey;
    addPageNode("Camera", _cameraPage, testKey2, ElaIconType::Camera);
    addPageNode("Telescope", _telescopePage, testKey2, ElaIconType::Telescope);
    addPageNode("Focuser", _focuserPage, testKey2, ElaIconType::BracketsCurly);
    addPageNode("FilterWheel", _filterWheelPage, testKey2, ElaIconType::Filter);
    addPageNode("Guider", _guiderPage, testKey2, 0, ElaIconType::MapLocation);

    QString sequencerKey;
    addExpanderNode("Sequencer", sequencerKey, ElaIconType::BallotCheck);
    addPageNode("Sequencer", _simpleSequencerPage, sequencerKey,
                ElaIconType::BlockQuote);

    QString pluginKey;
    addExpanderNode("Plugin", pluginKey, ElaIconType::Plug);
    addPageNode("Plugin", _pluginManagerPage, pluginKey, ElaIconType::Plug);

    addPageNode("TargetSearch", _targetSearchPage,
                ElaIconType::MagnifyingGlassPlus);

    addPageNode("SerialConfig", _serialConfigPage, ElaIconType::GearComplex);
    addPageNode("SerialDebug", _serialDebugPage, ElaIconType::Plug);

    addPageNode("ImageViewer", _imageViewerPage, ElaIconType::Image);

    QString systemKey;
    addExpanderNode("System", systemKey, ElaIconType::SolarSystem);
    addPageNode("Software", _softwarePage, systemKey, ElaIconType::Grid2);
    addPageNode("Process", _processPage, systemKey, ElaIconType::BarsProgress);
    addPageNode("SystemInfo", _systemInfoPage, systemKey, ElaIconType::List);
    addPageNode("Terminal", _terminalPage, systemKey, ElaIconType::Terminal);

    addPageNode("Config", _configPanel, ElaIconType::GearComplex);

    addPageNode("Log", _logPanelPage, ElaIconType::List);

    QString debugKey;
    addExpanderNode("Debug", debugKey, ElaIconType::Bug);
    addPageNode("HttpClient", _httpClientPage, debugKey,
                ElaIconType::NetworkWired);
    addPageNode("TcpClient", _tcpClientPage, debugKey, ElaIconType::Plug);
    addPageNode("WebSocketClient", _webSocketClientPage, debugKey,
                ElaIconType::Cloud);

    QString helpKey;
    addExpanderNode("Help", helpKey, ElaIconType::Question);
    addPageNode("Help", _helpWindow, helpKey, ElaIconType::Book);

    addFooterNode("About", nullptr, _aboutKey, 0, ElaIconType::User);
    auto *aboutPage = new T_About();
    aboutPage->hide();
    connect(this, &ElaWindow::navigationNodeClicked, this,
            [=](ElaNavigationType::NavigationNodeType, const QString &nodeKey) {
                if (_aboutKey == nodeKey) {
                    aboutPage->setFixedSize(AboutPageSize, AboutPageSize);
                    aboutPage->moveToCenter();
                    aboutPage->show();
                }
            });
    addFooterNode("Setting", _settingPage, _settingKey, 0,
                  ElaIconType::GearComplex);
    addFooterNode("I18n", i18nManager, _i18nKey, 0, ElaIconType::Language);
    connect(this, &MainWindow::userInfoCardClicked, this, [=]() {
        this->navigation(_homePage->property("ElaPageKey").toString());
    });
    connect(_homePage, &T_Home::elaSceneNavigation, this, [=]() {
        this->navigation(view->property("ElaPageKey").toString());
    });
    qDebug() << "已注册的事件列表"
             << ElaEventBus::getInstance()->getRegisteredEventsName();
}

void MainWindow::changeLanguage(const QString &languageCode) {
    // 卸载当前翻译
    qApp->removeTranslator(&translator);

    // 加载新的翻译文件
    QString qmFile = ":/translations/translation_" + languageCode + ".qm";
    if (translator.load(qmFile)) {
        qApp->installTranslator(&translator);
        // 更新主窗口中的所有翻译字符串
        setWindowTitle(tr("I18N Manager"));
    }

    // 保存用户选择到 QSettings
    QSettings settings("MyCompany", "MyApp");
    settings.setValue("language", languageCode);
}