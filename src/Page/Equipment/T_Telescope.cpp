#include "T_Telescope.h"

#include <QDateTime>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QPainter>
#include <QRandomGenerator>

#include "Components/C_InfoCard.h"
#include "Def.h"
#include "ElaComboBox.h"
#include "ElaIconButton.h"
#include "ElaPushButton.h"
#include "ElaSlider.h"
#include "ElaSpinBox.h"
#include "ElaTabWidget.h"
#include "ElaText.h"
#include "ElaToggleSwitch.h"


namespace {
    constexpr int kSpacing20 = 20;
    constexpr int kSpacing10 = 10;
    constexpr int kFixedSize40 = 40;
    constexpr int kMargin20 = 20;
    constexpr int kMargin10 = 10;
    constexpr int kTextPixelSize14 = 14;
    constexpr int kTextPixelSize12 = 12;
    constexpr int kTextPixelSize16 = 16;
    constexpr int kIconSize24 = 24;
    constexpr int kRateRange = 100;
    constexpr int kDefaultMainRate = 20;
    constexpr int kDefaultSecondaryRate = 20;
}

T_TelescopePage::T_TelescopePage(QWidget *parent) : T_BasePage(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(kSpacing20);
    mainLayout->setContentsMargins(kMargin20, kMargin20, kMargin20, kMargin20);

    // 顶部部分
    QHBoxLayout *topLayout = createTopLayout();
    mainLayout->addLayout(topLayout);

    // 创建标签页控件
    ElaTabWidget *tabWidget = new ElaTabWidget(this);

    // 创建并添加"信息"标签页
    QWidget *infoTab = createInfoTab();
    tabWidget->addTab(infoTab, "望远镜信息");

    // 创建并添加"控制"标签页
    QWidget *controlTab = createControlTab();
    tabWidget->addTab(controlTab, "望远镜控制");

    // 创建并添加"设置"标签页
    QWidget *settingsTab = createSettingsTab();
    tabWidget->addTab(settingsTab, "设置");

    mainLayout->addWidget(tabWidget);

    // 跟踪状态更新定时器
    //_trackingTimer = new QTimer(this);
    //connect(_trackingTimer, &QTimer::timeout, this, &T_TelescopePage::updateTrackingStatus);
    //_trackingTimer->start(1000); // 每秒更新一次

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("望远镜");
    QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

T_TelescopePage::~T_TelescopePage() {}

QHBoxLayout *T_TelescopePage::createTopLayout() {
    QHBoxLayout *topLayout = new QHBoxLayout();

    // 望远镜选择下拉框
    ElaComboBox *telescopeCombo = new ElaComboBox(this);
    telescopeCombo->addItem("Telescope Simulator for .NET");
    connect(telescopeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index){
                // 处理望远镜选择变化
            });

    // 创建图标按钮的辅助函数
    auto createIconButton = [this](ElaIconType::IconName icon) -> ElaIconButton* {
        ElaIconButton *button = new ElaIconButton(icon, this);
        button->setFixedSize(kFixedSize40, kFixedSize40);
        return button;
    };

    // 创建电源、刷新和设置按钮
    _powerButton = createIconButton(ElaIconType::PowerOff);
    _refreshButton = createIconButton(ElaIconType::ArrowsRotate);
    ElaIconButton *settingsButton = createIconButton(ElaIconType::Gears);

    // 连接按钮信号到槽函数
    connect(_powerButton, &ElaIconButton::clicked, this, &T_TelescopePage::onPowerButtonClicked);
    connect(_refreshButton, &ElaIconButton::clicked, this, &T_TelescopePage::onRefreshButtonClicked);
    connect(settingsButton, &ElaIconButton::clicked, this, [this]() {
        // 打开设置对话框或执行相关操作
    });

    topLayout->addWidget(telescopeCombo, 1);
    topLayout->addWidget(settingsButton);
    topLayout->addWidget(_refreshButton);
    topLayout->addWidget(_powerButton);

    return topLayout;
}

QWidget *T_TelescopePage::createInfoTab() {
    QWidget *infoWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(infoWidget);
    layout->setSpacing(kSpacing10);
    layout->setContentsMargins(kMargin10, kMargin10, kMargin10, kMargin10);

    // 基本信息组
    QGroupBox *basicInfoGroup = createInfoGroup("基本信息");
    QGridLayout *basicInfoLayout = new QGridLayout(basicInfoGroup);
    basicInfoLayout->addWidget(
        new InfoCard("望远镜名称", "Telescope Simulator for .NET", this), 0, 0);
    basicInfoLayout->addWidget(
        new InfoCard("描述", "Software Telescope Simulator for ASCOM", this), 0, 1);
    basicInfoLayout->addWidget(
        new InfoCard("驱动信息",
                       "ASCOM.Simulator.Telescope, Version=6.6.0.0, "
                       "Culture=neutral, PublicKeyToken=565de79389",
                       this),
        1, 0, 1, 2);
    basicInfoLayout->addWidget(new InfoCard("驱动版本", "6.6", this), 2, 0);

    // 站点信息组
    QGroupBox *siteInfoGroup = createInfoGroup("站点信息");
    QGridLayout *siteInfoLayout = new QGridLayout(siteInfoGroup);
    siteInfoLayout->addWidget(new InfoCard("纬度", "51° 04' 43\"", this), 0, 0);
    siteInfoLayout->addWidget(new InfoCard("经度", "-00° 17' 40\"", this), 0, 1);
    siteInfoLayout->addWidget(new InfoCard("海拔", "244.0 m", this), 1, 0);
    siteInfoLayout->addWidget(new InfoCard("纪元", "J2000", this), 1, 1);

    layout->addWidget(basicInfoGroup);
    layout->addWidget(siteInfoGroup);
    layout->addStretch();

    return infoWidget;
}

QWidget *T_TelescopePage::createControlTab() {
    QWidget *controlWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(controlWidget);
    layout->setSpacing(kSpacing20);
    layout->setContentsMargins(kMargin10, kMargin10, kMargin10, kMargin10);

    // 坐标控制组
    QGroupBox *coordGroup = createInfoGroup("手动坐标 (J2000)");
    QGridLayout *coordLayout = new QGridLayout(coordGroup);
    addCoordinateRow(coordLayout, "目标赤经", "12h", "34m", "56.7s");
    addCoordinateRow(coordLayout, "目标赤纬", "12°", "34'", "56\"");

    // 指向按钮
    _slewButton = new ElaPushButton("指向", this);
    _slewButton->setFixedHeight(40);
    connect(_slewButton, &ElaPushButton::clicked, this, &T_TelescopePage::onSlewButtonClicked);
    coordLayout->addWidget(_slewButton, 2, 3);

    // 手动控制组
    QGroupBox *manualControlGroup = createInfoGroup("手动控制");
    QVBoxLayout *manualControlLayout = new QVBoxLayout(manualControlGroup);
    manualControlLayout->setSpacing(kSpacing10);

    // 跟踪速率选择
    QHBoxLayout *trackingLayout = new QHBoxLayout();
    ElaText *trackingLabel = new ElaText("跟踪速率:", this);
    _trackingRateCombo = new ElaComboBox(this);
    _trackingRateCombo->addItem("无跟踪");
    _trackingRateCombo->addItem("赤道跟踪");
    _trackingRateCombo->addItem("银河跟踪");
    connect(_trackingRateCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &T_TelescopePage::updateTrackingStatus);
    trackingLayout->addWidget(trackingLabel);
    trackingLayout->addWidget(_trackingRateCombo);
    trackingLayout->addStretch();

    manualControlLayout->addLayout(trackingLayout);

    // 方向控制按钮
    QGridLayout *directionLayout = new QGridLayout();
    ElaIconButton *northButton = new ElaIconButton(ElaIconType::ArrowUp, this);
    ElaIconButton *westButton = new ElaIconButton(ElaIconType::ArrowLeft, this);
    ElaIconButton *stopButton = new ElaIconButton(ElaIconType::Stop, this);
    ElaIconButton *eastButton = new ElaIconButton(ElaIconType::ArrowRight, this);
    ElaIconButton *southButton = new ElaIconButton(ElaIconType::ArrowDown, this);

    northButton->setFixedSize(60, 60);
    westButton->setFixedSize(60, 60);
    stopButton->setFixedSize(60, 60);
    eastButton->setFixedSize(60, 60);
    southButton->setFixedSize(60, 60);

    connect(northButton, &ElaIconButton::clicked, this, [this]() { onDirectionButtonClicked("N"); });
    connect(westButton, &ElaIconButton::clicked, this, [this]() { onDirectionButtonClicked("W"); });
    connect(stopButton, &ElaIconButton::clicked, this, [this]() { onDirectionButtonClicked("Stop"); });
    connect(eastButton, &ElaIconButton::clicked, this, [this]() { onDirectionButtonClicked("E"); });
    connect(southButton, &ElaIconButton::clicked, this, [this]() { onDirectionButtonClicked("S"); });

    directionLayout->addWidget(northButton, 0, 1);
    directionLayout->addWidget(westButton, 1, 0);
    directionLayout->addWidget(stopButton, 1, 1);
    directionLayout->addWidget(eastButton, 1, 2);
    directionLayout->addWidget(southButton, 2, 1);

    manualControlLayout->addLayout(directionLayout);

    // 速率控制
    addRateControl(manualControlLayout, "主速率", kDefaultMainRate);
    addRateControl(manualControlLayout, "次速率", kDefaultSecondaryRate);

    // 反向切换
    addReversedToggle(manualControlLayout, "主轴反向");
    addReversedToggle(manualControlLayout, "次轴反向");

    // 停靠控制
    QHBoxLayout *parkLayout = new QHBoxLayout();
    _parkButton = new ElaPushButton("复位", this);
    _setParkPointButton = new ElaPushButton("设为复位点", this);
    _homeButton = new ElaPushButton("返回原点", this);
    _parkButton->setFixedHeight(40);
    _setParkPointButton->setFixedHeight(40);
    _homeButton->setFixedHeight(40);

    connect(_parkButton, &ElaPushButton::clicked, this, &T_TelescopePage::onParkButtonClicked);
    connect(_setParkPointButton, &ElaPushButton::clicked, this, &T_TelescopePage::onSetParkPointClicked);
    connect(_homeButton, &ElaPushButton::clicked, this, &T_TelescopePage::onHomeButtonClicked);

    parkLayout->addWidget(_parkButton);
    parkLayout->addWidget(_setParkPointButton);
    parkLayout->addWidget(_homeButton);
    parkLayout->addStretch();

    manualControlLayout->addLayout(parkLayout);

    layout->addWidget(coordGroup);
    layout->addWidget(manualControlGroup);
    layout->addStretch();

    return controlWidget;
}

QWidget *T_TelescopePage::createSettingsTab() {
    QWidget *settingsWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(settingsWidget);
    layout->setSpacing(kSpacing10);
    layout->setContentsMargins(kMargin10, kMargin10, kMargin10, kMargin10);

    // 暂无具体设置项，可以根据需求添加
    ElaText *settingsText = new ElaText("暂无设置选项", this);
    settingsText->setTextPixelSize(kTextPixelSize14);
    settingsText->setAlignment(Qt::AlignCenter);
    layout->addWidget(settingsText);
    layout->addStretch();

    return settingsWidget;
}

QGroupBox *T_TelescopePage::createInfoGroup(const QString &title) {
    QGroupBox *group = new QGroupBox(title, this);
    group->setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            border: 1px solid #bbb;
            border-radius: 5px;
            margin-top: 10px;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 3px 0 3px;
        }
    )");
    return group;
}

void T_TelescopePage::addCoordinateRow(QGridLayout *layout, const QString &label,
                                       const QString &value1, const QString &value2,
                                       const QString &value3) {
    int row = layout->rowCount();
    ElaText *labelWidget = new ElaText(label, this);
    QLineEdit *edit1 = new QLineEdit(value1, this);
    QLineEdit *edit2 = new QLineEdit(value2, this);
    QLineEdit *edit3 = new QLineEdit(value3, this);
    edit1->setFixedWidth(60);
    edit2->setFixedWidth(60);
    edit3->setFixedWidth(60);
    layout->addWidget(labelWidget, row, 0);
    layout->addWidget(edit1, row, 1);
    layout->addWidget(edit2, row, 2);
    layout->addWidget(edit3, row, 3);
}

void T_TelescopePage::addRateControl(QVBoxLayout *layout, const QString &label,
                                     double defaultValue) {
    QHBoxLayout *rateLayout = new QHBoxLayout();
    ElaText *rateLabel = new ElaText(label + ":", this);
    _mainRateSlider = new ElaSlider(Qt::Horizontal, this);
    _mainRateSlider->setRange(0, kRateRange);
    _mainRateSlider->setValue(defaultValue);
    ElaSpinBox *rateSpinBox = new ElaSpinBox(this);
    rateSpinBox->setRange(0, kRateRange);
    rateSpinBox->setValue(defaultValue);

    rateLayout->addWidget(rateLabel);
    rateLayout->addWidget(_mainRateSlider);
    rateLayout->addWidget(rateSpinBox);

    layout->addLayout(rateLayout);

    connect(_mainRateSlider, &ElaSlider::valueChanged, rateSpinBox, &ElaSpinBox::setValue);
    connect(rateSpinBox, QOverload<int>::of(&ElaSpinBox::valueChanged), _mainRateSlider, &ElaSlider::setValue);

    // 连接速率变化槽
    if (label.contains("主速率")) {
        connect(_mainRateSlider, &ElaSlider::valueChanged, this, &T_TelescopePage::onTrackingRateChanged);
    } else if (label.contains("次速率")) {
        connect(_mainRateSlider, &ElaSlider::valueChanged, this, &T_TelescopePage::onSecondaryRateChanged);
    }
}

void T_TelescopePage::addReversedToggle(QVBoxLayout *layout, const QString &label) {
    QHBoxLayout *toggleLayout = new QHBoxLayout();
    ElaText *toggleLabel = new ElaText(label, this);
    ElaToggleSwitch *toggleSwitch = new ElaToggleSwitch(this);

    toggleLayout->addWidget(toggleLabel);
    toggleLayout->addWidget(toggleSwitch);
    toggleLayout->addStretch();

    layout->addLayout(toggleLayout);

    // 连接反向切换槽
    if (label.contains("主轴")) {
        _mainAxisReversedSwitch = toggleSwitch;
        connect(_mainAxisReversedSwitch, &ElaToggleSwitch::toggled, this, &T_TelescopePage::onMainAxisReversed);
    } else if (label.contains("次轴")) {
        _secondaryAxisReversedSwitch = toggleSwitch;
        connect(_secondaryAxisReversedSwitch, &ElaToggleSwitch::toggled, this, &T_TelescopePage::onSecondaryAxisReversed);
    }
}

void T_TelescopePage::onPowerButtonClicked() {

}

void T_TelescopePage::onRefreshButtonClicked() {
    // 处理刷新按钮点击事件
    // 刷新望远镜信息
}

void T_TelescopePage::onSlewButtonClicked() {
    // 处理指向按钮点击事件
    // 执行指向操作
}

void T_TelescopePage::onDirectionButtonClicked(const QString &direction) {
    // 处理方向按钮点击事件
    if (direction == "N") {
        // 向北移动
    } else if (direction == "S") {
        // 向南移动
    } else if (direction == "E") {
        // 向东移动
    } else if (direction == "W") {
        // 向西移动
    } else {
        // 停止移动
    }
}

void T_TelescopePage::onParkButtonClicked() {
    // 处理复位按钮点击事件
    // 执行复位操作
}

void T_TelescopePage::onSetParkPointClicked() {
    // 处理设为复位点按钮点击事件
    // 设置当前坐标为复位点
}

void T_TelescopePage::onHomeButtonClicked() {
    // 处理返回原点按钮点击事件
    // 执行返回原点操作
}

void T_TelescopePage::onDewHeaterToggled(bool checked) {
    if (checked) {
        // 开启露水加热器
    } else {
        // 关闭露水加热器
    }
}

void T_TelescopePage::updateTrackingStatus() {
    // 更新跟踪状态显示
    QString status;
    switch (_trackingRateCombo->currentIndex()) {
        case 0:
            status = "无跟踪";
            break;
        case 1:
            status = "赤道跟踪";
            break;
        case 2:
            status = "银河跟踪";
            break;
        default:
            status = "未知";
            break;
    }
    _trackingStatusLabel->setText("当前跟踪状态: " + status);
}

void T_TelescopePage::onTrackingRateChanged(int value) {
    // 处理主跟踪速率变化
    Q_UNUSED(value);
    // 更新跟踪速率
}

void T_TelescopePage::onSecondaryRateChanged(int value) {
    // 处理次跟踪速率变化
    Q_UNUSED(value);
    // 更新次跟踪速率
}

void T_TelescopePage::onMainAxisReversed(bool checked) {
    if (checked) {
        // 主轴反向
    } else {
        // 主轴正向
    }
}

void T_TelescopePage::onSecondaryAxisReversed(bool checked) {
    if (checked) {
        // 次轴反向
    } else {
        // 次轴正向
    }
}