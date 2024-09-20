#ifndef T_DeviceConnection_H
#define T_DeviceConnection_H

#include "T_BasePage.h"

class ElaToggleSwitch;
class ElaToggleButton;
class ElaComboBox;
class ElaMultiSelectComboBox;
class ElaMessageButton;
class ElaCheckBox;
class ElaSpinBox;
class ElaSlider;
class ElaRadioButton;
class ElaProgressBar;
class T_DeviceConnection : public T_BasePage {
  Q_OBJECT
public:
  T_DeviceConnection(QWidget *parent = nullptr);
  ~T_DeviceConnection();

  void onSaveButtonClicked();

  void onCloseButtonClicked();

  void onWebManagerButtonClicked();

  void onScanButtonClicked();

  void onScriptsButtonClicked();

private:
  ElaToggleSwitch *_toggleSwitch{nullptr};
  ElaToggleButton *_toggleButton{nullptr};
  ElaComboBox *_comboBox{nullptr};
  ElaMultiSelectComboBox *_multiSelectComboBox{nullptr};
  ElaMessageButton *_messageButton{nullptr};
  ElaMessageButton *_infoMessageButton{nullptr};
  ElaMessageButton *_warningMessageButton{nullptr};
  ElaMessageButton *_errorMessageButton{nullptr};
  ElaCheckBox *_checkBox{nullptr};
  ElaSpinBox *_spinBox{nullptr};
  ElaSlider *_slider{nullptr};
  ElaRadioButton *_radioButton{nullptr};
  ElaProgressBar *_progressBar{nullptr};
};

#endif // T_DeviceConnection_H
