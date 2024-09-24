#include "T_Camera.h"

#include <QCborValue>
#include <QDateTime>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLine>
#include <QVBoxLayout>
#include <QWidget>
#include <QtCharts/QtCharts>
#include <QtGlobal>

#include "Components/C_INDIPanel.h"
#include "Components/C_InfoCard.h"
#include "Def.h"
#include "ElaComboBox.h"
#include "ElaIconButton.h"
#include "ElaSlider.h"
#include "ElaSpinBox.h"
#include "ElaTabWidget.h"
#include "ElaText.h"
#include "ElaToggleSwitch.h"

namespace {
    constexpr int kSpacing20 = 20;
    constexpr int kSpacing5 = 5;
    constexpr int kFixedSize40 = 40;
    constexpr int kMargin20 = 20;
    constexpr int kMargin10 = 10;
    constexpr int kTextPixelSize14 = 14;
    constexpr int kTextPixelSize12 = 12;
    constexpr int kTextPixelSize16 = 16;
    constexpr int kIconSize24 = 24;
    constexpr int kRange100 = 100;
    constexpr int kRange1000 = 1000;
    constexpr int kRange10000 = 10000;
    constexpr int kDays7 = 7;
    constexpr int kDays8 = 8;
    constexpr int kRandMax = 100;
    constexpr int kRandOffset = 50;
}

T_CameraPage::T_CameraPage(QWidget *parent) : T_BasePage(parent) {
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
    tabWidget->addTab(infoTab, "相机信息");

    // 创建并添加"温度控制"标签页
    auto *tempControlTab = createTempControlTab();
    tabWidget->addTab(tempControlTab, "温度控制");

    // 创建并添加"设置"标签页
    auto *settingsTab = createSettingsTab();
    tabWidget->addTab(settingsTab, "设置");

    // 创建并添加"图表"标签页
    auto *chartTab = createChartTab();
    tabWidget->addTab(chartTab, "温度图表");

    _indiPanel = new C_INDIPanel("CCD", this);
    tabWidget->addTab(_indiPanel, "INDI Panel");

    mainLayout->addWidget(tabWidget);

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("相机面板");
    auto *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(topLayout);
    centerLayout->addSpacing(kSpacing5);
    centerLayout->addLayout(mainLayout);
    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

T_CameraPage::~T_CameraPage() = default;

auto T_CameraPage::createTopLayout() -> QHBoxLayout* {
    auto *topLayout = new QHBoxLayout();

    auto *cameraCombo = new ElaComboBox(this);
    cameraCombo->addItem("N.I.N.A. Simulator Camera");

    auto createIconButton = [this](ElaIconType::IconName icon) {
        auto *button = new ElaIconButton(icon, this);
        button->setFixedSize(kFixedSize40, kFixedSize40);
        return button;
    };

    auto *settingsButton = createIconButton(ElaIconType::Gears);
    auto *refreshButton = createIconButton(ElaIconType::ArrowsRotate);
    auto *powerButton = createIconButton(ElaIconType::PowerOff);

    topLayout->addWidget(cameraCombo, 1);
    topLayout->addWidget(settingsButton);
    topLayout->addWidget(refreshButton);
    topLayout->addWidget(powerButton);

    return topLayout;
}

auto T_CameraPage::createInfoTab() -> QWidget* {
    auto *infoWidget = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(infoWidget);

    // 创建"基本信息"组
    auto *basicInfoGroup = createInfoGroup("基本信息");
    auto *basicInfoLayout = new QGridLayout(basicInfoGroup);
    basicInfoLayout->addWidget(new InfoCard("相机名称", "N.I.N.A. Simulator Camera"), 0, 0);
    basicInfoLayout->addWidget(new InfoCard("驱动版本", "2.2.0.9001"), 0, 1);
    basicInfoLayout->addWidget(new InfoCard("传感器类型", "Monochrome"), 1, 0);
    basicInfoLayout->addWidget(new InfoCard("传感器名称", "Simulated Sensor"), 1, 1);

    // 创建"传感器信息"组
    auto *sensorInfoGroup = createInfoGroup("传感器信息");
    auto *sensorInfoLayout = new QGridLayout(sensorInfoGroup);
    sensorInfoLayout->addWidget(new InfoCard("传感器尺寸", "640 x 480"), 0, 0);
    sensorInfoLayout->addWidget(new InfoCard("最大分箱", "1 x 1"), 0, 1);
    sensorInfoLayout->addWidget(new InfoCard("像素大小", "3.8 μm x 3.8 μm"), 1, 0);
    sensorInfoLayout->addWidget(new InfoCard("曝光时间范围", "0 - 1.79769313486232E+308"), 1, 1);

    // 创建"其他信息"组
    auto *otherInfoGroup = createInfoGroup("其他信息");
    auto *otherInfoLayout = new QGridLayout(otherInfoGroup);
    otherInfoLayout->addWidget(new InfoCard("增益", "0"), 0, 0);
    otherInfoLayout->addWidget(new InfoCard("偏移量", "0"), 0, 1);

    // 添加描述
    auto *descriptionLabel = new ElaText(
        "描述: A basic simulator to generate random noise for a specific "
        "median or load in an image that will be displayed on capture");
    descriptionLabel->setTextPixelSize(15);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setStyleSheet("font-size: 12px; color: #555; margin-top: 10px;");

    mainLayout->addWidget(basicInfoGroup);
    mainLayout->addWidget(sensorInfoGroup);
    mainLayout->addWidget(otherInfoGroup);
    mainLayout->addWidget(descriptionLabel);
    mainLayout->addStretch();

    return infoWidget;
}

auto T_CameraPage::createInfoGroup(const QString &title) -> QGroupBox* {
    auto *groupBox = new QGroupBox(title);
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

auto T_CameraPage::createTempControlTab() -> QWidget* {
    auto *tempWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(tempWidget);

    // 添加温度控制元素
    auto *dewHeaterLayout = new QHBoxLayout();
    auto *dewHeaterLabel = new ElaText("Dew heater", this);
    auto *dewHeaterSwitch = new ElaToggleSwitch(this);
    dewHeaterLayout->addWidget(dewHeaterLabel);
    dewHeaterLayout->addWidget(dewHeaterSwitch);
    dewHeaterLayout->addStretch();

    auto *sensorTempLayout = new QHBoxLayout();
    auto *sensorTempLabel = new ElaText("Sensor temperature", this);
    auto *sensorTempValue = new ElaText("--", this);
    sensorTempValue->setStyleSheet("font-weight: bold; color: #0078d4;");
    sensorTempLayout->addWidget(sensorTempLabel);
    sensorTempLayout->addWidget(sensorTempValue);
    sensorTempLayout->addStretch();

    layout->addLayout(dewHeaterLayout);
    layout->addLayout(sensorTempLayout);

    return tempWidget;
}

auto T_CameraPage::createSettingsTab() -> QWidget* {
    auto *containerWidget = new QWidget(this);
    auto *containerLayout = new QVBoxLayout(containerWidget);

    auto *settingsWidget = new QWidget(containerWidget);
    auto *layout = new QVBoxLayout(settingsWidget);
    layout->setContentsMargins(0, 0, 0, kMargin10);  // 添加一些边距

    // 创建设置卡片
    layout->addWidget(createSettingCard("增益设置", ":/icons/gain.png", "调整相机的信号增益", "Default gain", 0, kRange10000, 0));
    layout->addWidget(createSettingCard("偏移设置", ":/icons/offset.png", "调整相机的信号偏移", "Default offset", 0, kRange1000, 0));
    layout->addWidget(createSettingCard("USB限制", ":/icons/usb.png", "设置USB传输速度限制", "USB limit", 0, kRange100, 0));

    layout->addStretch();
    containerLayout->addWidget(settingsWidget);

    return containerWidget;
}

auto T_CameraPage::createSettingCard(const QString &title, const QString &iconPath, const QString &description, const QString &settingName, int min, int max, int defaultValue) -> QWidget* {
    auto *card = new QGroupBox(this);
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    auto *cardLayout = new QVBoxLayout(card);

    // 标题和图标
    auto *titleLayout = new QHBoxLayout();
    auto *iconLabel = new ElaText(card);
    iconLabel->setPixmap(QPixmap(iconPath).scaled(kIconSize24, kIconSize24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    auto *titleText = new ElaText(title, card);
    titleText->setTextPixelSize(kTextPixelSize16);
    titleLayout->addWidget(iconLabel);
    titleLayout->addWidget(titleText);
    titleLayout->addStretch();

    // 描述
    auto *descriptionText = new ElaText(description, card);
    descriptionText->setTextPixelSize(kTextPixelSize12);

    // 设置控件
    auto *settingLayout = new QHBoxLayout();
    auto *settingLabel = new ElaText(settingName, card);
    settingLabel->setTextPixelSize(kTextPixelSize14);
    auto *slider = new ElaSlider(Qt::Horizontal, card);
    slider->setRange(min, max);
    slider->setValue(defaultValue);
    auto *spinBox = new ElaSpinBox(card);
    spinBox->setRange(min, max);
    spinBox->setValue(defaultValue);

    settingLayout->addWidget(settingLabel);
    settingLayout->addWidget(slider);
    settingLayout->addWidget(spinBox);

    connect(slider, &ElaSlider::valueChanged, spinBox, &ElaSpinBox::setValue);
    connect(spinBox, QOverload<int>::of(&ElaSpinBox::valueChanged), slider, &ElaSlider::setValue);

    cardLayout->addLayout(titleLayout);
    cardLayout->addWidget(descriptionText);
    cardLayout->addLayout(settingLayout);

    return card;
}

auto T_CameraPage::createChartTab() -> QWidget* {
    auto *chartWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(chartWidget);

    auto *chartView = createTemperatureChart();
    layout->addWidget(chartView);

    return chartWidget;
}

void T_CameraPage::addInfoRow(QGridLayout *layout, const QString &label, const QString &value) {
    int row = layout->rowCount();
    auto *labelWidget = new ElaText(label + ":", this);
    labelWidget->setTextPixelSize(kTextPixelSize14);
    auto *valueWidget = new ElaText(value, this);
    valueWidget->setTextPixelSize(kTextPixelSize14);
    layout->addWidget(labelWidget, row, 0);
    layout->addWidget(valueWidget, row, 1);
}

void T_CameraPage::addSettingRow(QGridLayout *layout, const QString &label, int defaultValue) {
    int row = layout->rowCount();
    auto *labelWidget = new ElaText(label, this);
    labelWidget->setTextPixelSize(kTextPixelSize14);
    auto *slider = new ElaSlider(Qt::Horizontal, this);
    slider->setRange(0, kRange100);
    slider->setValue(defaultValue);
    auto *spinBox = new ElaSpinBox(this);
    spinBox->setRange(0, kRange100);
    spinBox->setValue(defaultValue);

    layout->addWidget(labelWidget, row, 0);
    layout->addWidget(slider, row, 1);
    layout->addWidget(spinBox, row, 2);

    connect(slider, &ElaSlider::valueChanged, spinBox, &ElaSpinBox::setValue);
    connect(spinBox, QOverload<int>::of(&ElaSpinBox::valueChanged), slider, &ElaSlider::setValue);
}

auto T_CameraPage::createTemperatureChart() -> QChartView* {
    auto *series = new QLineSeries();
    series->setName("Sensor temperature");

    // Add some dummy data
    auto startTime = QDateTime::currentDateTime().addDays(-kDays7);
    for (int i = 0; i < kDays8; ++i) {
        series->append(startTime.addDays(i).toMSecsSinceEpoch(), rand() % kRandMax - kRandOffset);
    }

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Temperature History");

    auto *axisX = new QDateTimeAxis;
    axisX->setTickCount(kDays8);
    axisX->setFormat("yyyy-MM-dd");
    axisX->setTitleText("Date");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto *axisY = new QValueAxis;
    axisY->setRange(-kRandOffset, kRandMax);
    axisY->setTitleText("Temperature (°C)");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    auto *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView;
}