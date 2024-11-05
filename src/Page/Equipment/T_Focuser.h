#ifndef T_FOCUSERPAGE_H
#define T_FOCUSERPAGE_H

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
class QGroupBox;
class ElaComboBox;
class ElaIconButton;
class ElaPushButton;
class ElaSlider;
class ElaSpinBox;
class ElaTabWidget;
class ElaText;
class C_InfoCard;
class ElaLineEdit;

class T_FocuserPage : public T_BasePage {
    Q_OBJECT
public:
    explicit T_FocuserPage(QWidget *parent = nullptr);
    ~T_FocuserPage();

protected:
    QHBoxLayout* createTopLayout();
    QWidget* createInfoTab();
    QWidget* createControlTab();
    QWidget* createSettingsTab();
    QGroupBox* createInfoGroup(const QString &title);
    void addInfoCard(QGridLayout *layout, const QString &label, const QString &value);
    void addControlRow(QHBoxLayout *layout, const QString &label, QWidget *widget);

private slots:
    void onPowerButtonClicked();
    void onRefreshButtonClicked();
    void onMoveButtonClicked();
    void onMoveLeftMostClicked();
    void onMoveLeftClicked();
    void onMoveRightClicked();
    void onMoveRightMostClicked();
    void onMoveToPositionClicked();
    void onTempCompToggled(bool checked);
    void updateFocuserStatus();

private:
    ElaToggleSwitch *_tempCompSwitch{nullptr};
    QLabel *_statusLabel{nullptr};
    QLabel *_temperatureLabel{nullptr};
    ElaComboBox *_focuserCombo{nullptr};
    ElaIconButton *_powerButton{nullptr};
    ElaIconButton *_refreshButton{nullptr};
    ElaPushButton *_moveButton{nullptr};
    ElaPushButton *_moveLeftMostButton{nullptr};
    ElaPushButton *_moveLeftButton{nullptr};
    ElaPushButton *_moveRightButton{nullptr};
    ElaPushButton *_moveRightMostButton{nullptr};
    ElaLineEdit *_targetPositionEdit{nullptr};
    ElaPushButton *_moveToPositionButton{nullptr};
    QTimer *_statusTimer{nullptr};
};

#endif  // T_FOCUSERPAGE_H