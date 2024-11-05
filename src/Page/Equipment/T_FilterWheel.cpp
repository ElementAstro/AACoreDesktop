#include "T_FilterWheel.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <QtCharts/QChartView>
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
constexpr int kTimerInterval = 1000;  // 1秒
}  // namespace

T_FilterWheelPage::T_FilterWheelPage(QWidget *parent) : T_BasePage(parent) {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(kSpacing20);
    mainLayout->setContentsMargins(kMargin20, kMargin20, kMargin20, kMargin20);

    // 顶部部分
    auto *topLayout = createTopLayout();
    mainLayout->addLayout(topLayout);

    // 创建标签页控件
    auto *tabWidget = new ElaTabWidget(this);

    // 创建并添加"滤镜轮信息"标签页
    auto *infoTab = createInfoTab();
    tabWidget->addTab(infoTab, "滤镜轮信息");

    // 创建并添加"滤镜管理"标签页
    auto *filtersTab = createFiltersTab();
    tabWidget->addTab(filtersTab, "滤镜管理");

    // 创建并添加"设置"标签页
    auto *settingsTab = createSettingsTab();
    tabWidget->addTab(settingsTab, "设置");

    mainLayout->addWidget(tabWidget);

    // 状态更新定时器
    _statusTimer = new QTimer(this);
    connect(_statusTimer, &QTimer::timeout, this,
            &T_FilterWheelPage::updateFilterStatus);
    _statusTimer->start(kTimerInterval);  // 每秒更新一次

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("滤镜轮");
    auto *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

T_FilterWheelPage::~T_FilterWheelPage() {}

QHBoxLayout *T_FilterWheelPage::createTopLayout() {
    auto *topLayout = new QHBoxLayout();

    // 滤镜轮选择下拉框
    _filterWheelCombo = new ElaComboBox(this);
    _filterWheelCombo->addItem("Manual Filter Wheel");
    connect(_filterWheelCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int index) {
                // 处理滤镜轮选择变化
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
            &T_FilterWheelPage::onPowerButtonClicked);
    connect(_refreshButton, &ElaIconButton::clicked, this,
            &T_FilterWheelPage::onRefreshButtonClicked);
    connect(settingsButton, &ElaIconButton::clicked, this, [this]() {
        // 打开设置对话框或执行相关操作
    });

    topLayout->addWidget(_filterWheelCombo, 1);
    topLayout->addWidget(settingsButton);
    topLayout->addWidget(_refreshButton);
    topLayout->addWidget(_powerButton);

    return topLayout;
}

QWidget *T_FilterWheelPage::createInfoTab() {
    auto *infoWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(infoWidget);
    layout->setSpacing(kSpacing10);
    layout->setContentsMargins(kMargin10, kMargin10, kMargin10, kMargin10);

    // 基本信息组
    auto *basicInfoGroup = createInfoGroup("基本信息");
    auto *basicInfoLayout = new QGridLayout(basicInfoGroup);
    basicInfoLayout->addWidget(
        new InfoCard("名称", "Manual Filter Wheel", this), 0, 0);
    basicInfoLayout->addWidget(
        new InfoCard("描述",
                       "手动滤镜轮用于手动更换滤镜，当请求更换滤镜时，会弹出通"
                       "知手动更换滤镜。",
                       this),
        1, 0, 1, 2);
    basicInfoLayout->addWidget(new InfoCard("驱动信息", "n.A.", this), 2, 0);
    basicInfoLayout->addWidget(new InfoCard("驱动版本", "1.0", this), 2, 1);

    // 系统信息组
    auto *systemInfoGroup = createInfoGroup("系统信息");
    auto *systemInfoLayout = new QGridLayout(systemInfoGroup);
    systemInfoLayout->addWidget(new InfoCard("软件版本", "2.1.0", this), 0,
                                0);
    systemInfoLayout->addWidget(
        new InfoCard("运行时间", "5小时 20分钟", this), 0, 1);
    systemInfoLayout->addWidget(new InfoCard("状态", "正常", this), 1, 0);
    systemInfoLayout->addWidget(new InfoCard("当前滤镜", "Filter1", this), 1,
                                1);

    layout->addWidget(basicInfoGroup);
    layout->addWidget(systemInfoGroup);
    layout->addStretch();

    return infoWidget;
}

QWidget *T_FilterWheelPage::createFiltersTab() {
    auto *filtersWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(filtersWidget);
    layout->setSpacing(kSpacing20);
    layout->setContentsMargins(kMargin10, kMargin10, kMargin10, kMargin10);

    // 当前滤镜显示
    auto *currentFilterGroup = createInfoGroup("当前滤镜");
    auto *currentFilterLayout = new QHBoxLayout(currentFilterGroup);
    auto *currentFilterCombo = new ElaComboBox(this);
    currentFilterCombo->addItem("Filter1");
    currentFilterCombo->addItem("Filter2");
    currentFilterCombo->addItem("Filter3");
    connect(currentFilterCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &T_FilterWheelPage::onChangeFilterClicked);
    _changeFilterButton = new ElaPushButton("更改", this);
    connect(_changeFilterButton, &ElaPushButton::clicked, this,
            &T_FilterWheelPage::onChangeFilterClicked);
    currentFilterLayout->addWidget(currentFilterCombo);
    currentFilterLayout->addWidget(_changeFilterButton);
    currentFilterLayout->addStretch();

    currentFilterGroup->setLayout(currentFilterLayout);
    layout->addWidget(currentFilterGroup);

    // 添加新滤镜
    auto *addFilterGroup = createInfoGroup("添加滤镜");
    auto *addFilterLayout = new QHBoxLayout(addFilterGroup);
    _newFilterEdit = new ElaLineEdit(this);
    _newFilterEdit->setPlaceholderText("输入新滤镜名称");
    _addFilterButton = new ElaPushButton("添加", this);
    connect(_addFilterButton, &ElaPushButton::clicked, this,
            &T_FilterWheelPage::onAddFilterClicked);
    addFilterLayout->addWidget(_newFilterEdit);
    addFilterLayout->addWidget(_addFilterButton);
    addFilterLayout->addStretch();

    addFilterGroup->setLayout(addFilterLayout);
    layout->addWidget(addFilterGroup);

    // 移除滤镜
    auto *removeFilterGroup = createInfoGroup("移除滤镜");
    auto *removeFilterLayout = new QHBoxLayout(removeFilterGroup);
    auto *removeFilterCombo = new ElaComboBox(this);
    removeFilterCombo->addItem("Filter1");
    removeFilterCombo->addItem("Filter2");
    removeFilterCombo->addItem("Filter3");
    _removeFilterButton = new ElaPushButton("移除", this);
    connect(_removeFilterButton, &ElaPushButton::clicked, this,
            &T_FilterWheelPage::onRemoveFilterClicked);
    removeFilterLayout->addWidget(removeFilterCombo);
    removeFilterLayout->addWidget(_removeFilterButton);
    removeFilterLayout->addStretch();

    removeFilterGroup->setLayout(removeFilterLayout);
    layout->addWidget(removeFilterGroup);

    layout->addStretch();

    return filtersWidget;
}

QWidget *T_FilterWheelPage::createSettingsTab() {
    auto *settingsWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(settingsWidget);
    layout->setSpacing(kSpacing10);
    layout->setContentsMargins(kMargin10, kMargin10, kMargin10, kMargin10);

    // 示例设置项
    auto addSettingRow = [this, layout](
                             const QString &label, const QString &value,
                             const QString &unit, bool hasToggle = false) {
        auto *rowLayout = new QHBoxLayout();
        auto *labelWidget = new ElaText(label + ":", this);
        labelWidget->setTextPixelSize(15);
        rowLayout->addWidget(labelWidget);
        auto *settingEdit = new ElaLineEdit(this);
        settingEdit->setText(value);
        rowLayout->addWidget(settingEdit);
        rowLayout->addWidget(new ElaText(unit, this));
        if (hasToggle) {
            auto *toggle = new ElaToggleSwitch(this);
            rowLayout->addWidget(toggle);
        }
        rowLayout->addStretch();
        layout->addLayout(rowLayout);
    };

    addSettingRow("最大滤镜数", "5", "个");
    addSettingRow("自动校准间隔", "10", "分钟");
    addSettingRow("启用日志记录", "", "", true);

    layout->addStretch();
    return settingsWidget;
}

QGroupBox *T_FilterWheelPage::createInfoGroup(const QString &title) {
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

void T_FilterWheelPage::updateFilterStatus() {
    // 模拟更新滤镜轮状态
    // 在实际应用中，应从滤镜轮接口获取实时状态
    QString status =
        QRandomGenerator::global()->bounded(0, 100) > 50 ? "正常" : "错误";
    QString currentFilter =
        "Filter" + QString::number(QRandomGenerator::global()->bounded(1, 4));

    // 更新信息卡
    // 假设有方法获取 InfoCard 指针，可动态更新
    // 示例中仅更新状态标签
    _statusLabel->setText(status);
}

void T_FilterWheelPage::onPowerButtonClicked() {
}

void T_FilterWheelPage::onRefreshButtonClicked() {
    // 处理刷新按钮点击事件
    // 刷新滤镜轮信息
    updateFilterStatus();
}

void T_FilterWheelPage::onChangeFilterClicked() {
    // 处理更改滤镜按钮点击事件
    // 更改当前滤镜
}

void T_FilterWheelPage::onAddFilterClicked() {
    QString newFilter = _newFilterEdit->text();
    if (!newFilter.isEmpty()) {
        // 添加新滤镜逻辑
        // 例如，添加到滤镜列表中
        // 这里仅示例添加到下拉框
        // 假设有下拉框保存所有滤镜
    }
}

void T_FilterWheelPage::onRemoveFilterClicked() {
    // 处理移除滤镜按钮点击事件
    // 移除选中的滤镜
}
