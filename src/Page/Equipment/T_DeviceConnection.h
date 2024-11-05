#ifndef T_DeviceConnection_H
#define T_DeviceConnection_H

#include "T_BasePage.h"

class ElaToggleSwitch;
class ElaToggleButton;
class ElaComboBox;
class ElaMultiSelectComboBox;
class ElaMessageButton;
class ElaCheckBox;
class ElaLineEdit;
class ElaSpinBox;
class ElaSlider;
class ElaRadioButton;
class ElaProgressBar;
class ElaPushButton;

class InfoCard;

class T_DeviceConnection : public T_BasePage {
    Q_OBJECT
public:
    T_DeviceConnection(QWidget *parent = nullptr);
    ~T_DeviceConnection();

public slots:
    void onSaveButtonClicked();
    void onResetButtonClicked();
    void onWebManagerButtonClicked();
    void onScanButtonClicked();
    void onScriptsButtonClicked();
    void onConnectionStatusUpdated(bool connected);

private:
    void createProfileSection(QVBoxLayout *mainLayout);
    void createDevicesSection(QVBoxLayout *mainLayout);
    void createBottomButtons(QVBoxLayout *mainLayout);
    void updateConnectionStatus();

    // Profile Section Components
    ElaLineEdit *_nameEdit{nullptr};
    ElaCheckBox *_autoConnectCheck{nullptr};
    ElaCheckBox *_portSelectorCheck{nullptr};
    ElaCheckBox *_siteInfoCheck{nullptr};
    ElaRadioButton *_localRadio{nullptr};
    ElaRadioButton *_remoteRadio{nullptr};
    ElaLineEdit *_hostEdit{nullptr};
    ElaSpinBox *_portSpinBox{nullptr};
    ElaComboBox *_guidingCombo{nullptr};
    ElaCheckBox *_indiWebManagerCheck{nullptr};
    ElaLineEdit *_indiWebManagerHost{nullptr};
    ElaSpinBox *_indiWebManagerPort{nullptr};
    ElaPushButton *_webManagerButton{nullptr};
    ElaPushButton *_scanButton{nullptr};
    ElaPushButton *_scriptsButton{nullptr};

    // Devices Section Components
    ElaComboBox *_mountCombo{nullptr};
    ElaComboBox *_camera1Combo{nullptr};
    ElaComboBox *_camera2Combo{nullptr};
    ElaComboBox *_focuserCombo{nullptr};
    ElaComboBox *_filterCombo{nullptr};
    ElaComboBox *_aoCombo{nullptr};
    ElaComboBox *_domeCombo{nullptr};
    ElaComboBox *_weatherCombo{nullptr};
    ElaComboBox *_aux1Combo{nullptr};
    ElaComboBox *_aux2Combo{nullptr};
    ElaComboBox *_aux3Combo{nullptr};
    ElaComboBox *_aux4Combo{nullptr};

    // Bottom Buttons
    ElaPushButton *_saveButton{nullptr};
    ElaPushButton *_resetButton{nullptr};

    // Info Cards
    InfoCard *_connectionStatusCard{nullptr};
    InfoCard *_deviceStatusCard{nullptr};

    QTimer *_statusTimer{nullptr};
};

#endif  // T_DeviceConnection_H
