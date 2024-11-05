#include "Serial.h"

#include <zlib.h>  // 用于CRC32
#include <QCryptographicHash>
#include <QDateTime>
#include <QJsonParseError>
#include <QTextStream>
#include <QXmlStreamReader>
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
      m_xmlMode(false),
      m_csvMode(false),
      m_compressionEnabled(false),
      m_autoReconnectEnabled(false),
      m_maxPacketSize(1024),
      m_logLevel(Info),
      m_currentBaudRate(0) {  // 初始化 m_currentBaudRate

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
    QMutexLocker locker(&m_mutex);

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
            QString("Port opened: %1 at %2 baud").arg(portName).arg(baudRate),
            Info);
        return true;
    }

    logMessage(
        QString("Failed to open port: %1").arg(m_serialPort->errorString()),
        Error);
    return false;
}

void SerialCommunicator::closePort() {
    QMutexLocker locker(&m_mutex);
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
        emit portClosed();
        logMessage("Port closed", Info);
    }
}

bool SerialCommunicator::isPortOpen() const { return m_serialPort->isOpen(); }

void SerialCommunicator::sendData(const QByteArray &data) {
    QMutexLocker locker(&m_mutex);
    QByteArray processedData = data;

    if (m_compressionEnabled) {
        processedData = compressData(processedData);
    }

    if (!m_encryptionKey.isEmpty()) {
        processedData = encryptData(processedData);
    }

    // 添加CRC32校验
    quint32 crc = calculateCRC32(processedData);
    QByteArray crcData;
    QDataStream stream(&crcData, QIODevice::WriteOnly);
    stream << crc;
    processedData.append(crcData);

    QList<QByteArray> packets = splitPacket(processedData);
    for (const QByteArray &packet : packets) {
        m_writeQueue.enqueue(packet);
    }

    processWriteQueue();
    logMessage(QString("Sent data: %1").arg(QString(data)), Debug);
}

void SerialCommunicator::sendJsonObject(const QJsonObject &jsonObject) {
    QJsonDocument doc(jsonObject);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    sendData(jsonData);
    emit jsonObjectSent(jsonObject);  // 修复：将信号名称更改为已声明的信号
}

void SerialCommunicator::sendXmlData(const QString &xmlString) {
    QByteArray xmlData = xmlString.toUtf8();
    sendData(xmlData);
    emit xmlReceived(xmlString);
}

void SerialCommunicator::sendCsvData(const QString &csvString) {
    QByteArray csvData = csvString.toUtf8();
    sendData(csvData);
    emit csvReceived(csvString);
}

QStringList SerialCommunicator::availablePorts() const {
    QStringList ports;
    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &portInfo : serialPortInfos) {
        ports << portInfo.portName();
    }
    return ports;
}

void SerialCommunicator::loadConfiguration(const QString &configFilePath) {
    QSettings settings(configFilePath,
                       QSettings::IniFormat);  // 修复：使用正确的格式
    settings.beginGroup("SerialCommunicator");
    QString port = settings.value("portName", "").toString();
    int baud = settings.value("baudRate", QSerialPort::Baud9600).toInt();
    setFlowControl(static_cast<QSerialPort::FlowControl>(
        settings.value("flowControl", QSerialPort::NoFlowControl).toInt()));
    setParity(static_cast<QSerialPort::Parity>(
        settings.value("parity", QSerialPort::NoParity).toInt()));
    setDataBits(static_cast<QSerialPort::DataBits>(
        settings.value("dataBits", QSerialPort::Data8).toInt()));
    setStopBits(static_cast<QSerialPort::StopBits>(
        settings.value("stopBits", QSerialPort::OneStop).toInt()));
    enableDTR(settings.value("dtr", false).toBool());
    enableRTS(settings.value("rts", false).toBool());
    settings.endGroup();

    if (!port.isEmpty()) {
        openPort(port, baud);
    }
}

void SerialCommunicator::saveConfiguration(
    const QString &configFilePath) const {
    QSettings settings(configFilePath,
                       QSettings::IniFormat);  // 修复：使用正确的格式
    settings.beginGroup("SerialCommunicator");
    settings.setValue("portName", m_currentPortName);
    settings.setValue("baudRate", m_currentBaudRate);
    settings.setValue("flowControl",
                      static_cast<int>(m_serialPort->flowControl()));
    settings.setValue("parity", static_cast<int>(m_serialPort->parity()));
    settings.setValue("dataBits", static_cast<int>(m_serialPort->dataBits()));
    settings.setValue("stopBits", static_cast<int>(m_serialPort->stopBits()));
    settings.setValue("dtr", m_serialPort->isDataTerminalReady());
    settings.setValue("rts", m_serialPort->isRequestToSend());
    settings.endGroup();
}

void SerialCommunicator::setReadTimeout(int msecs) {
    QMutexLocker locker(&m_mutex);
    m_readTimeoutTimer->setInterval(msecs);
}

void SerialCommunicator::setWriteTimeout(int msecs) {
    QMutexLocker locker(&m_mutex);
    m_writeTimeoutTimer->setInterval(msecs);
}

void SerialCommunicator::setFlowControl(QSerialPort::FlowControl flowControl) {
    QMutexLocker locker(&m_mutex);
    m_serialPort->setFlowControl(flowControl);
}

void SerialCommunicator::setParity(QSerialPort::Parity parity) {
    QMutexLocker locker(&m_mutex);
    m_serialPort->setParity(parity);
}

void SerialCommunicator::setDataBits(QSerialPort::DataBits dataBits) {
    QMutexLocker locker(&m_mutex);
    m_serialPort->setDataBits(dataBits);
}

void SerialCommunicator::setStopBits(QSerialPort::StopBits stopBits) {
    QMutexLocker locker(&m_mutex);
    m_serialPort->setStopBits(stopBits);
}

void SerialCommunicator::enableDTR(bool enable) {
    QMutexLocker locker(&m_mutex);
    m_serialPort->setDataTerminalReady(enable);
}

void SerialCommunicator::enableRTS(bool enable) {
    QMutexLocker locker(&m_mutex);
    m_serialPort->setRequestToSend(enable);
}

void SerialCommunicator::setJsonMode(bool enabled) {
    QMutexLocker locker(&m_mutex);
    m_jsonMode = enabled;
    if (!enabled) {
        m_buffer.clear();
    }
}

bool SerialCommunicator::isJsonMode() const { return m_jsonMode; }

void SerialCommunicator::setXmlMode(bool enabled) {
    QMutexLocker locker(&m_mutex);
    m_xmlMode = enabled;
    if (!enabled) {
        m_buffer.clear();
    }
}

bool SerialCommunicator::isXmlMode() const { return m_xmlMode; }

void SerialCommunicator::setCsvMode(bool enabled) {
    QMutexLocker locker(&m_mutex);
    m_csvMode = enabled;
    if (!enabled) {
        m_buffer.clear();
    }
}

bool SerialCommunicator::isCsvMode() const { return m_csvMode; }

void SerialCommunicator::setEncryptionKey(const QByteArray &key) {
    QMutexLocker locker(&m_mutex);
    // 使用SHA-256哈希生成256位密钥
    m_encryptionKey = QCryptographicHash::hash(key, QCryptographicHash::Sha256);
}

void SerialCommunicator::setCompressionEnabled(bool enabled) {
    QMutexLocker locker(&m_mutex);
    m_compressionEnabled = enabled;
}

void SerialCommunicator::setAutoReconnectEnabled(bool enabled) {
    QMutexLocker locker(&m_mutex);
    m_autoReconnectEnabled = enabled;
}

void SerialCommunicator::setMaxPacketSize(int size) {
    QMutexLocker locker(&m_mutex);
    m_maxPacketSize = size;
}

void SerialCommunicator::setLogFile(const QString &filePath) {
    QMutexLocker locker(&m_mutex);
    m_logFile.setFileName(filePath);
    if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Append |
                        QIODevice::Text)) {
        qCWarning(serialComm) << "Failed to open log file:" << filePath;
    }
}

void SerialCommunicator::setLogLevel(LogLevel level) {
    QMutexLocker locker(&m_mutex);
    m_logLevel = level;
}

void SerialCommunicator::startPeriodicTransmission(const QByteArray &data,
                                                   int intervalMs) {
    QMutexLocker locker(&m_mutex);
    m_periodicTransmitTimer->setInterval(intervalMs);
    m_periodicTransmitTimer->start();
}

void SerialCommunicator::stopPeriodicTransmission() {
    QMutexLocker locker(&m_mutex);
    m_periodicTransmitTimer->stop();
}

void SerialCommunicator::handleReadyRead() {
    QMutexLocker locker(&m_mutex);
    m_readTimeoutTimer->stop();
    QByteArray data = m_serialPort->readAll();

    if (!m_encryptionKey.isEmpty()) {
        data = decryptData(data);
    }

    if (m_compressionEnabled) {
        data = decompressData(data);
    }

    processReceivedData(data);
    logMessage(QString("Received data: %1").arg(QString::fromUtf8(data)),
               Debug);
}

void SerialCommunicator::handleError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::ResourceError) {
        closePort();
        if (m_autoReconnectEnabled) {
            m_reconnectTimer->start(1000);  // 1秒后尝试重连
            emit reconnecting();
        }
    }
    emit errorOccurred(m_serialPort->errorString());
    logMessage(QString("Error occurred: %1").arg(m_serialPort->errorString()),
               Error);
}

void SerialCommunicator::handleBytesWritten(qint64 bytes) {
    QMutexLocker locker(&m_mutex);
    m_writeTimeoutTimer->stop();
    emit dataSent(bytes);
    processWriteQueue();
}

void SerialCommunicator::handleReadTimeout() {
    emit errorOccurred("Read operation timed out");
    logMessage("Read operation timed out", Warning);
}

void SerialCommunicator::handleWriteTimeout() {
    emit errorOccurred("Write operation timed out");
    logMessage("Write operation timed out", Warning);
    m_writeQueue.clear();
}

void SerialCommunicator::transmitPeriodicData() {
    // 示例：周期性发送固定数据
    QByteArray periodicData = "Periodic message";
    sendData(periodicData);
}

void SerialCommunicator::attemptReconnect() {
    QMutexLocker locker(&m_mutex);
    if (openPort(m_currentPortName, m_currentBaudRate)) {
        emit reconnected();
        logMessage("Successfully reconnected", Info);
    } else {
        m_reconnectTimer->start(5000);  // 5秒后再次尝试
        emit reconnecting();
        logMessage("Reconnection attempt failed, trying again in 5 seconds",
                   Warning);
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
        // 检查CRC32
        if (m_buffer.size() < 4) {
            break;  // 不足以包含CRC32
        }

        QByteArray payload = m_buffer.left(m_buffer.size() - 4);
        QByteArray receivedCrcData = m_buffer.mid(m_buffer.size() - 4, 4);
        QDataStream crcStream(receivedCrcData);
        quint32 receivedCrc;
        crcStream >> receivedCrc;

        quint32 calculatedCrc = calculateCRC32(payload);
        if (receivedCrc != calculatedCrc) {
            emit errorOccurred("CRC mismatch detected");
            logMessage("CRC mismatch detected", Warning);
            m_buffer.clear();
            break;
        }

        // 根据模式处理数据
        if (m_jsonMode && isValidJson(payload)) {
            QJsonDocument doc = QJsonDocument::fromJson(payload);
            emit jsonReceived(doc.object());
        } else if (m_xmlMode && isValidXml(payload)) {
            QString xmlString = QString::fromUtf8(payload);
            emit xmlReceived(xmlString);
        } else if (m_csvMode && isValidCsv(payload)) {
            QString csvString = QString::fromUtf8(payload);
            emit csvReceived(csvString);
        } else {
            emit dataReceived(payload);
        }

        m_buffer.clear();
    }
}

bool SerialCommunicator::isValidJson(const QByteArray &data) {
    QJsonParseError parseError;
    QJsonDocument::fromJson(data, &parseError);
    return parseError.error == QJsonParseError::NoError;
}

bool SerialCommunicator::isValidXml(const QByteArray &data) {
    QXmlStreamReader xml(data);
    return xml.readNextStartElement();
}

bool SerialCommunicator::isValidCsv(const QByteArray &data) {
    QString csvString = QString::fromUtf8(data);
    return csvString.contains(',');
}

QByteArray SerialCommunicator::encryptData(const QByteArray &data) {
    // 使用AES加密（示例中使用简单的对称加密）
    // 在实际应用中，应使用成熟的加密库，如 QCA 或 OpenSSL
    QByteArray encrypted = data;
    for (int i = 0; i < encrypted.size(); ++i) {
        encrypted[i] =
            encrypted[i] ^ m_encryptionKey[i % m_encryptionKey.size()];
    }
    return encrypted;
}

QByteArray SerialCommunicator::decryptData(const QByteArray &data) {
    // AES解密（与加密过程相同）
    return encryptData(data);
}

QByteArray SerialCommunicator::compressData(const QByteArray &data) {
    return qCompress(data);  // 修复：直接返回 qCompress 结果
}

QByteArray SerialCommunicator::decompressData(const QByteArray &data) {
    QByteArray decompressed = qUncompress(data);
    if (decompressed.isNull()) {
        logMessage("Decompression failed", Warning);
    }
    return decompressed;
}

void SerialCommunicator::logMessage(const QString &message, LogLevel level) {
    if (level > m_logLevel) {
        return;
    }

    QString logEntry =
        QString("[%1] %2")
            .arg(QDateTime::currentDateTime().toString(Qt::ISODate))
            .arg(message);

    if (m_logFile.isOpen()) {
        QTextStream out(&m_logFile);
        out << logEntry << "\n";
    }

    switch (level) {
        case Error:
            qCritical(serialComm) << message;
            break;
        case Warning:
            qCWarning(serialComm) << message;
            break;
        case Info:
            qCInfo(serialComm) << message;
            break;
        case Debug:
            qCDebug(serialComm) << message;
            break;
    }
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

quint32 SerialCommunicator::calculateCRC32(const QByteArray &data) {
    quint32 crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, reinterpret_cast<const Bytef *>(data.constData()),
                data.size());
    return crc;
}