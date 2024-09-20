#include "T_Focuser.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "C_InfoCard.h"
#include "ElaComboBox.h"
#include "ElaIconButton.h"
#include "ElaTabWidget.h"
#include "ElaText.h"
#include "ElaToggleSwitch.h"

T_FocuserPage::T_FocuserPage(QWidget *parent) : T_BasePage(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Top section
    QHBoxLayout *topLayout = createTopLayout();
    mainLayout->addLayout(topLayout);

    // Create tab widget
    ElaTabWidget *tabWidget = new ElaTabWidget(this);

    // Create and add "Information" tab
    QWidget *infoTab = createInfoTab();
    tabWidget->addTab(infoTab, "调焦器信息");

    // Create and add "Control" tab
    QWidget *controlTab = createControlTab();
    tabWidget->addTab(controlTab, "调焦器控制");

    // Create and add "Settings" tab
    QWidget *settingsTab = createSettingsTab();
    tabWidget->addTab(settingsTab, "设置");

    mainLayout->addWidget(tabWidget);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("调焦器面板");
    QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

T_FocuserPage::~T_FocuserPage() {}

QHBoxLayout *T_FocuserPage::createTopLayout() {
    QHBoxLayout *topLayout = new QHBoxLayout();

    ElaComboBox *focuserCombo = new ElaComboBox(this);
    focuserCombo->addItem("Simulator");

    auto createIconButton = [this](ElaIconType::IconName icon) {
        ElaIconButton *button = new ElaIconButton(icon, this);
        button->setFixedSize(40, 40);
        return button;
    };

    ElaIconButton *settingsButton = createIconButton(ElaIconType::Gears);
    ElaIconButton *refreshButton = createIconButton(ElaIconType::ArrowsRotate);
    ElaIconButton *powerButton = createIconButton(ElaIconType::PowerOff);

    topLayout->addWidget(focuserCombo, 1);
    topLayout->addWidget(settingsButton);
    topLayout->addWidget(refreshButton);
    topLayout->addWidget(powerButton);

    return topLayout;
}

QWidget *T_FocuserPage::createInfoTab() {
    QWidget *infoWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(infoWidget);

    // Basic Info Group
    QGridLayout *infoLayout = new QGridLayout();
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

QWidget *T_FocuserPage::createControlTab() {
    QWidget *controlWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(controlWidget);

    // Temperature Compensation
    QHBoxLayout *tempCompLayout = new QHBoxLayout();
    ElaText *tempCompLabel = new ElaText("温度补偿", this);
    ElaToggleSwitch *tempCompSwitch = new ElaToggleSwitch(this);
    tempCompLayout->addWidget(tempCompLabel);
    tempCompLayout->addWidget(tempCompSwitch);
    tempCompLayout->addStretch();
    layout->addLayout(tempCompLayout);

    // Target Position
    QHBoxLayout *targetPosLayout = new QHBoxLayout();
    ElaText *targetPosLabel = new ElaText("目标位置", this);
    QLineEdit *targetPosEdit = new QLineEdit("12500", this);
    QPushButton *moveButton = new QPushButton("移动", this);
    targetPosLayout->addWidget(targetPosLabel);
    targetPosLayout->addWidget(targetPosEdit);
    targetPosLayout->addWidget(moveButton);
    layout->addLayout(targetPosLayout);

    // Movement Buttons
    QHBoxLayout *moveButtonsLayout = new QHBoxLayout();
    QPushButton *moveLeftMost = new QPushButton("<<", this);
    QPushButton *moveLeft = new QPushButton("<", this);
    QPushButton *moveRight = new QPushButton(">", this);
    QPushButton *moveRightMost = new QPushButton(">>", this);
    moveButtonsLayout->addWidget(moveLeftMost);
    moveButtonsLayout->addWidget(moveLeft);
    moveButtonsLayout->addWidget(moveRight);
    moveButtonsLayout->addWidget(moveRightMost);
    layout->addLayout(moveButtonsLayout);

    layout->addStretch();

    return controlWidget;
}

QWidget *T_FocuserPage::createSettingsTab() {
    QWidget *settingsWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(settingsWidget);

    ElaText *settingsText = new ElaText("暂无设置选项", this);
    layout->addWidget(settingsText);
    layout->addStretch();

    return settingsWidget;
}