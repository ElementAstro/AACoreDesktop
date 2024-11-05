#include "T_Focuser.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <QtCharts/QChartView>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>


#include "Components/C_InfoCard.h"
#include "ElaComboBox.h"
#include "ElaIconButton.h"
#include "ElaLineEdit.h"
#include "ElaPushButton.h"
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
constexpr int kTimerInterval = 1000;  // 1秒
}  // namespace

T_FocuserPage::T_FocuserPage(QWidget *parent) : T_BasePage(parent) {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(kSpacing20);
    mainLayout->setContentsMargins(kMargin20, kMargin20, kMargin20, kMargin20);

    // 顶部部分
    auto *topLayout = createTopLayout();
    mainLayout->addLayout(topLayout);

    // 创建标签页控件
    auto *tabWidget = new ElaTabWidget(this);

    // 创建并添加"信息"标签页
    auto *infoTab = createInfoTab();
    tabWidget->addTab(infoTab, "调焦器信息");

    // 创建并添加"控制"标签页
    auto *controlTab = createControlTab();
    tabWidget->addTab(controlTab, "调焦器控制");

    // 创建并添加"设置"标签页
    auto *settingsTab = createSettingsTab();
    tabWidget->addTab(settingsTab, "设置");

    mainLayout->addWidget(tabWidget);

    // 状态更新定时器
    _statusTimer = new QTimer(this);
    connect(_statusTimer, &QTimer::timeout, this,
            &T_FocuserPage::updateFocuserStatus);
    _statusTimer->start(kTimerInterval);  // 每秒更新一次

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("调焦器");
    auto *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

T_FocuserPage::~T_FocuserPage() {}

QHBoxLayout *T_FocuserPage::createTopLayout() {
    auto *topLayout = new QHBoxLayout();

    // 调焦器选择下拉框
    _focuserCombo = new ElaComboBox(this);
    _focuserCombo->addItem("Simulator");
    connect(_focuserCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
                // 处理调焦器选择变化
            });

    // 创建图标按钮的辅助函数
    auto createIconButton =
        [this](ElaIconType::IconName icon) -> ElaIconButton * {
        auto *button = new ElaIconButton(icon, this);
        button->setFixedSize(kFixedSize40, kFixedSize40);
        return button;
    };

    // 创建电源、刷新和设置按钮
    _powerButton = createIconButton(ElaIconType::PowerOff);
    _refreshButton = createIconButton(ElaIconType::ArrowsRotate);
    auto *settingsButton = createIconButton(ElaIconType::Gears);

    // 连接按钮信号到槽函数
    connect(_powerButton, &ElaIconButton::clicked, this,
            &T_FocuserPage::onPowerButtonClicked);
    connect(_refreshButton, &ElaIconButton::clicked, this,
            &T_FocuserPage::onRefreshButtonClicked);
    connect(settingsButton, &ElaIconButton::clicked, this, [this]() {
        // 打开设置对话框或执行相关操作
    });

    topLayout->addWidget(_focuserCombo, 1);
    topLayout->addWidget(settingsButton);
    topLayout->addWidget(_refreshButton);
    topLayout->addWidget(_powerButton);

    return topLayout;
}

QWidget *T_FocuserPage::createInfoTab() {
    auto *infoWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(infoWidget);
    layout->setSpacing(kSpacing10);
    layout->setContentsMargins(kMargin10, kMargin10, kMargin10, kMargin10);

    // 基本信息组
    auto *basicInfoGroup = createInfoGroup("基本信息");
    auto *basicInfoLayout = new QGridLayout(basicInfoGroup);
    addInfoCard(basicInfoLayout, "名称", "Simulator");
    addInfoCard(basicInfoLayout, "描述", "调焦器模拟器用于测试和开发");
    addInfoCard(basicInfoLayout, "驱动信息",
                "ASCOM.Simulator.Focuser, Version=1.0.0.0");
    addInfoCard(basicInfoLayout, "驱动版本", "1.0");
    addInfoCard(basicInfoLayout, "正在移动", "否");
    addInfoCard(basicInfoLayout, "正在定位", "否");
    addInfoCard(basicInfoLayout, "最大增量", "50000");
    addInfoCard(basicInfoLayout, "最大步进", "50000");
    addInfoCard(basicInfoLayout, "当前位置", "12500");
    addInfoCard(basicInfoLayout, "温度", "4.52 °C");

    // 系统信息组
    auto *systemInfoGroup = createInfoGroup("系统信息");
    auto *systemInfoLayout = new QGridLayout(systemInfoGroup);
    addInfoCard(systemInfoLayout, "软件版本", "2.1.3");
    addInfoCard(systemInfoLayout, "运行时间", "12小时 34分钟");
    addInfoCard(systemInfoLayout, "状态", "正常");

    layout->addWidget(basicInfoGroup);
    layout->addWidget(systemInfoGroup);
    layout->addStretch();

    return infoWidget;
}

QWidget *T_FocuserPage::createControlTab() {
    auto *controlWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(controlWidget);
    layout->setSpacing(kSpacing20);
    layout->setContentsMargins(kMargin10, kMargin10, kMargin10, kMargin10);

    // 温度补偿控制
    auto *tempCompLayout = new QHBoxLayout();
    auto *tempCompLabel = new ElaText("温度补偿", this);
    _tempCompSwitch = new ElaToggleSwitch(this);
    connect(_tempCompSwitch, &ElaToggleSwitch::toggled, this,
            &T_FocuserPage::onTempCompToggled);
    tempCompLayout->addWidget(tempCompLabel);
    tempCompLayout->addWidget(_tempCompSwitch);
    tempCompLayout->addStretch();
    layout->addLayout(tempCompLayout);

    // 当前位置显示
    auto *positionLayout = new QHBoxLayout();
    auto *positionLabel = new ElaText("当前位置:", this);
    _statusLabel = new QLabel("12500", this);
    _statusLabel->setStyleSheet("font-weight: bold; color: #0078d4;");
    positionLayout->addWidget(positionLabel);
    positionLayout->addWidget(_statusLabel);
    positionLayout->addStretch();
    layout->addLayout(positionLayout);

    // 目标位置输入和移动按钮
    auto *targetPosLayout = new QHBoxLayout();
    auto *targetPosLabel = new ElaText("目标位置:", this);
    _targetPositionEdit = new ElaLineEdit(this);
    _targetPositionEdit->setText("12500");
    _moveToPositionButton = new ElaPushButton("移动到位置", this);
    connect(_moveToPositionButton, &ElaPushButton::clicked, this,
            &T_FocuserPage::onMoveToPositionClicked);
    targetPosLayout->addWidget(targetPosLabel);
    targetPosLayout->addWidget(_targetPositionEdit);
    targetPosLayout->addWidget(_moveToPositionButton);
    layout->addLayout(targetPosLayout);

    // 移动控制按钮
    auto *moveButtonsLayout = new QHBoxLayout();
    _moveLeftMostButton = new ElaPushButton("<<", this);
    _moveLeftButton = new ElaPushButton("<", this);
    _moveRightButton = new ElaPushButton(">", this);
    _moveRightMostButton = new ElaPushButton(">>", this);

    connect(_moveLeftMostButton, &ElaPushButton::clicked, this,
            &T_FocuserPage::onMoveLeftMostClicked);
    connect(_moveLeftButton, &ElaPushButton::clicked, this,
            &T_FocuserPage::onMoveLeftClicked);
    connect(_moveRightButton, &ElaPushButton::clicked, this,
            &T_FocuserPage::onMoveRightClicked);
    connect(_moveRightMostButton, &ElaPushButton::clicked, this,
            &T_FocuserPage::onMoveRightMostClicked);

    moveButtonsLayout->addWidget(_moveLeftMostButton);
    moveButtonsLayout->addWidget(_moveLeftButton);
    moveButtonsLayout->addWidget(_moveRightButton);
    moveButtonsLayout->addWidget(_moveRightMostButton);
    layout->addLayout(moveButtonsLayout);

    layout->addStretch();

    return controlWidget;
}

QWidget *T_FocuserPage::createSettingsTab() {
    auto *settingsWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(settingsWidget);
    layout->setSpacing(kSpacing10);
    layout->setContentsMargins(kMargin10, kMargin10, kMargin10, kMargin10);

    // 示例设置项
    auto *settingGroup = createInfoGroup("高级设置");
    auto *settingLayout = new QGridLayout(settingGroup);
    addInfoCard(settingLayout, "步进速度", "100");
    addInfoCard(settingLayout, "加速度", "50");

    layout->addWidget(settingGroup);
    layout->addStretch();

    return settingsWidget;
}

QGroupBox *T_FocuserPage::createInfoGroup(const QString &title) {
    auto *groupBox = new QGroupBox(title, this);
    groupBox->setStyleSheet(R"(
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
    return groupBox;
}

void T_FocuserPage::addInfoCard(QGridLayout *layout, const QString &label,
                                const QString &value) {
    int row = layout->rowCount();
    auto *infoCard = new InfoCard(label, value, this);
    layout->addWidget(infoCard, row, 0, 1, 2);
}

void T_FocuserPage::addControlRow(QHBoxLayout *layout, const QString &label,
                                  QWidget *widget) {
    auto *labelWidget = new ElaText(label + ":", this);
    layout->addWidget(labelWidget);
    layout->addWidget(widget);
    layout->addStretch();
}

void T_FocuserPage::onPowerButtonClicked() {}

void T_FocuserPage::onRefreshButtonClicked() {
    // 处理刷新按钮点击事件
    // 刷新调焦器信息
}

void T_FocuserPage::onMoveButtonClicked() {
    // 处理通用移动按钮点击事件
}

void T_FocuserPage::onMoveLeftMostClicked() {
    // 移动到最左边
}

void T_FocuserPage::onMoveLeftClicked() {
    // 向左移动
}

void T_FocuserPage::onMoveRightClicked() {
    // 向右移动
}

void T_FocuserPage::onMoveRightMostClicked() {
    // 移动到最右边
}

void T_FocuserPage::onMoveToPositionClicked() {
    // 移动到指定位置
    QString posStr = _targetPositionEdit->text();
    bool ok;
    int position = posStr.toInt(&ok);
    if (ok) {
        // 执行移动到指定位置的操作
        _statusLabel->setText(QString::number(position));
    } else {
        // 输入无效，显示错误
    }
}

void T_FocuserPage::onTempCompToggled(bool checked) {
    if (checked) {
        // 开启温度补偿
    } else {
        // 关闭温度补偿
    }
}

void T_FocuserPage::updateFocuserStatus() {
    // 模拟更新调焦器状态
    // 在实际应用中，应从调焦器接口获取实时状态
    QString status =
        QRandomGenerator::global()->bounded(0, 100) > 50 ? "正常" : "错误";
    QString temp =
        QString::number(QRandomGenerator::global()->bounded(20, 30)) + " °C";
    _statusLabel->setText(status);
    _temperatureLabel->setText(temp);
}