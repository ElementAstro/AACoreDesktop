#ifndef T_CCAMETAPAGE_H
#define T_CCAMETAPAGE_H

#include "ElaToggleSwitch.h"
#include "T_BasePage.h"

#include <QtCharts/QChartView>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QLabel>

class QVBoxLayout;
class QChartView;
class QGridLayout;
class QGroupBox;
class ElaIconButton;
class ElaToggleSwitch;
class C_INDIPanel;

class T_CameraPage : public T_BasePage {
    Q_OBJECT

public:
    explicit T_CameraPage(QWidget *parent = nullptr);
    ~T_CameraPage();

protected:
    QHBoxLayout *createTopLayout();
    QWidget *createInfoTab();
    QGroupBox *createInfoGroup(const QString &title);
    QWidget *createTempControlTab();
    QWidget *createSettingsTab();
    QWidget *createSettingCard(const QString &title, const QString &iconPath,
                               const QString &description,
                               const QString &settingName, int min, int max,
                               int defaultValue);
    QWidget *createChartTab();
    void addInfoRow(QGridLayout *layout, const QString &label,
                    const QString &value);
    void addSettingRow(QGridLayout *layout, const QString &label,
                       int defaultValue);
    QGroupBox *createInfoGroup(const QString &title,
                               const QVector<QPair<QString, QString>> &items);
    QChartView *createTemperatureChart();
    
private slots:
    void onDewHeaterToggled(bool checked);
    void onPowerButtonClicked();
    void onRefreshButtonClicked();
    void updateSensorTemperature();
    void onGainChanged(int value);
    void onOffsetChanged(int value);
    void onUSBLimitChanged(int value);

private:
    ElaToggleSwitch *_toggleSwitch{nullptr};
    C_INDIPanel *_indiPanel{nullptr};
    QLabel *_sensorTempValue{nullptr};
    QTimer *_tempUpdateTimer{nullptr};
    ElaIconButton *_powerButton{nullptr};
    ElaIconButton *_refreshButton{nullptr};
};

#endif  // T_CCAMETAPAGE_H