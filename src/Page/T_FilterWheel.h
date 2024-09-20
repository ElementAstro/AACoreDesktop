#ifndef T_FILTERWHEELPAGE_H
#define T_FILTERWHEELPAGE_H

#include "ElaToggleSwitch.h"
#include "T_BasePage.h"

class QVBoxLayout;
class QChartView;
class QGridLayout;
class QGroupBox;
class ElaToggleSwitch;
class T_FilterWheelPage : public T_BasePage {
  Q_OBJECT
public:
  explicit T_FilterWheelPage(QWidget *parent = nullptr);

protected:
  QHBoxLayout *createTopLayout();
  QWidget *createInfoTab();
  QWidget *createControlTab();
  QWidget *createSettingsTab();
  QWidget* createFiltersTab();
  QGroupBox *createInfoGroup(const QString &title);

  ElaToggleSwitch *_toggleSwitch{nullptr};
};

#endif // T_FILTERWHEELPAGE_H
