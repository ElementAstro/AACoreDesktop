#include "T_Telescope.h"

#include <QDateTime>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "Components/C_InfoCard.h"
#include "ElaComboBox.h"
#include "ElaIconButton.h"
#include "ElaSlider.h"
#include "ElaSpinBox.h"
#include "ElaTabWidget.h"
#include "ElaText.h"
#include "ElaToggleSwitch.h"

T_TelescopePage::T_TelescopePage(QWidget *parent) : T_BasePage(parent) {
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
    tabWidget->addTab(infoTab, "望远镜信息");

    // Create and add "Control" tab
    QWidget *controlTab = createControlTab();
    tabWidget->addTab(controlTab, "望远镜控制");

    // Create and add "Settings" tab
    QWidget *settingsTab = createSettingsTab();
    tabWidget->addTab(settingsTab, "设置");

    mainLayout->addWidget(tabWidget);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("望远镜面板");
    QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

T_TelescopePage ::~T_TelescopePage() {}

QHBoxLayout *T_TelescopePage::createTopLayout() {
    QHBoxLayout *topLayout = new QHBoxLayout();

    ElaComboBox *telescopeCombo = new ElaComboBox(this);
    telescopeCombo->addItem("Telescope Simulator for .NET");

    auto createIconButton = [this](ElaIconType::IconName icon) {
        ElaIconButton *button = new ElaIconButton(icon, this);
        button->setFixedSize(40, 40);
        return button;
    };

    ElaIconButton *settingsButton = createIconButton(ElaIconType::Gears);
    ElaIconButton *refreshButton = createIconButton(ElaIconType::ArrowsRotate);
    ElaIconButton *powerButton = createIconButton(ElaIconType::PowerOff);

    topLayout->addWidget(telescopeCombo, 1);
    topLayout->addWidget(settingsButton);
    topLayout->addWidget(refreshButton);
    topLayout->addWidget(powerButton);

    return topLayout;
}

QWidget *T_TelescopePage::createInfoTab() {
    QWidget *infoWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(infoWidget);

    // Basic Info Group
    QGroupBox *basicInfoGroup = createInfoGroup("基本信息");
    QGridLayout *basicInfoLayout = new QGridLayout(basicInfoGroup);
    basicInfoLayout->addWidget(
        new InfoCard("望远镜名称", "Telescope Simulator for .NET", this), 0, 0);
    basicInfoLayout->addWidget(
        new InfoCard("描述", "Software Telescope Simulator for ASCOM", this), 0,
        1);
    basicInfoLayout->addWidget(
        new InfoCard("驱动信息",
                     "ASCOM.Simulator.Telescope, Version=6.6.0.0, "
                     "Culture=neutral, PublicKeyToken=565de79389",
                     this),
        1, 0, 1, 2);
    basicInfoLayout->addWidget(new InfoCard("驱动版本", "6.6", this), 2, 0);

    // Site Info Group
    QGroupBox *siteInfoGroup = createInfoGroup("站点信息");
    QGridLayout *siteInfoLayout = new QGridLayout(siteInfoGroup);
    siteInfoLayout->addWidget(new InfoCard("纬度", "51° 04' 43\"", this), 0, 0);
    siteInfoLayout->addWidget(new InfoCard("经度", "-00° 17' 40\"", this), 0,
                              1);
    siteInfoLayout->addWidget(new InfoCard("海拔", "244.0 m", this), 1, 0);
    siteInfoLayout->addWidget(new InfoCard("纪元", "JNOW", this), 1, 1);

    layout->addWidget(basicInfoGroup);
    layout->addWidget(siteInfoGroup);
    layout->addStretch();

    return infoWidget;
}

QWidget *T_TelescopePage::createControlTab() {
    QWidget *controlWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(controlWidget);

    // Coordinates Group
    QGroupBox *coordGroup = createInfoGroup("手动坐标 (J2000)");
    QGridLayout *coordLayout = new QGridLayout(coordGroup);
    addCoordinateRow(coordLayout, "目标赤经", "0h", "0m", "0.0s");
    addCoordinateRow(coordLayout, "目标赤纬", "0d", "0m", "0.0s");
    QPushButton *slewButton = new QPushButton("指向", this);
    coordLayout->addWidget(slewButton, 2, 3);

    // Manual Control Group
    QGroupBox *manualControlGroup = createInfoGroup("手动控制");
    QVBoxLayout *manualControlLayout = new QVBoxLayout(manualControlGroup);

    // Tracking rate
    QHBoxLayout *trackingLayout = new QHBoxLayout();
    ElaComboBox *trackingCombo = new ElaComboBox(this);
    trackingCombo->addItem("设置跟踪速率");
    trackingLayout->addWidget(trackingCombo);
    manualControlLayout->addLayout(trackingLayout);

    // Direction buttons
    QGridLayout *directionLayout = new QGridLayout();
    QPushButton *northButton = new QPushButton("N", this);
    QPushButton *westButton = new QPushButton("W", this);
    QPushButton *stopButton = new QPushButton("停止", this);
    QPushButton *eastButton = new QPushButton("E", this);
    QPushButton *southButton = new QPushButton("S", this);

    directionLayout->addWidget(northButton, 0, 1);
    directionLayout->addWidget(westButton, 1, 0);
    directionLayout->addWidget(stopButton, 1, 1);
    directionLayout->addWidget(eastButton, 1, 2);
    directionLayout->addWidget(southButton, 2, 1);

    manualControlLayout->addLayout(directionLayout);

    // Rate controls
    addRateControl(manualControlLayout, "主速率", 20.00);
    addRateControl(manualControlLayout, "次速率", 20.00);

    // Reversed toggles
    addReversedToggle(manualControlLayout, "主轴反向");
    addReversedToggle(manualControlLayout, "次轴反向");

    // Park controls
    QHBoxLayout *parkLayout = new QHBoxLayout();
    QPushButton *parkButton = new QPushButton("复位", this);
    QPushButton *setAsParkButton = new QPushButton("设为复位点", this);
    QPushButton *homeButton = new QPushButton("返回原点", this);
    parkLayout->addWidget(parkButton);
    parkLayout->addWidget(setAsParkButton);
    parkLayout->addWidget(homeButton);
    manualControlLayout->addLayout(parkLayout);

    layout->addWidget(coordGroup);
    layout->addWidget(manualControlGroup);
    layout->addStretch();

    return controlWidget;
}

QWidget *T_TelescopePage::createSettingsTab() {
    QWidget *settingsWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(settingsWidget);

    ElaText *settingsText = new ElaText("暂无设置选项", this);
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

void T_TelescopePage::addCoordinateRow(QGridLayout *layout,
                                       const QString &label,
                                       const QString &value1,
                                       const QString &value2,
                                       const QString &value3) {
    int row = layout->rowCount();
    ElaText *labelWidget = new ElaText(label, this);
    QLineEdit *edit1 = new QLineEdit(value1, this);
    QLineEdit *edit2 = new QLineEdit(value2, this);
    QLineEdit *edit3 = new QLineEdit(value3, this);
    layout->addWidget(labelWidget, row, 0);
    layout->addWidget(edit1, row, 1);
    layout->addWidget(edit2, row, 2);
    layout->addWidget(edit3, row, 3);
}

void T_TelescopePage::addRateControl(QVBoxLayout *layout, const QString &label,
                                     double defaultValue) {
    QHBoxLayout *rateLayout = new QHBoxLayout();
    ElaText *rateLabel = new ElaText(label, this);
    ElaSlider *rateSlider = new ElaSlider(Qt::Horizontal, this);
    rateSlider->setRange(0, 100);
    rateSlider->setValue(defaultValue);
    ElaSpinBox *rateSpinBox = new ElaSpinBox(this);
    rateSpinBox->setRange(0, 100);
    rateSpinBox->setValue(defaultValue);

    rateLayout->addWidget(rateLabel);
    rateLayout->addWidget(rateSlider);
    rateLayout->addWidget(rateSpinBox);

    layout->addLayout(rateLayout);

    connect(rateSlider, &ElaSlider::valueChanged, rateSpinBox,
            &ElaSpinBox::setValue);
    connect(rateSpinBox, QOverload<int>::of(&ElaSpinBox::valueChanged),
            rateSlider, &ElaSlider::setValue);
}

void T_TelescopePage::addReversedToggle(QVBoxLayout *layout,
                                        const QString &label) {
    QHBoxLayout *toggleLayout = new QHBoxLayout();
    ElaText *toggleLabel = new ElaText(label, this);
    ElaToggleSwitch *toggleSwitch = new ElaToggleSwitch(this);

    toggleLayout->addWidget(toggleLabel);
    toggleLayout->addWidget(toggleSwitch);
    toggleLayout->addStretch();

    layout->addLayout(toggleLayout);
}