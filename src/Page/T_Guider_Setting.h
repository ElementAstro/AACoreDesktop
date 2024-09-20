#ifndef PHD2SETUPDIALOG_H
#define PHD2SETUPDIALOG_H

#include "ElaWidget.h"

class QLineEdit;
class QVBoxLayout;

class ElaSpinBox;
class ElaCheckBox;
class ElaPushButton;
class ElaMessageButton;

class T_PHD2SetupDialog : public ElaWidget {
  Q_OBJECT

public:
  explicit T_PHD2SetupDialog(QWidget *parent = nullptr);

private slots:
    void accept();
    void reject();

private:
  QVBoxLayout* createTopLayout(const QString& description);
  QLineEdit *phd2PathEdit;
  QLineEdit *serverUrlEdit;
  ElaSpinBox *serverPortSpin;
  ElaSpinBox *instanceNumberSpin;
  ElaSpinBox *ditherPixelsSpin;
  ElaCheckBox *ditherRaOnlyCheck;
  ElaSpinBox *settlePixelToleranceSpin;
  ElaSpinBox *minSettleTimeSpin;
  ElaSpinBox *settleTimeoutSpin;
  ElaCheckBox *guidingStartRetryCheck;
  ElaSpinBox *guidingStartTimeoutSpin;
  ElaSpinBox *roiPercentageSpin;
  ElaMessageButton *okButton;
  ElaMessageButton *cancelButton;

  void setupUi();
};

#endif // PHD2SETUPDIALOG_H