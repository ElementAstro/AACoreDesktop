#include "SerialScanner.h"

#include <QtSerialPort/QSerialPortInfo>

SerialPortScanner::SerialPortScanner(QObject *parent) : QObject(parent) {}

QList<SerialPortScanner::PortInfo> SerialPortScanner::scanPorts() {
    QList<PortInfo> portInfoList;
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port : ports) {
        PortInfo info;
        info.portName = port.portName();
        info.description = port.description();
        info.manufacturer = port.manufacturer();
        info.serialNumber = port.serialNumber();
        info.vendorId = port.vendorIdentifier();
        info.productId = port.productIdentifier();
        portInfoList.append(info);
    }
    return portInfoList;
}

QString SerialPortScanner::getBaudRateString(qint32 baudRate) {
    switch (baudRate) {
        case QSerialPort::Baud1200:
            return "1200";
        case QSerialPort::Baud2400:
            return "2400";
        case QSerialPort::Baud4800:
            return "4800";
        case QSerialPort::Baud9600:
            return "9600";
        case QSerialPort::Baud19200:
            return "19200";
        case QSerialPort::Baud38400:
            return "38400";
        case QSerialPort::Baud57600:
            return "57600";
        case QSerialPort::Baud115200:
            return "115200";
        default:
            return "Custom";
    }
}

QString SerialPortScanner::getDataBitsString(int dataBits) {
    switch (dataBits) {
        case QSerialPort::Data5:
            return "5";
        case QSerialPort::Data6:
            return "6";
        case QSerialPort::Data7:
            return "7";
        case QSerialPort::Data8:
            return "8";
        default:
            return "Unknown";
    }
}

QString SerialPortScanner::getParityString(int parity) {
    switch (parity) {
        case QSerialPort::NoParity:
            return "None";
        case QSerialPort::EvenParity:
            return "Even";
        case QSerialPort::OddParity:
            return "Odd";
        case QSerialPort::SpaceParity:
            return "Space";
        case QSerialPort::MarkParity:
            return "Mark";
        default:
            return "Unknown";
    }
}

QString SerialPortScanner::getStopBitsString(int stopBits) {
    switch (stopBits) {
        case QSerialPort::OneStop:
            return "1";
        case QSerialPort::OneAndHalfStop:
            return "1.5";
        case QSerialPort::TwoStop:
            return "2";
        default:
            return "Unknown";
    }
}

QString SerialPortScanner::getFlowControlString(int flowControl) {
    switch (flowControl) {
        case QSerialPort::NoFlowControl:
            return "None";
        case QSerialPort::HardwareControl:
            return "Hardware";
        case QSerialPort::SoftwareControl:
            return "Software";
        default:
            return "Unknown";
    }
}