#ifndef T_SERIALDEBUGPAGE_H
#define T_SERIALDEBUGPAGE_H

#include <QDateTime>
#include <QFile>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QWidget>

#include "T_BasePage.h"

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
    void handleReconnect();

private:
    void setupUI();
    void loadSendHistory();
    void saveSendHistory();
    void keyPressEvent(QKeyEvent *event) override;

    QSerialPort *serial;
    QFile logFile;
    quint64 receivedBytes = 0;
    quint64 sentBytes = 0;

    QStringList sendHistory;
    int currentHistoryIndex = -1;

    // GUI 控件
    ElaPushButton *openSerialButton;
    ElaPushButton *sendButton;
    ElaPushButton *clearScreenButton;
    ElaPushButton *saveLogButton;
    ElaPushButton *refreshSerialButton;
    ElaCheckBox *autoReconnectCheckBox;
    ElaCheckBox *newlineCheckBox;
    ElaCheckBox *timestampCheckBox;

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
    ElaText *statusLabel;
};

#endif  // T_SERIALDEBUGPAGE_H