#include "T_Camera.h"

#include <qgridlayout.h>
#include <qwidget.h>
#include <QCborValue>
#include <QDateTime>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLine>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QtCharts/QtCharts>
#include <QtGlobal>


#include "C_INDIPanel.h"
#include "C_InfoCard.h"
#include "Def.h"
#include "ElaComboBox.h"
#include "ElaIcon.h"
#include "ElaIconButton.h"
#include "ElaScrollPageArea.h"
#include "ElaSlider.h"
#include "ElaSpinBox.h"
#include "ElaTabWidget.h"
#include "ElaText.h"
#include "ElaToggleSwitch.h"


T_CameraPage::T_CameraPage(QWidget *parent) : T_BasePage(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // 顶部部分
    QHBoxLayout *topLayout = createTopLayout();
    mainLayout->addLayout(topLayout);

    // 创建标签页控件
    ElaTabWidget *tabWidget = new ElaTabWidget(this);

    // 创建并添加"信息"标签页
    QWidget *infoTab = createInfoTab();
    tabWidget->addTab(infoTab, "相机信息");

    // 创建并添加"温度控制"标签页
    QWidget *tempControlTab = createTempControlTab();
    tabWidget->addTab(tempControlTab, "温度控制");

    // 创建并添加"设置"标签页
    QWidget *settingsTab = createSettingsTab();
    tabWidget->addTab(settingsTab, "设置");

    // 创建并添加"图表"标签页
    QWidget *chartTab = createChartTab();
    tabWidget->addTab(chartTab, "温度图表");

    _indiPanel = new C_INDIPanel("CCD", this);
    tabWidget->addTab(_indiPanel, "INDI Panel");

    mainLayout->addWidget(tabWidget);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("相机面板");
    QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(topLayout);
    centerLayout->addSpacing(5);
    centerLayout->addLayout(mainLayout);
    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

T_CameraPage::~T_CameraPage() {}

QHBoxLayout *T_CameraPage::createTopLayout() {
    QHBoxLayout *topLayout = new QHBoxLayout();

    ElaComboBox *cameraCombo = new ElaComboBox(this);
    cameraCombo->addItem("N.I.N.A. Simulator Camera");

    /*
      auto createIconButton = [this](const ElaIcon &icon) {
        ElaIconButton *button = new ElaIconButton(this);
        button->setIcon(ElaIcon);
        button->setFixedSize(40, 40);
        button->setStyleSheet(R"(
                QPushButton {
                    background-color: #f0f0f0;
                    border: none;
                    border-radius: 20px;
                }
                QPushButton:hover {
                    background-color: #e0e0e0;
                }
            )");
        return button;
      };
    */
    auto createIconButton = [this](ElaIconType::IconName icon) {
        ElaIconButton *button = new ElaIconButton(icon, this);
        // button->setIcon(ElaIcon);
        button->setFixedSize(40, 40);
        return button;
    };

    ElaIconButton *settingsButton = createIconButton(ElaIconType::Gears);
    ElaIconButton *refreshButton = createIconButton(ElaIconType::ArrowsRotate);
    ElaIconButton *powerButton = createIconButton(ElaIconType::PowerOff);

    topLayout->addWidget(cameraCombo, 1);
    topLayout->addWidget(settingsButton);
    topLayout->addWidget(refreshButton);
    topLayout->addWidget(powerButton);

    return topLayout;
}

QWidget *T_CameraPage::createInfoTab() {
    QWidget *infoWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(infoWidget);

    // 创建"基本信息"组
    QGroupBox *basicInfoGroup = createInfoGroup("基本信息");
    QGridLayout *basicInfoLayout = new QGridLayout(basicInfoGroup);
    basicInfoLayout->addWidget(
        new InfoCard("相机名称", "N.I.N.A. Simulator Camera"), 0, 0);
    basicInfoLayout->addWidget(new InfoCard("驱动版本", "2.2.0.9001"), 0, 1);
    basicInfoLayout->addWidget(new InfoCard("传感器类型", "Monochrome"), 1, 0);
    basicInfoLayout->addWidget(new InfoCard("传感器名称", "Simulated Sensor"),
                               1, 1);

    // 创建"传感器信息"组
    QGroupBox *sensorInfoGroup = createInfoGroup("传感器信息");
    QGridLayout *sensorInfoLayout = new QGridLayout(sensorInfoGroup);
    sensorInfoLayout->addWidget(new InfoCard("传感器尺寸", "640 x 480"), 0, 0);
    sensorInfoLayout->addWidget(new InfoCard("最大分箱", "1 x 1"), 0, 1);
    sensorInfoLayout->addWidget(new InfoCard("像素大小", "3.8 μm x 3.8 μm"), 1,
                                0);
    sensorInfoLayout->addWidget(
        new InfoCard("曝光时间范围", "0 - 1.79769313486232E+308"), 1, 1);

    // 创建"其他信息"组
    QGroupBox *otherInfoGroup = createInfoGroup("其他信息");
    QGridLayout *otherInfoLayout = new QGridLayout(otherInfoGroup);
    otherInfoLayout->addWidget(new InfoCard("增益", "0"), 0, 0);
    otherInfoLayout->addWidget(new InfoCard("偏移量", "0"), 0, 1);

    // 添加描述
    QLabel *descriptionLabel = new QLabel(
        "描述: A basic simulator to generate random noise for a specific "
        "median or load in an image that will be displayed on capture");
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
    QGroupBox *groupBox = new QGroupBox(title);
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
    QWidget *tempWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(tempWidget);

    // 添加温度控制元素
    QHBoxLayout *dewHeaterLayout = new QHBoxLayout();
    ElaText *dewHeaterLabel = new ElaText("Dew heater", this);
    ElaToggleSwitch *dewHeaterSwitch = new ElaToggleSwitch(this);
    dewHeaterLayout->addWidget(dewHeaterLabel);
    dewHeaterLayout->addWidget(dewHeaterSwitch);
    dewHeaterLayout->addStretch();

    QHBoxLayout *sensorTempLayout = new QHBoxLayout();
    ElaText *sensorTempLabel = new ElaText("Sensor temperature", this);
    ElaText *sensorTempValue = new ElaText("--", this);
    sensorTempValue->setStyleSheet("font-weight: bold; color: #0078d4;");
    sensorTempLayout->addWidget(sensorTempLabel);
    sensorTempLayout->addWidget(sensorTempValue);
    sensorTempLayout->addStretch();

    layout->addLayout(dewHeaterLayout);
    layout->addLayout(sensorTempLayout);

    return tempWidget;
}

QWidget *T_CameraPage::createSettingsTab() {
    QWidget *containerWidget = new QWidget(this);
    QVBoxLayout *containerLayout = new QVBoxLayout(containerWidget);

    QWidget *settingsWidget = new QWidget(containerWidget);
    QVBoxLayout *layout = new QVBoxLayout(settingsWidget);
    layout->setContentsMargins(0, 0, 0, 10);  // 添加一些边距

    // 创建设置卡片
    layout->addWidget(createSettingCard("增益设置", ":/icons/gain.png",
                                        "调整相机的信号增益", "Default gain", 0,
                                        10000, 0));
    layout->addWidget(createSettingCard("偏移设置", ":/icons/offset.png",
                                        "调整相机的信号偏移", "Default offset",
                                        0, 1000, 0));
    layout->addWidget(createSettingCard("USB限制", ":/icons/usb.png",
                                        "设置USB传输速度限制", "USB limit", 0,
                                        100, 0));

    layout->addStretch();
    containerLayout->addWidget(settingsWidget);

    return containerWidget;
}

QWidget *T_CameraPage::createSettingCard(const QString &title,
                                         const QString &iconPath,
                                         const QString &description,
                                         const QString &settingName, int min,
                                         int max, int defaultValue) {
    QGroupBox *card = new QGroupBox(this);
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    QVBoxLayout *cardLayout = new QVBoxLayout(card);

    // 标题和图标
    QHBoxLayout *titleLayout = new QHBoxLayout();
    ElaText *iconLabel = new ElaText(card);
    iconLabel->setPixmap(QPixmap(iconPath).scaled(24, 24, Qt::KeepAspectRatio,
                                                  Qt::SmoothTransformation));
    ElaText *titleText = new ElaText(title, card);
    titleText->setTextPixelSize(16);
    titleLayout->addWidget(iconLabel);
    titleLayout->addWidget(titleText);
    titleLayout->addStretch();

    // 描述
    ElaText *descriptionText = new ElaText(description, card);
    descriptionText->setTextPixelSize(12);

    // 设置控件
    QHBoxLayout *settingLayout = new QHBoxLayout();
    ElaText *settingLabel = new ElaText(settingName, card);
    settingLabel->setTextPixelSize(14);
    ElaSlider *slider = new ElaSlider(Qt::Horizontal, card);
    slider->setRange(min, max);
    slider->setValue(defaultValue);
    ElaSpinBox *spinBox = new ElaSpinBox(card);
    spinBox->setRange(min, max);
    spinBox->setValue(defaultValue);

    settingLayout->addWidget(settingLabel);
    settingLayout->addWidget(slider);
    settingLayout->addWidget(spinBox);

    connect(slider, &ElaSlider::valueChanged, spinBox, &ElaSpinBox::setValue);
    connect(spinBox, QOverload<int>::of(&ElaSpinBox::valueChanged), slider,
            &ElaSlider::setValue);

    cardLayout->addLayout(titleLayout);
    cardLayout->addWidget(descriptionText);
    cardLayout->addLayout(settingLayout);

    return card;
}

QWidget *T_CameraPage::createChartTab() {
    QWidget *chartWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(chartWidget);

    QChartView *chartView = createTemperatureChart();
    layout->addWidget(chartView);

    return chartWidget;
}

void T_CameraPage::addInfoRow(QGridLayout *layout, const QString &label,
                              const QString &value) {
    int row = layout->rowCount();
    ElaText *labelWidget = new ElaText(label + ":", this);
    labelWidget->setTextPixelSize(14);
    ElaText *valueWidget = new ElaText(value, this);
    valueWidget->setTextPixelSize(14);
    layout->addWidget(labelWidget, row, 0);
    layout->addWidget(valueWidget, row, 1);
}

void T_CameraPage::addSettingRow(QGridLayout *layout, const QString &label,
                                 int defaultValue) {
    int row = layout->rowCount();
    ElaText *labelWidget = new ElaText(label, this);
    labelWidget->setTextPixelSize(14);
    ElaSlider *slider = new ElaSlider(Qt::Horizontal, this);
    slider->setRange(0, 100);
    slider->setValue(defaultValue);
    ElaSpinBox *spinBox = new ElaSpinBox(this);
    spinBox->setRange(0, 100);
    spinBox->setValue(defaultValue);

    layout->addWidget(labelWidget, row, 0);
    layout->addWidget(slider, row, 1);
    layout->addWidget(spinBox, row, 2);

    connect(slider, &ElaSlider::valueChanged, spinBox, &ElaSpinBox::setValue);
    connect(spinBox, QOverload<int>::of(&ElaSpinBox::valueChanged), slider,
            &ElaSlider::setValue);
}

QChartView *T_CameraPage::createTemperatureChart() {
    QLineSeries *series = new QLineSeries();
    series->setName("Sensor temperature");

    // Add some dummy data
    QDateTime startTime = QDateTime::currentDateTime().addDays(-7);
    for (int i = 0; i < 8; ++i) {
        series->append(startTime.addDays(i).toMSecsSinceEpoch(),
                       rand() % 100 - 50);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Temperature History");

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(8);
    axisX->setFormat("yyyy-MM-dd");
    axisX->setTitleText("Date");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(-50, 100);
    axisY->setTitleText("Temperature (°C)");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView;
}