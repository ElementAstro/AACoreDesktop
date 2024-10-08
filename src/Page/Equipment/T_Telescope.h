#ifndef T_TELESCOPEPAGE_H
#define T_TELESCOPEPAGE_H

#include "ElaToggleSwitch.h"
#include "T_BasePage.h"

class QVBoxLayout;
class QChartView;
class QGridLayout;
class QGroupBox;
class ElaToggleSwitch;
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

    ElaToggleSwitch *_toggleSwitch{nullptr};
};

#endif  // T_TELESCOPEPAGE_H
