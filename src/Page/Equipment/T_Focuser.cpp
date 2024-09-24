#include "T_Focuser.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

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
constexpr int kFixedSize40 = 40;
}  // namespace

T_FocuserPage::T_FocuserPage(QWidget *parent) : T_BasePage(parent) {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(kSpacing20);
    mainLayout->setContentsMargins(kSpacing20, kSpacing20, kSpacing20,
                                   kSpacing20);

    // Top section
    auto *topLayout = createTopLayout();
    mainLayout->addLayout(topLayout);

    // Create tab widget
    auto *tabWidget = new ElaTabWidget(this);

    // Create and add "Information" tab
    auto *infoTab = createInfoTab();
    tabWidget->addTab(infoTab, "调焦器信息");

    // Create and add "Control" tab
    auto *controlTab = createControlTab();
    tabWidget->addTab(controlTab, "调焦器控制");

    // Create and add "Settings" tab
    auto *settingsTab = createSettingsTab();
    tabWidget->addTab(settingsTab, "设置");

    mainLayout->addWidget(tabWidget);

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("调焦器面板");
    auto *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

T_FocuserPage::~T_FocuserPage() = default;

auto T_FocuserPage::createTopLayout() -> QHBoxLayout * {
    auto *topLayout = new QHBoxLayout();

    auto *focuserCombo = new ElaComboBox(this);
    focuserCombo->addItem("Simulator");

    auto createIconButton = [this](ElaIconType::IconName icon) {
        auto *button = new ElaIconButton(icon, this);
        button->setFixedSize(kFixedSize40, kFixedSize40);
        return button;
    };

    auto *settingsButton = createIconButton(ElaIconType::Gears);
    auto *refreshButton = createIconButton(ElaIconType::ArrowsRotate);
    auto *powerButton = createIconButton(ElaIconType::PowerOff);

    topLayout->addWidget(focuserCombo, 1);
    topLayout->addWidget(settingsButton);
    topLayout->addWidget(refreshButton);
    topLayout->addWidget(powerButton);

    return topLayout;
}

auto T_FocuserPage::createInfoTab() -> QWidget * {
    auto *infoWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(infoWidget);

    // Basic Info Group
    auto *infoLayout = new QGridLayout();
    infoLayout->addWidget(new InfoCard("名称", "Simulator", this), 0, 0);
    infoLayout->addWidget(new InfoCard("描述", "", this), 0, 1);
    infoLayout->addWidget(new InfoCard("驱动信息", "", this), 1, 0);
    infoLayout->addWidget(new InfoCard("驱动版本", "0.0", this), 1, 1);
    infoLayout->addWidget(new InfoCard("正在移动", "否", this), 2, 0);
    infoLayout->addWidget(new InfoCard("正在定位", "否", this), 2, 1);
    infoLayout->addWidget(new InfoCard("最大增量", "50000", this), 3, 0);
    infoLayout->addWidget(new InfoCard("最大步进", "50000", this), 3, 1);
    infoLayout->addWidget(new InfoCard("位置", "12500", this), 4, 0);
    infoLayout->addWidget(new InfoCard("温度", "4.52 °C", this), 4, 1);

    layout->addLayout(infoLayout);
    layout->addStretch();

    return infoWidget;
}

auto T_FocuserPage::createControlTab() -> QWidget * {
    auto *controlWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(controlWidget);

    // Temperature Compensation
    auto *tempCompLayout = new QHBoxLayout();
    auto *tempCompLabel = new ElaText("温度补偿", this);
    auto *tempCompSwitch = new ElaToggleSwitch(this);
    tempCompLayout->addWidget(tempCompLabel);
    tempCompLayout->addWidget(tempCompSwitch);
    tempCompLayout->addStretch();
    layout->addLayout(tempCompLayout);

    // Target Position
    auto *targetPosLayout = new QHBoxLayout();
    auto *targetPosLabel = new ElaText("目标位置", this);
    auto *targetPosEdit = new ElaLineEdit(this);
    targetPosEdit->setText("12500");
    auto *moveButton = new ElaPushButton("移动", this);
    targetPosLayout->addWidget(targetPosLabel);
    targetPosLayout->addWidget(targetPosEdit);
    targetPosLayout->addWidget(moveButton);
    layout->addLayout(targetPosLayout);

    // Movement Buttons
    auto *moveButtonsLayout = new QHBoxLayout();
    auto *moveLeftMost = new ElaPushButton("<<", this);
    auto *moveLeft = new ElaPushButton("<", this);
    auto *moveRight = new ElaPushButton(">", this);
    auto *moveRightMost = new ElaPushButton(">>", this);
    moveButtonsLayout->addWidget(moveLeftMost);
    moveButtonsLayout->addWidget(moveLeft);
    moveButtonsLayout->addWidget(moveRight);
    moveButtonsLayout->addWidget(moveRightMost);
    layout->addLayout(moveButtonsLayout);

    layout->addStretch();

    return controlWidget;
}

auto T_FocuserPage::createSettingsTab() -> QWidget * {
    auto *settingsWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(settingsWidget);

    auto *settingsText = new ElaText("暂无设置选项", this);
    layout->addWidget(settingsText);
    layout->addStretch();

    return settingsWidget;
}