#include "T_Camera.h"

#include <QCborValue>
#include <QDateTime>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include "Components/C_INDIPanel.h"
#include "ElaComboBox.h"
#include "ElaIcon.h"
#include "ElaIconButton.h"
#include "ElaPushButton.h"
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
}  // namespace

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

    // 温度更新定时器
    _tempUpdateTimer = new QTimer(this);
    connect(_tempUpdateTimer, &QTimer::timeout, this,
            &T_CameraPage::updateSensorTemperature);
    _tempUpdateTimer->start(2000);  // 每2秒更新一次

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("相机面板");
    auto *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

T_CameraPage::~T_CameraPage() = default;

QHBoxLayout *T_CameraPage::createTopLayout() {
    auto *topLayout = new QHBoxLayout();

    auto *cameraCombo = new ElaComboBox(this);
    cameraCombo->addItem("N.I.N.A. Simulator Camera");
    connect(cameraCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
                // 处理相机选择变化
            });

    auto createIconButton =
        [this](ElaIconType::IconName icon) -> ElaIconButton * {
        auto *button = new ElaIconButton(icon, this);
        button->setFixedSize(kFixedSize40, kFixedSize40);
        return button;
    };

    _powerButton = createIconButton(ElaIconType::PowerOff);
    _refreshButton = createIconButton(ElaIconType::ArrowsRotate);
    auto *settingsButton = createIconButton(ElaIconType::Gears);

    connect(_powerButton, &ElaIconButton::clicked, this,
            &T_CameraPage::onPowerButtonClicked);
    connect(_refreshButton, &ElaIconButton::clicked, this,
            &T_CameraPage::onRefreshButtonClicked);

    topLayout->addWidget(cameraCombo, 1);
    topLayout->addWidget(settingsButton);
    topLayout->addWidget(_refreshButton);
    topLayout->addWidget(_powerButton);

    return topLayout;
}

QWidget *T_CameraPage::createInfoTab() {
    auto *infoWidget = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(infoWidget);

    // 创建"基本信息"组
    auto *basicInfoGroup = createInfoGroup("基本信息");
    auto *basicInfoLayout = new QGridLayout(basicInfoGroup);
    addInfoRow(basicInfoLayout, "相机名称", "N.I.N.A. Simulator Camera");
    addInfoRow(basicInfoLayout, "驱动版本", "2.2.0.9001");
    addInfoRow(basicInfoLayout, "传感器类型", "Monochrome");
    addInfoRow(basicInfoLayout, "传感器名称", "Simulated Sensor");

    // 创建"传感器信息"组
    auto *sensorInfoGroup = createInfoGroup("传感器信息");
    auto *sensorInfoLayout = new QGridLayout(sensorInfoGroup);
    addInfoRow(sensorInfoLayout, "传感器尺寸", "640 x 480");
    addInfoRow(sensorInfoLayout, "最大分箱", "1 x 1");
    addInfoRow(sensorInfoLayout, "像素大小", "3.8 μm x 3.8 μm");
    addInfoRow(sensorInfoLayout, "曝光时间范围", "0 - 1.79769313486232E+308");

    // 创建"其他信息"组
    auto *otherInfoGroup = createInfoGroup("其他信息");
    auto *otherInfoLayout = new QGridLayout(otherInfoGroup);
    addInfoRow(otherInfoLayout, "增益", "0");
    addInfoRow(otherInfoLayout, "偏移量", "0");

    // 添加描述
    auto *descriptionLabel = new ElaText(
        "描述: A basic simulator to generate random noise for a specific "
        "median or load in an image that will be displayed on capture",
        infoWidget);
    descriptionLabel->setTextPixelSize(15);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setStyleSheet(
        "font-size: 12px; color: #555; margin-top: 10px;");

    mainLayout->addWidget(basicInfoGroup);
    mainLayout->addWidget(sensorInfoGroup);
    mainLayout->addWidget(otherInfoGroup);
    mainLayout->addWidget(descriptionLabel);
    mainLayout->addStretch();

    return infoWidget;
}

QGroupBox *T_CameraPage::createInfoGroup(const QString &title) {
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

QWidget *T_CameraPage::createTempControlTab() {
    auto *tempWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(tempWidget);

    // 添加露水加热器控制
    auto *dewHeaterLayout = new QHBoxLayout();
    auto *dewHeaterLabel = new ElaText("Dew heater", this);
    _toggleSwitch = new ElaToggleSwitch(this);
    connect(_toggleSwitch, &ElaToggleSwitch::toggled, this,
            &T_CameraPage::onDewHeaterToggled);
    dewHeaterLayout->addWidget(dewHeaterLabel);
    dewHeaterLayout->addWidget(_toggleSwitch);
    dewHeaterLayout->addStretch();

    // 添加传感器温度显示
    auto *sensorTempLayout = new QHBoxLayout();
    auto *sensorTempLabel = new ElaText("Sensor temperature", this);
    _sensorTempValue = new QLabel("--", this);
    _sensorTempValue->setStyleSheet("font-weight: bold; color: #0078d4;");
    sensorTempLayout->addWidget(sensorTempLabel);
    sensorTempLayout->addWidget(_sensorTempValue);
    sensorTempLayout->addStretch();

    layout->addLayout(dewHeaterLayout);
    layout->addLayout(sensorTempLayout);
    layout->addStretch();

    return tempWidget;
}

QWidget *T_CameraPage::createSettingsTab() {
    auto *containerWidget = new QWidget(this);
    auto *containerLayout = new QVBoxLayout(containerWidget);

    auto *settingsWidget = new QWidget(containerWidget);
    auto *layout = new QVBoxLayout(settingsWidget);
    layout->setContentsMargins(0, 0, 0, kMargin10);  // 添加一些边距

    // 创建设置卡片
    layout->addWidget(createSettingCard("增益设置", ":/icons/gain.png",
                                        "调整相机的信号增益", "Default gain", 0,
                                        kRange10000, 0));
    layout->addWidget(createSettingCard("偏移设置", ":/icons/offset.png",
                                        "调整相机的信号偏移", "Default offset",
                                        0, kRange1000, 0));
    layout->addWidget(createSettingCard("USB限制", ":/icons/usb.png",
                                        "设置USB传输速度限制", "USB limit", 0,
                                        kRange100, 0));

    layout->addStretch();
    containerLayout->addWidget(settingsWidget);

    return containerWidget;
}

QWidget *T_CameraPage::createSettingCard(const QString &title,
                                         const QString &iconPath,
                                         const QString &description,
                                         const QString &settingName, int min,
                                         int max, int defaultValue) {
    auto *card = new QGroupBox(this);
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    auto *cardLayout = new QVBoxLayout(card);

    // 标题和图标
    auto *titleLayout = new QHBoxLayout();
    auto *iconLabel = new QLabel(card);
    iconLabel->setPixmap(QPixmap(iconPath).scaled(kIconSize24, kIconSize24,
                                                  Qt::KeepAspectRatio,
                                                  Qt::SmoothTransformation));
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
    connect(spinBox, QOverload<int>::of(&ElaSpinBox::valueChanged), slider,
            &ElaSlider::setValue);

    // 连接设置变化槽
    if (settingName.contains("gain", Qt::CaseInsensitive)) {
        connect(slider, &ElaSlider::valueChanged, this,
                &T_CameraPage::onGainChanged);
    } else if (settingName.contains("offset", Qt::CaseInsensitive)) {
        connect(slider, &ElaSlider::valueChanged, this,
                &T_CameraPage::onOffsetChanged);
    } else if (settingName.contains("USB", Qt::CaseInsensitive)) {
        connect(slider, &ElaSlider::valueChanged, this,
                &T_CameraPage::onUSBLimitChanged);
    }

    cardLayout->addLayout(titleLayout);
    cardLayout->addWidget(descriptionText);
    cardLayout->addLayout(settingLayout);

    return card;
}

QWidget *T_CameraPage::createChartTab() {
    auto *chartWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(chartWidget);

    auto *chartView = createTemperatureChart();
    layout->addWidget(chartView);

    return chartWidget;
}

void T_CameraPage::addInfoRow(QGridLayout *layout, const QString &label,
                              const QString &value) {
    int row = layout->rowCount();
    auto *labelWidget = new ElaText(label + ":", this);
    labelWidget->setTextPixelSize(kTextPixelSize14);
    auto *valueWidget = new ElaText(value, this);
    valueWidget->setTextPixelSize(kTextPixelSize14);
    layout->addWidget(labelWidget, row, 0);
    layout->addWidget(valueWidget, row, 1);
}

void T_CameraPage::addSettingRow(QGridLayout *layout, const QString &label,
                                 int defaultValue) {
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
    connect(spinBox, QOverload<int>::of(&ElaSpinBox::valueChanged), slider,
            &ElaSlider::setValue);
}

QChartView *T_CameraPage::createTemperatureChart() {
    auto *series = new QLineSeries();
    series->setName("Sensor temperature");

    // 添加一些模拟数据
    auto startTime = QDateTime::currentDateTime().addDays(-kDays7);
    for (int i = 0; i <= kDays7; ++i) {
        qint64 time = startTime.addDays(i).toMSecsSinceEpoch();
        double temp =
            QRandomGenerator::global()->bounded(-kRandOffset, kRandMax);
        series->append(time, temp);
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

void T_CameraPage::onDewHeaterToggled(bool checked) {
    if (checked) {
        // 开启露水加热器
    } else {
        // 关闭露水加热器
    }
}

void T_CameraPage::onPowerButtonClicked() {
    // 处理电源按钮点击事件
}

void T_CameraPage::onRefreshButtonClicked() {
    // 刷新相机信息
}

void T_CameraPage::updateSensorTemperature() {
    // 获取当前传感器温度（模拟）
    double temp = QRandomGenerator::global()->bounded(-kRandOffset, kRandMax);
    _sensorTempValue->setText(QString::number(temp, 'f', 2));

    // 更新图表数据
    QChartView *chartView =
        qobject_cast<QChartView *>(findChild<QChartView *>());
    if (chartView) {
        QChart *chart = chartView->chart();
        QLineSeries *series =
            qobject_cast<QLineSeries *>(chart->series().first());
        if (series) {
            qint64 currentTime =
                QDateTime::currentDateTime().toMSecsSinceEpoch();
            series->append(currentTime, temp);
            // 移除旧数据
            while (series->count() >
                   kDays8 * 24 * 60 * (2000 / 1000)) {  // 假设每2秒一个数据点
                series->remove(0);
            }
        }
    }
}

void T_CameraPage::onGainChanged(int value) {
    // 处理增益变化
}

void T_CameraPage::onOffsetChanged(int value) {
    // 处理偏移量变化
}

void T_CameraPage::onUSBLimitChanged(int value) {
    // 处理USB限制变化
}