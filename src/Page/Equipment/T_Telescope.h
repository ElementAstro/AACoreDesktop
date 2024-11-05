#ifndef T_TELESCOPEPAGE_H
#define T_TELESCOPEPAGE_H

#include "ElaToggleSwitch.h"
#include "T_BasePage.h"

#include <QChartView>
#include <QGroupBox>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>

class QVBoxLayout;
class QChartView;
class QGridLayout;
class ElaToggleSwitch;
class ElaComboBox;
class ElaIconButton;
class ElaPushButton;
class ElaSlider;
class ElaSpinBox;
class ElaTabWidget;
class ElaText;
class C_InfoCard;

class T_TelescopePage : public T_BasePage {
    Q_OBJECT
public:
    explicit T_TelescopePage(QWidget *parent = nullptr);
    ~T_TelescopePage();

protected:
    QHBoxLayout *createTopLayout();
    QWidget *createInfoTab();
    QWidget *createControlTab();
    QWidget *createSettingsTab();
    QGroupBox *createInfoGroup(const QString &title);
    void addCoordinateRow(QGridLayout *layout, const QString &label,
                          const QString &value1, const QString &value2,
                          const QString &value3);
    void addRateControl(QVBoxLayout *layout, const QString &label,
                        double defaultValue);
    void addReversedToggle(QVBoxLayout *layout, const QString &label);

private slots:
    void onPowerButtonClicked();
    void onRefreshButtonClicked();
    void onSlewButtonClicked();
    void onDirectionButtonClicked(const QString &direction);
    void onParkButtonClicked();
    void onSetParkPointClicked();
    void onHomeButtonClicked();
    void onDewHeaterToggled(bool checked);
    void updateTrackingStatus();
    void onTrackingRateChanged(int value);
    void onSecondaryRateChanged(int value);
    void onMainAxisReversed(bool checked);
    void onSecondaryAxisReversed(bool checked);

private:
    ElaToggleSwitch *_dewHeaterSwitch{nullptr};
    QLabel *_trackingStatusLabel{nullptr};
    QTimer *_trackingTimer{nullptr};
    ElaComboBox *_trackingRateCombo{nullptr};
    ElaSlider *_mainRateSlider{nullptr};
    ElaSpinBox *_mainRateSpinBox{nullptr};
    ElaSlider *_secondaryRateSlider{nullptr};
    ElaSpinBox *_secondaryRateSpinBox{nullptr};
    ElaToggleSwitch *_mainAxisReversedSwitch{nullptr};
    ElaToggleSwitch *_secondaryAxisReversedSwitch{nullptr};
    ElaIconButton *_powerButton{nullptr};
    ElaIconButton *_refreshButton{nullptr};
    ElaPushButton *_slewButton{nullptr};
    ElaPushButton *_parkButton{nullptr};
    ElaPushButton *_setParkPointButton{nullptr};
    ElaPushButton *_homeButton{nullptr};
};

#endif  // T_TELESCOPEPAGE_H