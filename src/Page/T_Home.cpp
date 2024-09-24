#include "T_Home.h"

#include <QDebug>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QVBoxLayout>

#include "ElaAcrylicUrlCard.h"
#include "ElaFlowLayout.h"
#include "ElaImageCard.h"
#include "ElaMenu.h"
#include "ElaMessageBar.h"
#include "ElaNavigationRouter.h"
#include "ElaPopularCard.h"
#include "ElaScrollArea.h"
#include "ElaText.h"

namespace {
constexpr int kSpacing20 = 20;
constexpr int kSpacing15 = 15;
constexpr int kMargin20 = 20;
constexpr int kMargin40 = 40;
constexpr int kElaFlowLayoutSpacing = 5;
constexpr int kElaFlowLayoutMargin = 0;
constexpr int kMessageBarDuration = 2000;
}  // namespace

T_Home::T_Home(QWidget *parent) : ElaScrollPage(parent) {
    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("Home");
    auto *centerVLayout = new QVBoxLayout(centralWidget);
    centerVLayout->setContentsMargins(0, 0, 0, 0);

    // 背景图片（星空背景）
    auto *backgroundCard = new ElaImageCard(this);
    backgroundCard->setCardImage(
        QImage(":/Resource/Image/Home_Background.png"));

    auto *flowLayoutText = new ElaText("天文摄影工具", this);

    // 天文摄影工具卡片
    auto *starStackerCard = new ElaPopularCard(this);
    connect(
        starStackerCard, &ElaPopularCard::popularCardButtonClicked, this,
        [=]() {
            QDesktopServices::openUrl(QUrl("https://example.com/star-stacker"));
        });
    starStackerCard->setCardPixmap(QPixmap(":/Resource/Image/StarStacker.jpg"));
    starStackerCard->setTitle("星轨叠加器");
    starStackerCard->setSubTitle("5.0⭐ 图像处理");
    starStackerCard->setInteractiveTips("立即使用");
    starStackerCard->setDetailedText(
        "强大的星轨叠加工具，自动对齐和合并多张长曝"
        "光图像，创造出令人惊叹的星轨效果。");

    auto *deepSkyCard = new ElaPopularCard(this);
    connect(deepSkyCard, &ElaPopularCard::popularCardButtonClicked, this,
            [=]() {});
    deepSkyCard->setTitle("深空目标拍摄");
    deepSkyCard->setSubTitle("4.9⭐ 专业摄影");
    deepSkyCard->setCardPixmap(QPixmap(":/Resource/Image/DeepSky.png"));
    deepSkyCard->setInteractiveTips("开始拍摄");
    deepSkyCard->setDetailedText(
        "专为深空天体摄影设计，支持长时间曝光控制和自动导星功能。");

    auto *planetaryCard = new ElaPopularCard(this);
    connect(planetaryCard, &ElaPopularCard::popularCardButtonClicked, this,
            [=]() {});
    planetaryCard->setTitle("行星摄影");
    planetaryCard->setSubTitle("4.8⭐ 高速摄影");
    planetaryCard->setCardPixmap(QPixmap(":/Resource/Image/Planetary.png"));
    planetaryCard->setInteractiveTips("开始拍摄");
    planetaryCard->setDetailedText(
        "高速摄影模式，专为捕捉行星和月球细节而设计，支持视频模式和自动堆栈。");

    auto *processingCard = new ElaPopularCard(this);
    connect(processingCard, &ElaPopularCard::popularCardButtonClicked, this,
            [=]() {});
    processingCard->setTitle("后期处理");
    processingCard->setSubTitle("5.0⭐ 图像处理");
    processingCard->setCardPixmap(QPixmap(":/Resource/Image/Processing.png"));
    processingCard->setInteractiveTips("开始处理");
    processingCard->setDetailedText(
        "强大的后期处理工具，包括去噪、锐化、色彩校正"
        "等功能，让您的天文照片更加出色。");

    auto *flowLayout = new ElaFlowLayout(
        kElaFlowLayoutMargin, kElaFlowLayoutSpacing, kElaFlowLayoutSpacing);
    flowLayout->setIsAnimation(true);
    flowLayout->addWidget(starStackerCard);
    flowLayout->addWidget(deepSkyCard);
    flowLayout->addWidget(planetaryCard);
    flowLayout->addWidget(processingCard);

    centerVLayout->addWidget(backgroundCard);
    centerVLayout->addWidget(flowLayoutText);
    centerVLayout->addLayout(flowLayout);
    centerVLayout->setSpacing(kSpacing20);
    centerVLayout->addStretch();
    addCentralWidget(centralWidget);

    // 滚动区域中的卡片
    auto *tutorialCard = new ElaAcrylicUrlCard(this);
    tutorialCard->setUrl("https://example.com/astro-tutorials");
    tutorialCard->setCardPixmap(QPixmap(":/Resource/Image/Tutorial.png"));
    tutorialCard->setTitle("天文摄影教程");
    tutorialCard->setSubTitle("从新手到专家的完整指南");

    auto *communityCard = new ElaAcrylicUrlCard(this);
    communityCard->setUrl("https://example.com/astro-community");
    communityCard->setCardPixmap(QPixmap(":/Resource/Image/Community.png"));
    communityCard->setTitle("天文摄影社区");
    communityCard->setSubTitle("分享作品，交流技巧");

    auto *cardScrollArea = new ElaScrollArea(backgroundCard);
    cardScrollArea->setWidgetResizable(true);
    cardScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    cardScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    cardScrollArea->setIsGrabGesture(Qt::Horizontal, true);
    auto *cardScrollAreaWidget = new QWidget(this);
    cardScrollAreaWidget->setStyleSheet("background-color:transparent;");
    cardScrollArea->setWidget(cardScrollAreaWidget);
    auto *cardScrollAreaWidgetLayout = new QHBoxLayout(cardScrollAreaWidget);
    cardScrollAreaWidgetLayout->setSpacing(kSpacing15);
    cardScrollAreaWidgetLayout->setContentsMargins(kMargin20, kMargin40, 0, 0);
    cardScrollAreaWidgetLayout->addWidget(tutorialCard);
    cardScrollAreaWidgetLayout->addWidget(communityCard);
    cardScrollAreaWidgetLayout->addStretch();

    auto *backgroundLayout = new QHBoxLayout(backgroundCard);
    backgroundLayout->addWidget(cardScrollArea);
    backgroundLayout->setContentsMargins(0, 0, 0, 0);

    // 菜单
    _homeMenu = new ElaMenu(this);
    auto *viewMenu = _homeMenu->addMenu(ElaIconType::Cubes, "查看");
    viewMenu->addAction("图库");
    viewMenu->addAction("设备管理");
    viewMenu->addAction("天文日历");
    viewMenu->addAction("天气预报");

    _homeMenu->addSeparator();
    _homeMenu->addElaIconAction(ElaIconType::BoxCheck, "排序方式",
                                QKeySequence::Save);
    _homeMenu->addElaIconAction(ElaIconType::ArrowRotateRight, "刷新");
    auto *action =
        _homeMenu->addElaIconAction(ElaIconType::ArrowRotateLeft, "返回");
    connect(action, &QAction::triggered, this, [=]() {
        ElaNavigationRouter::getInstance()->navigationRouteBack();
    });

    _homeMenu->addElaIconAction(ElaIconType::Copy, "复制");
    _homeMenu->addElaIconAction(ElaIconType::MagnifyingGlassPlus, "显示设置");

    // 初始化提示
    ElaMessageBar::success(ElaMessageBarType::BottomRight, "Success",
                           "欢迎使用天文摄影软件!", kMessageBarDuration);
    qDebug() << "天文摄影软件初始化成功";
}

T_Home::~T_Home() = default;

void T_Home::mouseReleaseEvent(QMouseEvent *event) {
    switch (event->button()) {
        case Qt::LeftButton: {
            // ElaMessageBar::success(ElaMessageBarType::TopRight, "Success",
            // "Never Close Your Eyes", 2500);
            // ElaMessageBar::success(ElaMessageBarType::TopRight, "Success",
            // "Never Close Your Eyes", 1500);
            break;
        }
        case Qt::RightButton: {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            _homeMenu->popup(event->globalPosition().toPoint());
#else
            _homeMenu->popup(event->globalPos());
#endif
            break;
        }
        case Qt::BackButton: {
            this->navigation(0);
            break;
        }
        case Qt::ForwardButton: {
            this->navigation(1);
            break;
        }
        case Qt::MiddleButton: {
            this->navigation(2);
            break;
        }
        default: {
            break;
        }
    }
    ElaScrollPage::mouseReleaseEvent(event);
}