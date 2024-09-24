#include "T_Switch.h"

#include <QHBoxLayout>
#include <QVBoxLayout>


#include "ElaPushButton.h"
#include "ElaSlider.h"
#include "ElaText.h"

WeatherControlPanel::WeatherControlPanel(QWidget *parent) : QWidget(parent) {
    createLayout();

    // 连接信号槽，处理按钮点击和滑动条变化
    connect(power1Button, &ElaPushButton::clicked, [this]() {
        bool isOn = (power1Button->text() == "Power1 OFF");
        power1Button->setText(isOn ? "Power1 ON" : "Power1 OFF");
        emit power1Changed(isOn);  // 发射信号
    });

    connect(power2Button, &ElaPushButton::clicked, [this]() {
        bool isOn = (power2Button->text() == "Power2 OFF");
        power2Button->setText(isOn ? "Power2 ON" : "Power2 OFF");
        emit power2Changed(isOn);  // 发射信号
    });

    connect(lightboxSlider, &ElaSlider::valueChanged, this,
            &WeatherControlPanel::lightBoxValueChanged);
    connect(flatPanelSlider, &ElaSlider::valueChanged, this,
            &WeatherControlPanel::flatPanelValueChanged);
}

void WeatherControlPanel::createLayout() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Gauges Section
    QVBoxLayout *gaugesLayout = new QVBoxLayout();

    scopeLabel = new ElaText("Scope Parked: 0", this);
    temperatureLabel = new ElaText("Temperature: 12°C", this);
    rainLabel = new ElaText("Raining: No", this);
    cloudLabel = new ElaText("Cloudy: 0", this);
    humidityLabel = new ElaText("Humidity: 50%", this);

    gaugesLayout->addWidget(scopeLabel);
    gaugesLayout->addWidget(temperatureLabel);
    gaugesLayout->addWidget(rainLabel);
    gaugesLayout->addWidget(cloudLabel);
    gaugesLayout->addWidget(humidityLabel);

    mainLayout->addLayout(gaugesLayout);

    // Switches Section
    QHBoxLayout *switchesLayout = new QHBoxLayout();

    power1Button = new ElaPushButton("Power1 OFF", this);
    power2Button = new ElaPushButton("Power2 OFF", this);

    switchesLayout->addWidget(power1Button);
    switchesLayout->addWidget(power2Button);
    mainLayout->addLayout(switchesLayout);

    // Lightbox and Flat Panel Controls
    QHBoxLayout *lightboxLayout = new QHBoxLayout();
    ElaText *lightboxLabel = new ElaText("Light Box: ", this);
    lightboxSlider = new ElaSlider(Qt::Horizontal, this);
    lightboxSlider->setRange(0, 100);

    lightboxLayout->addWidget(lightboxLabel);
    lightboxLayout->addWidget(lightboxSlider);

    QHBoxLayout *flatPanelLayout = new QHBoxLayout();
    ElaText *flatPanelLabel = new ElaText("Flat Panel: ", this);
    flatPanelSlider = new ElaSlider(Qt::Horizontal, this);
    flatPanelSlider->setRange(0, 255);

    flatPanelLayout->addWidget(flatPanelLabel);
    flatPanelLayout->addWidget(flatPanelSlider);

    mainLayout->addLayout(lightboxLayout);
    mainLayout->addLayout(flatPanelLayout);
}
