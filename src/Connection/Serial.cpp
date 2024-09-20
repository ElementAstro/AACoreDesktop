#include "Serial.h"
#include <QCryptographicHash>
#include <QDateTime>
#include <QJsonParseError>
#include <QtSerialPort/QSerialPortInfo>

Q_LOGGING_CATEGORY(serialComm, "serial.communicator")

SerialCommunicator::SerialCommunicator(QObject *parent)
    : QObject(parent),
      m_serialPort(new QSerialPort(this)),
      m_readTimeoutTimer(new QTimer(this)),
      m_writeTimeoutTimer(new QTimer(this)),
      m_periodicTransmitTimer(new QTimer(this)),
      m_reconnectTimer(new QTimer(this)),
      m_jsonMode(false),
      m_compressionEnabled(false),
      m_autoReconnectEnabled(false),
      m_maxPacketSize(1024) {
    connect(m_serialPort, &QSerialPort::readyRead, this,
            &SerialCommunicator::handleReadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred, this,
            &SerialCommunicator::handleError);
    connect(m_serialPort, &QSerialPort::bytesWritten, this,
            &SerialCommunicator::handleBytesWritten);

    m_readTimeoutTimer->setSingleShot(true);
    connect(m_readTimeoutTimer, &QTimer::timeout, this,
            &SerialCommunicator::handleReadTimeout);

    m_writeTimeoutTimer->setSingleShot(true);
    connect(m_writeTimeoutTimer, &QTimer::timeout, this,
            &SerialCommunicator::handleWriteTimeout);

    connect(m_periodicTransmitTimer, &QTimer::timeout, this,
            &SerialCommunicator::transmitPeriodicData);

    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer, &QTimer::timeout, this,
            &SerialCommunicator::attemptReconnect);

    m_elapsedTimer.start();
}

SerialCommunicator::~SerialCommunicator() {
    closePort();
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }
}

bool SerialCommunicator::openPort(const QString &portName, int baudRate) {
    if (m_serialPort->isOpen()) {
        closePort();
    }

    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(baudRate);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (m_serialPort->open(QIODevice::ReadWrite)) {
        m_currentPortName = portName;
        m_currentBaudRate = baudRate;
        emit portOpened();
        logMessage(
            QString("Port opened: %1 at %2 baud").arg(portName).arg(baudRate));
        return true;
    }

    logMessage(
        QString("Failed to open port: %1").arg(m_serialPort->errorString()));
    return false;
}

void SerialCommunicator::closePort() {
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
        emit portClosed();
        logMessage("Port closed");
    }
}

bool SerialCommunicator::isPortOpen() const { return m_serialPort->isOpen(); }

void SerialCommunicator::sendData(const QByteArray &data) {
    QByteArray processedData = data;

    if (m_compressionEnabled) {
        processedData = compressData(processedData);
    }

    if (!m_encryptionKey.isEmpty()) {
        processedData = encryptData(processedData);
    }

    QList<QByteArray> packets = splitPacket(processedData);
    for (const QByteArray &packet : packets) {
        m_writeQueue.enqueue(packet);
    }

    processWriteQueue();
    logMessage(QString("Sent data: %1").arg(QString(data)));
}

QStringList SerialCommunicator::availablePorts() const {
    QStringList ports;
    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &portInfo : serialPortInfos) {
        ports << portInfo.portName();
    }
    return ports;
}

void SerialCommunicator::setReadTimeout(int msecs) {
    m_readTimeoutTimer->setInterval(msecs);
}

void SerialCommunicator::setWriteTimeout(int msecs) {
    m_writeTimeoutTimer->setInterval(msecs);
}

void SerialCommunicator::setFlowControl(QSerialPort::FlowControl flowControl) {
    m_serialPort->setFlowControl(flowControl);
}

void SerialCommunicator::setParity(QSerialPort::Parity parity) {
    m_serialPort->setParity(parity);
}

void SerialCommunicator::setDataBits(QSerialPort::DataBits dataBits) {
    m_serialPort->setDataBits(dataBits);
}

void SerialCommunicator::setStopBits(QSerialPort::StopBits stopBits) {
    m_serialPort->setStopBits(stopBits);
}

void SerialCommunicator::enableDTR(bool enable) {
    m_serialPort->setDataTerminalReady(enable);
}

void SerialCommunicator::enableRTS(bool enable) {
    m_serialPort->setRequestToSend(enable);
}

void SerialCommunicator::sendJsonObject(const QJsonObject &jsonObject) {
    QJsonDocument doc(jsonObject);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    sendData(jsonData);
    emit jsonSent(jsonObject);
}

void SerialCommunicator::setJsonMode(bool enabled) {
    m_jsonMode = enabled;
    if (!enabled) {
        m_buffer.clear();
    }
}

bool SerialCommunicator::isJsonMode() const { return m_jsonMode; }

void SerialCommunicator::setEncryptionKey(const QByteArray &key) {
    m_encryptionKey = QCryptographicHash::hash(key, QCryptographicHash::Sha256);
}

void SerialCommunicator::setCompressionEnabled(bool enabled) {
    m_compressionEnabled = enabled;
}

void SerialCommunicator::setAutoReconnectEnabled(bool enabled) {
    m_autoReconnectEnabled = enabled;
}

void SerialCommunicator::setMaxPacketSize(int size) { m_maxPacketSize = size; }

void SerialCommunicator::setLogFile(const QString &filePath) {
    m_logFile.setFileName(filePath);
    if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        qCWarning(serialComm) << "Failed to open log file:" << filePath;
    }
}

void SerialCommunicator::startPeriodicTransmission(const QByteArray &data,
                                                   int intervalMs) {
    m_periodicTransmitTimer->setInterval(intervalMs);
    m_periodicTransmitTimer->start();
}

void SerialCommunicator::stopPeriodicTransmission() {
    m_periodicTransmitTimer->stop();
}

void SerialCommunicator::handleReadyRead() {
    m_readTimeoutTimer->stop();
    QByteArray data = m_serialPort->readAll();

    if (!m_encryptionKey.isEmpty()) {
        data = decryptData(data);
    }

    if (m_compressionEnabled) {
        data = decompressData(data);
    }

    if (m_jsonMode) {
        processReceivedData(data);
    } else {
        emit dataReceived(data);
    }

    logMessage(QString("Received data: %1").arg(QString(data)));
}

void SerialCommunicator::handleError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::ResourceError) {
        closePort();
        if (m_autoReconnectEnabled) {
            m_reconnectTimer->start(1000);  // Try to reconnect in 1 second
            emit reconnecting();
        }
    }
    emit errorOccurred(m_serialPort->errorString());
    logMessage(QString("Error occurred: %1").arg(m_serialPort->errorString()));
}

void SerialCommunicator::handleBytesWritten(qint64 bytes) {
    m_writeTimeoutTimer->stop();
    emit dataSent(bytes);
    processWriteQueue();
}

void SerialCommunicator::handleReadTimeout() {
    emit errorOccurred("Read operation timed out");
    logMessage("Read operation timed out");
}

void SerialCommunicator::handleWriteTimeout() {
    emit errorOccurred("Write operation timed out");
    logMessage("Write operation timed out");
    m_writeQueue.clear();
}

void SerialCommunicator::transmitPeriodicData() {
    // Implement your periodic data transmission logic here
    // For example:
    // QByteArray periodicData = "Periodic message";
    // sendData(periodicData);
}

void SerialCommunicator::attemptReconnect() {
    if (openPort(m_currentPortName, m_currentBaudRate)) {
        emit reconnected();
        logMessage("Successfully reconnected");
    } else {
        m_reconnectTimer->start(5000);  // Try again in 5 seconds
        emit reconnecting();
        logMessage("Reconnection attempt failed, trying again in 5 seconds");
    }
}

void SerialCommunicator::processWriteQueue() {
    if (m_writeQueue.isEmpty() || !m_serialPort->isOpen()) {
        return;
    }

    if (m_serialPort->bytesToWrite() == 0) {
        QByteArray data = m_writeQueue.dequeue();
        m_serialPort->write(data);
        m_writeTimeoutTimer->start();
    }
}

void SerialCommunicator::processReceivedData(const QByteArray &data) {
    m_buffer.append(data);

    while (!m_buffer.isEmpty()) {
        int openBraceIndex = m_buffer.indexOf('{');
        if (openBraceIndex == -1) {
            m_buffer.clear();
            break;
        }

        int closeBraceIndex = m_buffer.lastIndexOf('}');
        if (closeBraceIndex == -1) {
            break;
        }

        QByteArray jsonData =
            m_buffer.mid(openBraceIndex, closeBraceIndex - openBraceIndex + 1);
        if (isValidJson(jsonData)) {
            QJsonDocument doc = QJsonDocument::fromJson(jsonData);
            emit jsonReceived(doc.object());
            m_buffer.remove(0, closeBraceIndex + 1);
        } else {
            m_buffer.remove(0, openBraceIndex + 1);
        }
    }
}

bool SerialCommunicator::isValidJson(const QByteArray &data) {
    QJsonParseError parseError;
    QJsonDocument::fromJson(data, &parseError);
    return parseError.error == QJsonParseError::NoError;
}

QByteArray SerialCommunicator::encryptData(const QByteArray &data) {
    // Simple XOR encryption for demonstration. In a real-world scenario, use a
    // robust encryption library.
    QByteArray encrypted;
    for (int i = 0; i < data.size(); ++i) {
        encrypted.append(data.at(i) ^
                         m_encryptionKey.at(i % m_encryptionKey.size()));
    }
    return encrypted;
}

QByteArray SerialCommunicator::decryptData(const QByteArray &data) {
    // XOR decryption (same as encryption for XOR)
    return encryptData(data);
}

QByteArray SerialCommunicator::compressData(const QByteArray &data) {
    return qCompress(data);
}

QByteArray SerialCommunicator::decompressData(const QByteArray &data) {
    return qUncompress(data);
}

void SerialCommunicator::logMessage(const QString &message) {
    if (m_logFile.isOpen()) {
        QTextStream out(&m_logFile);
        out << QDateTime::currentDateTime().toString(Qt::ISODate) << " - "
            << message << "\n";
    }
    qCDebug(serialComm) << message;
}

QList<QByteArray> SerialCommunicator::splitPacket(const QByteArray &data) {
    QList<QByteArray> packets;
    for (int i = 0; i < data.size(); i += m_maxPacketSize) {
        packets.append(data.mid(i, m_maxPacketSize));
    }
    return packets;
}

QByteArray SerialCommunicator::reassemblePacket(
    const QList<QByteArray> &packets) {
    QByteArray reassembled;
    for (const QByteArray &packet : packets) {
        reassembled.append(packet);
    }
    return reassembled;
}
