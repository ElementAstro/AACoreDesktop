#include "T_FilterWheel.h"

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


T_FilterWheelPage::T_FilterWheelPage(QWidget *parent) : T_BasePage(parent) {
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
  tabWidget->addTab(infoTab, "滤镜轮信息");

  // Create and add "Filters" tab
  QWidget *filtersTab = createFiltersTab();
  tabWidget->addTab(filtersTab, "滤镜");

  // Create and add "Settings" tab
  QWidget *settingsTab = createSettingsTab();
  tabWidget->addTab(settingsTab, "设置");

  mainLayout->addWidget(tabWidget);

  QWidget *centralWidget = new QWidget(this);
  centralWidget->setWindowTitle("滤镜轮面板");
  QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
  centerLayout->addLayout(mainLayout);
  centerLayout->setContentsMargins(0, 0, 0, 0);
  addCentralWidget(centralWidget, true, true, 0);
}

QHBoxLayout *T_FilterWheelPage::createTopLayout() {
  QHBoxLayout *topLayout = new QHBoxLayout();

  ElaComboBox *filterWheelCombo = new ElaComboBox(this);
  filterWheelCombo->addItem("Manual filter wheel");

  auto createIconButton = [this](ElaIconType::IconName icon) {
    ElaIconButton *button = new ElaIconButton(icon, this);
    button->setFixedSize(40, 40);
    return button;
  };

  ElaIconButton *settingsButton = createIconButton(ElaIconType::Gears);
  ElaIconButton *refreshButton = createIconButton(ElaIconType::ArrowsRotate);
  ElaIconButton *powerButton = createIconButton(ElaIconType::PowerOff);

  topLayout->addWidget(filterWheelCombo, 1);
  topLayout->addWidget(settingsButton);
  topLayout->addWidget(refreshButton);
  topLayout->addWidget(powerButton);

  return topLayout;
}

QWidget *T_FilterWheelPage::createInfoTab() {
  QWidget *infoWidget = new QWidget(this);
  QVBoxLayout *layout = new QVBoxLayout(infoWidget);

  // Basic Info Group
  QGridLayout *infoLayout = new QGridLayout();
  infoLayout->addWidget(new InfoCard("名称", "Manual filter wheel", this), 0,
                        0);
  infoLayout->addWidget(
      new InfoCard("描述",
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

QWidget *T_FilterWheelPage::createFiltersTab() {
  QWidget *filtersWidget = new QWidget(this);
  QVBoxLayout *layout = new QVBoxLayout(filtersWidget);

  // Current Filter
  QHBoxLayout *currentFilterLayout = new QHBoxLayout();
  ElaComboBox *filterCombo = new ElaComboBox(this);
  filterCombo->addItem("Filter1");
  QPushButton *changeButton = new QPushButton("更改", this);
  currentFilterLayout->addWidget(filterCombo);
  currentFilterLayout->addWidget(changeButton);
  layout->addLayout(currentFilterLayout);

  // Filter List
  QVBoxLayout *filterListLayout = new QVBoxLayout();
  ElaText *filterNameLabel = new ElaText("滤镜名称", this);
  QLineEdit *filterNameEdit = new QLineEdit("Filter1", this);
  filterListLayout->addWidget(filterNameLabel);
  filterListLayout->addWidget(filterNameEdit);
  layout->addLayout(filterListLayout);

  layout->addStretch();

  return filtersWidget;
}

QWidget *T_FilterWheelPage::createSettingsTab() {
  QWidget *settingsWidget = new QWidget(this);
  QVBoxLayout *layout = new QVBoxLayout(settingsWidget);

  ElaText *settingsText = new ElaText("暂无设置选项", this);
  layout->addWidget(settingsText);
  layout->addStretch();

  return settingsWidget;
}