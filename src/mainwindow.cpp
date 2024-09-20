#include "mainwindow.h"

#include <QDebug>
#include <QGraphicsView>
#include <QHBoxLayout>
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
#include "Page/T_Guider.h"
#include "T_About.h"
#include "Page/T_Camera.h"
#include "Page/T_ConfigPanel.h"
#include "Page/T_DeviceConnection.h"
#include "Page/T_FilterWheel.h"
#include "Page/T_Focuser.h"
#include "Page/T_Home.h"
#include "Page/T_SimpleSequencer.h"
#include "Page/T_TargetSearch.h"
#include "Page/T_Telescope.h"

MainWindow::MainWindow(QWidget *parent) : ElaWindow(parent) {
  initWindow();

  // 额外布局
  initEdgeLayout();

  // 中心窗口
  initContent();

  // 拦截默认关闭事件
  this->setIsDefaultClosed(false);
  connect(this, &MainWindow::closeButtonClicked, this,
          &MainWindow::onCloseButtonClicked);

  // 移动到中心
  moveToCenter();
}

MainWindow::~MainWindow() {}

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
  resize(1200, 740);
  // ElaLog::getInstance()->initMessageLog(true);
  // eApp->setThemeMode(ElaThemeType::Dark);
  // setIsNavigationBarEnable(false);
  // setNavigationBarDisplayMode(ElaNavigationType::Compact);
  // setWindowButtonFlag(ElaAppBarType::MinimizeButtonHint, false);
  setUserInfoCardPixmap(QPixmap(":/Resource/Image/icon.ico"));
  setUserInfoCardTitle("Cobalt Desktop");
  setUserInfoCardSubTitle("1.0.0");
  setWindowTitle("Cobalt Desktop");
  // setIsStayTop(true);
  // setUserInfoCardVisible(false);
}

void MainWindow::initEdgeLayout() {
  // 状态栏
  ElaStatusBar *statusBar = new ElaStatusBar(this);
  ElaText *statusText = new ElaText("初始化成功！", this);
  statusText->setTextPixelSize(14);
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

  // GraphicsView
  ElaGraphicsScene *scene = new ElaGraphicsScene(this);
  scene->setSceneRect(0, 0, 1500, 1500);
  ElaGraphicsItem *item1 = new ElaGraphicsItem();
  item1->setWidth(100);
  item1->setHeight(100);
  item1->setMaxLinkPortCount(100);
  item1->setMaxLinkPortCount(1);
  ElaGraphicsItem *item2 = new ElaGraphicsItem();
  item2->setWidth(100);
  item2->setHeight(100);
  scene->addItem(item1);
  scene->addItem(item2);
  ElaGraphicsView *view = new ElaGraphicsView(scene);
  view->setScene(scene);

  QString testKey_1;
  QString testKey_2;
  addPageNode("HOME", _homePage, ElaIconType::House);
  addPageNode("DeviceConnection", _deviceConnectionPage,
              ElaIconType::HardDrive);

  addExpanderNode("DeviceControl", testKey_2, ElaIconType::ScrewdriverWrench);
  QString cameraKey;
  addPageNode("Camera", _cameraPage, testKey_2, ElaIconType::Camera);
  addPageNode("Telescope", _telescopePage, testKey_2, ElaIconType::Telescope);
  addPageNode("Focuser", _focuserPage, testKey_2, ElaIconType::BracketsCurly);
  addPageNode("FilterWheel", _filterWheelPage, testKey_2, ElaIconType::Filter);
  addPageNode("Guider", _guiderPage, testKey_2, 0, ElaIconType::MapLocation);

  QString sequencerKey;
  addExpanderNode("Sequencer", sequencerKey, ElaIconType::BallotCheck);
  addPageNode("Sequencer", _simpleSequencerPage, sequencerKey,
              ElaIconType::BlockQuote);

  addPageNode("TargetSearch", _targetSearchPage,
              ElaIconType::MagnifyingGlassPlus);

  addPageNode("Config", _configPanel, ElaIconType::GearComplex);

  addFooterNode("About", nullptr, _aboutKey, 0, ElaIconType::User);
  T_About *aboutPage = new T_About();
  aboutPage->hide();
  connect(this, &ElaWindow::navigationNodeClicked, this,
          [=](ElaNavigationType::NavigationNodeType nodeType, QString nodeKey) {
            if (_aboutKey == nodeKey) {
              aboutPage->setFixedSize(400, 400);
              aboutPage->moveToCenter();
              aboutPage->show();
            }
          });
  addFooterNode("Setting", new QWidget(this), _settingKey, 0,
                ElaIconType::GearComplex);
  connect(this, &MainWindow::userInfoCardClicked, this, [=]() {
    this->navigation(_homePage->property("ElaPageKey").toString());
  });
  connect(_homePage, &T_Home::elaSceneNavigation, this,
          [=]() { this->navigation(view->property("ElaPageKey").toString()); });
  qDebug() << "已注册的事件列表"
           << ElaEventBus::getInstance()->getRegisteredEventsName();
}
