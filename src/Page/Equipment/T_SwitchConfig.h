#ifndef T_SWITCHCONFIG_H
#define T_SWITCHCONFIG_H

#include <QComboBox>
#include <QDialog>
#include <QStringList>
#include <QThread>

#include "Serial/T_SerialConfig.h"

class ElaComboBox;
class ElaLineEdit;
class ElaPushButton;

class T_SwitchConfig : public QDialog {
    Q_OBJECT

public:
    explicit T_SwitchConfig(QWidget *parent = nullptr);
    ~T_SwitchConfig();

private slots:
    void onOkClicked();
    void onCancelClicked();
    void onComPortChanged(int index);
    void updateComPorts(const QStringList &ports);
    void saveConfig();
    void loadConfig();
    void onEditButtonClicked();
    void onLineEditReturnPressed();

private:
    void setupUi();
    void setupConnections();

    ElaComboBox *comComboBox;
    ElaComboBox *baudRateComboBox;
    ElaComboBox *dataBitsComboBox;
    ElaComboBox *stopBitsComboBox;
    ElaComboBox *parityComboBox;

    ElaPushButton *okButton;
    ElaPushButton *cancelButton;

    QThread serialScanThread;
    GetSerialPortTask *serialPortTask;
};

#endif  // T_SWITCHCONFIG_H