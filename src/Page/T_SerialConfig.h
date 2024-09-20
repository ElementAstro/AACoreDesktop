#ifndef T_SERIAL_CONFIG_H
#define T_SERIAL_CONFIG_H

#include "T_BasePage.h"

class ElaComboBox;

class GetSerialPortTask : public QObject {
    Q_OBJECT
public slots:
    void run();

signals:
    void scanFinished(const QStringList &ports);
};

class T_SerialConfig : public T_BasePage {
    Q_OBJECT

public:
    T_SerialConfig(QWidget *parent = nullptr);
    ~T_SerialConfig();

private slots:
    void onOkClicked();
    void onCancelClicked();
    void onComPortChanged(int index);
    void updateComPorts(const QStringList &ports);

private:
    ElaComboBox *comComboBox;
};
#endif  // T_SERIAL_CONFIG_H
