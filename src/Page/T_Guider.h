#ifndef T_GUIDERPAGE_H
#define T_GUIDERPAGE_H

#include "ElaToggleSwitch.h"
#include "T_BasePage.h"

class QVBoxLayout;
class QChartView;
class QGridLayout;
class QGroupBox;
class ElaToggleSwitch;
class ElaIconButton;

class T_PHD2SetupDialog;
class T_GuiderPage : public T_BasePage {
  Q_OBJECT
public:
  explicit T_GuiderPage(QWidget *parent = nullptr);

protected:
  QHBoxLayout *createTopLayout();
  QWidget *createInfoTab();
  QWidget *createControlTab();
  QWidget *createSettingsTab();
  QGroupBox *createInfoGroup(const QString &title);
  QChartView *createGuideChart();
  void onRAColorButtonClicked();
  void onDecColorButtonClicked();

  ElaToggleSwitch *_toggleSwitch{nullptr};

  ElaIconButton *_settingButton{nullptr};
  T_PHD2SetupDialog *_phd2SetupDialog{nullptr};
};

#endif // T_GUIDERPAGE_H
