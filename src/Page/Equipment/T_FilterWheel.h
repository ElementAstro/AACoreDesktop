#ifndef T_FILTERWHEELPAGE_H
#define T_FILTERWHEELPAGE_H

#include "ElaToggleSwitch.h"
#include "T_BasePage.h"

#include <QChartView>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>


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

class T_FilterWheelPage : public T_BasePage {
    Q_OBJECT
public:
    explicit T_FilterWheelPage(QWidget *parent = nullptr);
    ~T_FilterWheelPage();

protected:
    QHBoxLayout *createTopLayout();
    QWidget *createInfoTab();
    QWidget *createControlTab();
    QWidget *createFiltersTab();
    QWidget *createSettingsTab();
    QGroupBox *createInfoGroup(const QString &title);
    void updateFilterStatus();

private slots:
    void onPowerButtonClicked();
    void onRefreshButtonClicked();
    void onChangeFilterClicked();
    void onAddFilterClicked();
    void onRemoveFilterClicked();

private:
    ElaToggleSwitch *_powerSwitch{nullptr};
    QLabel *_statusLabel{nullptr};
    QTimer *_statusTimer{nullptr};
    ElaComboBox *_filterWheelCombo{nullptr};
    ElaIconButton *_powerButton{nullptr};
    ElaIconButton *_refreshButton{nullptr};
    ElaPushButton *_changeFilterButton{nullptr};
    ElaPushButton *_addFilterButton{nullptr};
    ElaPushButton *_removeFilterButton{nullptr};
    ElaLineEdit *_newFilterEdit{nullptr};
};

#endif  // T_FILTERWHEELPAGE_H