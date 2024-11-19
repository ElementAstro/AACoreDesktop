#include "T_Switch.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>

#include "Def.h"
#include "ElaPushButton.h"
#include "ElaSlider.h"
#include "ElaText.h"
#include "ElaIcon.h"
#include "ElaTabWidget.h"
#include "ElaIconButton.h"
#include "ElaComboBox.h"
#include "ElaToggleSwitch.h"
#include "Components/C_INDIPanel.h"

namespace {
    constexpr int kSpacing20 = 20;
    constexpr int kSpacing10 = 10;
    constexpr int kMargin20 = 20;
    constexpr int kMargin10 = 10;
    constexpr int kFixedSize40 = 40;
    constexpr int kFixedHeight30 = 30;
}

T_SwitchPage::T_SwitchPage(QWidget *parent) 
    : T_BasePage(parent)
    , serialConfigDialog(nullptr)
    , isPowered(false)
    , isLightboxOn(false)
    , isFlatPanelOn(false) {
    
    createLayout();

    // 状态更新定时器
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &T_SwitchPage::updateStatus);
    updateTimer->start(2000);
}

T_SwitchPage::~T_SwitchPage() = default;

void T_SwitchPage::createLayout() {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(kSpacing20);
    mainLayout->setContentsMargins(kMargin20, kMargin20, kMargin20, kMargin20);

    // 顶部工具栏
    mainLayout->addLayout(createTopLayout());

    // 标签页控件
    tabWidget = new ElaTabWidget(this);
    tabWidget->addTab(createInfoTab(), "信息");
    tabWidget->addTab(createControlTab(), "控制");
    tabWidget->addTab(createSettingsTab(), "设置");
    tabWidget->addTab(createChartTab(), "图表");
    
    auto *indiPanel = new C_INDIPanel("Switch INDI", this);
    tabWidget->addTab(indiPanel, "INDI Panel");

    mainLayout->addWidget(tabWidget);

    // 中心部件设置
    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("电源控制面板");
    auto *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

QHBoxLayout* T_SwitchPage::createTopLayout() {
    auto *topLayout = new QHBoxLayout();

    // 设备选择下拉框
    auto *deviceCombo = new ElaComboBox(this);
    deviceCombo->addItem("AACore Switch");
    connect(deviceCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [](int) {});

    // 创建图标按钮
    auto createIconButton = [this](ElaIconType::IconName icon) -> ElaIconButton* {
        auto *button = new ElaIconButton(icon, this);
        button->setFixedSize(kFixedSize40, kFixedSize40);
        return button;
    };

    _powerButton = createIconButton(ElaIconType::PowerOff);
    _refreshButton = createIconButton(ElaIconType::ArrowsRotate);
    settingsButton = new ElaPushButton("设置", this);
    settingsButton->setFixedSize(kFixedSize40, kFixedSize40);

    connect(_powerButton, &ElaIconButton::clicked, this, &T_SwitchPage::onPowerButtonClicked);
    connect(_refreshButton, &ElaIconButton::clicked, this, &T_SwitchPage::onRefreshButtonClicked);
    connect(settingsButton, &ElaPushButton::clicked, this, &T_SwitchPage::onSettingsButtonClicked);

    topLayout->addWidget(deviceCombo, 1);
    topLayout->addWidget(settingsButton);
    topLayout->addWidget(_refreshButton);
    topLayout->addWidget(_powerButton);

    return topLayout;
}

QWidget* T_SwitchPage::createInfoTab() {
    auto *infoWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(infoWidget);
    layout->setSpacing(kSpacing10);

    // 基本信息组
    auto *basicGroup = createInfoGroup("基本信息");
    auto *basicLayout = new QGridLayout(basicGroup);
    
    deviceCard = new InfoCard("设备名称", "AACore Switch", this);
    statusCard = new InfoCard("连接状态", "已连接", this);
    power1Card = new InfoCard("电源1状态", "关闭", this);
    power2Card = new InfoCard("电源2状态", "关闭", this);
    lightboxCard = new InfoCard("光箱亮度", "0%", this);
    flatPanelCard = new InfoCard("平场板亮度", "0%", this);

    basicLayout->addWidget(deviceCard, 0, 0);
    basicLayout->addWidget(statusCard, 0, 1);
    basicLayout->addWidget(power1Card, 1, 0);
    basicLayout->addWidget(power2Card, 1, 1);
    basicLayout->addWidget(lightboxCard, 2, 0);
    basicLayout->addWidget(flatPanelCard, 2, 1);

    layout->addWidget(basicGroup);
    layout->addStretch();

    return infoWidget;
}

QWidget* T_SwitchPage::createControlTab() {
    auto *controlWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(controlWidget);
    layout->setSpacing(kSpacing20);

    // 电源控制组
    auto *powerGroup = createInfoGroup("电源控制");
    auto *powerLayout = new QGridLayout(powerGroup);

    power1Button = new ElaPushButton("电源1", this);
    power2Button = new ElaPushButton("电源2", this);
    power1Button->setFixedHeight(kFixedHeight30);
    power2Button->setFixedHeight(kFixedHeight30);

    powerLayout->addWidget(power1Button, 0, 0);
    powerLayout->addWidget(power2Button, 0, 1);

    layout->addWidget(powerGroup);

    // 亮度控制组
    auto *brightnessGroup = createInfoGroup("亮度控制");
    auto *brightnessLayout = new QVBoxLayout(brightnessGroup);

            addSliderControl(brightnessLayout, "平场板亮度", flatPanelSlider, 0, 255, 128);
    
        // 添加开关控件
        auto *switchLayout = new QGridLayout();
        lightboxSwitch = new ElaToggleSwitch( this);
        flatPanelSwitch = new ElaToggleSwitch(this);
        
        switchLayout->addWidget(lightboxSwitch, 0, 0);
        switchLayout->addWidget(flatPanelSwitch, 0, 1);
        
        brightnessLayout->addLayout(switchLayout);
        layout->addWidget(brightnessGroup);
        layout->addStretch();
    
        // 连接信号槽
        connect(lightboxSlider, &ElaSlider::valueChanged, 
                this, &T_SwitchPage::onLightboxSliderChanged);
        connect(flatPanelSlider, &ElaSlider::valueChanged,
                this, &T_SwitchPage::onFlatPanelSliderChanged);
        connect(power1Button, &ElaPushButton::clicked, this, [this]() {
            emit power1Changed(!power1Button->isChecked());
            power1Button->setChecked(!power1Button->isChecked());
            updateStatus();
        });
        connect(power2Button, &ElaPushButton::clicked, this, [this]() {
            emit power2Changed(!power2Button->isChecked());
            power2Button->setChecked(!power2Button->isChecked());
            updateStatus();
        });
    
        return controlWidget;
    }
    
    QWidget* T_SwitchPage::createSettingsTab() {
        auto *settingsWidget = new QWidget(this);
        auto *layout = new QVBoxLayout(settingsWidget);
        layout->setSpacing(kSpacing20);
    
        // 串口设置组
        auto *serialGroup = createInfoGroup("串口设置");
        auto *serialLayout = new QVBoxLayout(serialGroup);
        
        auto *openSerialButton = new ElaPushButton("配置串口", this);
        connect(openSerialButton, &ElaPushButton::clicked, 
                this, &T_SwitchPage::onSettingsButtonClicked);
        
        serialLayout->addWidget(openSerialButton);
        layout->addWidget(serialGroup);
        layout->addStretch();
    
        return settingsWidget;
    }
    
    QWidget* T_SwitchPage::createChartTab() {
        auto *chartWidget = new QWidget(this);
        auto *layout = new QVBoxLayout(chartWidget);
        layout->setSpacing(kSpacing20);
    
        // 使用 QtCharts 创建图表
        auto *chart = new QChart();
        chart->setTitle("亮度历史记录");
        
        // 创建数据序列
        auto *lightboxSeries = new QLineSeries();
        lightboxSeries->setName("光箱亮度");
        auto *flatPanelSeries = new QLineSeries();
        flatPanelSeries->setName("平场板亮度");
    
        // 添加一些示例数据点
        QDateTime currentTime = QDateTime::currentDateTime();
        for(int i = -10; i <= 0; ++i) {
            lightboxSeries->append(currentTime.addSecs(i * 60).toMSecsSinceEpoch(), 
                                 rand() % 100);
            flatPanelSeries->append(currentTime.addSecs(i * 60).toMSecsSinceEpoch(), 
                                  rand() % 255);
        }
    
        chart->addSeries(lightboxSeries);
        chart->addSeries(flatPanelSeries);
    
        // 创建坐标轴
        auto *axisX = new QDateTimeAxis;
        axisX->setFormat("HH:mm");
        axisX->setTitleText("时间");
        chart->addAxis(axisX, Qt::AlignBottom);
        lightboxSeries->attachAxis(axisX);
        flatPanelSeries->attachAxis(axisX);
    
        auto *axisY = new QValueAxis;
        axisY->setTitleText("亮度");
        chart->addAxis(axisY, Qt::AlignLeft);
        lightboxSeries->attachAxis(axisY);
        flatPanelSeries->attachAxis(axisY);
    
        auto *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        layout->addWidget(chartView);
    
        return chartWidget;
    }
    
    QGroupBox* T_SwitchPage::createInfoGroup(const QString &title) {
        auto *group = new QGroupBox(title, this);
        group->setStyleSheet("QGroupBox { font-weight: bold; }");
        return group;
    }
    
    void T_SwitchPage::addSliderControl(QVBoxLayout *layout, const QString &label,
                                       ElaSlider *&slider, int min, int max, 
                                       int default_value) {
        auto *sliderLayout = new QHBoxLayout();
        auto *labelWidget = new ElaText(label + ":", this);
        
        slider = new ElaSlider(Qt::Horizontal, this);
        slider->setRange(min, max);
        slider->setValue(default_value);
        
        auto *valueLabel = new ElaText(QString::number(default_value), this);
        connect(slider, &ElaSlider::valueChanged, valueLabel, 
                [valueLabel](int value) {
                    valueLabel->setText(QString::number(value));
                });
    
        sliderLayout->addWidget(labelWidget);
        sliderLayout->addWidget(slider);
        sliderLayout->addWidget(valueLabel);
        
        layout->addLayout(sliderLayout);
    }
    
    void T_SwitchPage::onPowerButtonClicked() {
        isPowered = !isPowered;
        _powerButton->setIcon(isPowered ? ElaIcon::getInstance()->getElaIcon(ElaIconType::Plug) : ElaIcon::getInstance()->getElaIcon(ElaIconType::PowerOff));
        updateStatus();
    }
    
    void T_SwitchPage::onRefreshButtonClicked() {
        updateStatus();
    }
    
    void T_SwitchPage::updateStatus() {
        // 更新信息卡片显示
        statusCard->setValue(isPowered ? "已连接" : "未连接");
        power1Card->setValue(power1Button->isChecked() ? "开启" : "关闭");
        power2Card->setValue(power2Button->isChecked() ? "开启" : "关闭");
        lightboxCard->setValue(QString("%1%").arg(lightboxSlider->value()));
        flatPanelCard->setValue(QString("%1").arg(flatPanelSlider->value()));
    }
    
    void T_SwitchPage::onLightboxSliderChanged(int value) {
        emit lightBoxValueChanged(value);
        updateStatus();
    }
    
    void T_SwitchPage::onFlatPanelSliderChanged(int value) {
        emit flatPanelValueChanged(value);
        updateStatus();
    }
    
    void T_SwitchPage::onSettingsButtonClicked() {
        if (!serialConfigDialog) {
            serialConfigDialog = new T_SwitchConfig(this);
        }
        serialConfigDialog->exec();
    }