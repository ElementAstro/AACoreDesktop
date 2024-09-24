#include "T_FilterWheel.h"

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

namespace {
constexpr int kSpacing20 = 20;
constexpr int kMargin20 = 20;
constexpr int kFixedSize40 = 40;
}  // namespace

T_FilterWheelPage::T_FilterWheelPage(QWidget *parent) : T_BasePage(parent) {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(kSpacing20);
    mainLayout->setContentsMargins(kMargin20, kMargin20, kMargin20, kMargin20);

    // Top section
    auto *topLayout = createTopLayout();
    mainLayout->addLayout(topLayout);

    // Create tab widget
    auto *tabWidget = new ElaTabWidget(this);

    // Create and add "Information" tab
    auto *infoTab = createInfoTab();
    tabWidget->addTab(infoTab, "滤镜轮信息");

    // Create and add "Filters" tab
    auto *filtersTab = createFiltersTab();
    tabWidget->addTab(filtersTab, "滤镜");

    // Create and add "Settings" tab
    auto *settingsTab = createSettingsTab();
    tabWidget->addTab(settingsTab, "设置");

    mainLayout->addWidget(tabWidget);

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("滤镜轮面板");
    auto *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

auto T_FilterWheelPage::createTopLayout() -> QHBoxLayout * {
    auto *topLayout = new QHBoxLayout();

    auto *filterWheelCombo = new ElaComboBox(this);
    filterWheelCombo->addItem("Manual filter wheel");

    auto createIconButton = [this](ElaIconType::IconName icon) {
        auto *button = new ElaIconButton(icon, this);
        button->setFixedSize(kFixedSize40, kFixedSize40);
        return button;
    };

    auto *settingsButton = createIconButton(ElaIconType::Gears);
    auto *refreshButton = createIconButton(ElaIconType::ArrowsRotate);
    auto *powerButton = createIconButton(ElaIconType::PowerOff);

    topLayout->addWidget(filterWheelCombo, 1);
    topLayout->addWidget(settingsButton);
    topLayout->addWidget(refreshButton);
    topLayout->addWidget(powerButton);

    return topLayout;
}

auto T_FilterWheelPage::createInfoTab() -> QWidget * {
    auto *infoWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(infoWidget);

    // Basic Info Group
    auto *infoLayout = new QGridLayout();
    infoLayout->addWidget(new InfoCard("名称", "Manual filter wheel", this), 0,
                          0);
    infoLayout->addWidget(
        new InfoCard(
            "描述",
            "Mirrors the filters that are set up inside the options. "
            "When a filter change is requested a notification will pop "
            "up to manually change the filter.",
            this),
        1, 0, 1, 2);
    infoLayout->addWidget(new InfoCard("驱动信息", "n.A.", this), 2, 0);
    infoLayout->addWidget(new InfoCard("驱动版本", "1.0", this), 2, 1);

    layout->addLayout(infoLayout);
    layout->addStretch();

    return infoWidget;
}

auto T_FilterWheelPage::createFiltersTab() -> QWidget * {
    auto *filtersWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(filtersWidget);

    // Current Filter
    auto *currentFilterLayout = new QHBoxLayout();
    auto *filterCombo = new ElaComboBox(this);
    filterCombo->addItem("Filter1");
    auto *changeButton = new ElaPushButton("更改", this);
    currentFilterLayout->addWidget(filterCombo);
    currentFilterLayout->addWidget(changeButton);
    layout->addLayout(currentFilterLayout);

    // Filter List
    auto *filterListLayout = new QVBoxLayout();
    auto *filterNameLabel = new ElaText("滤镜名称", this);
    auto *filterNameEdit = new ElaLineEdit(this);
    filterNameEdit->setText("Filter1");
    filterListLayout->addWidget(filterNameLabel);
    filterListLayout->addWidget(filterNameEdit);
    layout->addLayout(filterListLayout);

    layout->addStretch();

    return filtersWidget;
}

auto T_FilterWheelPage::createSettingsTab() -> QWidget * {
    auto *settingsWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(settingsWidget);

    auto *settingsText = new ElaText("暂无设置选项", this);
    layout->addWidget(settingsText);
    layout->addStretch();

    return settingsWidget;
}