#ifndef SERIALPORTSCANNER_H
#define SERIALPORTSCANNER_H

#include <QList>
#include <QObject>
#include <QString>
#include <QtSerialPort/QSerialPort>

class SerialPortScanner : public QObject {
    Q_OBJECT

public:
    explicit SerialPortScanner(QObject *parent = nullptr);

    struct PortInfo {
        QString portName;
        QString description;
        QString manufacturer;
        QString serialNumber;
        qint32 vendorId;
        qint32 productId;
    };

    QList<PortInfo> scanPorts();
    static QString getBaudRateString(qint32 baudRate);
    static QString getDataBitsString(int dataBits);
    static QString getParityString(int parity);
    static QString getStopBitsString(int stopBits);
    static QString getFlowControlString(int flowControl);
};

#endif  // SERIALPORTSCANNER_H