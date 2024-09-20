#ifndef T_SERIALDEBUGPAGE_H
#define T_SERIALDEBUGPAGE_H

#include "T_BasePage.h"

#include <QDateTime>
#include <QFile>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QWidget>

class ElaCheckBox;
class ElaPushButton;
class ElaComboBox;
class ElaLineEdit;
class ElaText;
class ElaPlainTextEdit;

class T_SerialDebugPage : public T_BasePage {
    Q_OBJECT

public:
    T_SerialDebugPage(QWidget *parent = nullptr);
    ~T_SerialDebugPage();

private slots:
    void on_openSerialButton_clicked();
    void on_sendButton_clicked();
    void readSerialData();
    void updateSerialPorts();
    void on_clearScreenButton_clicked();
    void on_saveLogButton_clicked();

private:
    void setupUI();

    QSerialPort *serial;
    QFile logFile;
    quint64 receivedBytes = 0;
    quint64 sentBytes = 0;

    // GUI 控件
    ElaPushButton *openSerialButton;
    ElaPushButton *sendButton;
    ElaPushButton *clearScreenButton;
    ElaPushButton *saveLogButton;
    ElaPushButton *refreshSerialButton;

    ElaComboBox *serialPortComboBox;
    ElaComboBox *baudRateComboBox;
    ElaComboBox *dataBitsComboBox;
    ElaComboBox *stopBitsComboBox;
    ElaComboBox *parityComboBox;
    ElaComboBox *flowControlComboBox;

    ElaLineEdit *sendLineEdit;
    ElaPlainTextEdit *receiveTextEdit;

    ElaText *rxLabel;
    ElaText *txLabel;

    ElaCheckBox *timestampCheckBox;
};

#endif  // T_SERIALDEBUGPAGE_H
