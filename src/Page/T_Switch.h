#ifndef WEATHERCONTROLPANEL_H
#define WEATHERCONTROLPANEL_H

#include "T_BasePage.h"

class ElaText;
class ElaPushButton;
class ElaSlider;

class WeatherControlPanel : public QWidget {
    Q_OBJECT

public:
    explicit WeatherControlPanel(QWidget *parent = nullptr);

signals:
    // 回调点：电源开关状态变化
    void power1Changed(bool state);
    void power2Changed(bool state);
    // 回调点：LightBox和Flat Panel值变化
    void lightBoxValueChanged(int value);
    void flatPanelValueChanged(int value);

private:
    // 私有方法：创建布局
    void createLayout();

    // UI 控件
    ElaText *scopeLabel;
    ElaText *temperatureLabel;
    ElaText *rainLabel;
    ElaText *cloudLabel;
    ElaText *humidityLabel;

    ElaPushButton *power1Button;
    ElaPushButton *power2Button;

    ElaSlider *lightboxSlider;
    ElaSlider *flatPanelSlider;
};

#endif  // WEATHERCONTROLPANEL_H
