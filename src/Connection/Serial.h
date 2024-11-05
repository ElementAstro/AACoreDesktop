#ifndef SERIALCOMMUNICATOR_H
#define SERIALCOMMUNICATOR_H

#include <QByteArray>
#include <QCryptographicHash>
#include <QDataStream>
#include <QElapsedTimer>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QObject>
#include <QQueue>
#include <QTimer>
#include <QtSerialPort/QSerialPort>
#include <QMutex>
#include <QMutexLocker>
#include <QSettings>

Q_DECLARE_LOGGING_CATEGORY(serialComm)

class SerialCommunicator : public QObject {
    Q_OBJECT

public:
    enum LogLevel {
        Error,
        Warning,
        Info,
        Debug
    };

    explicit SerialCommunicator(QObject *parent = nullptr);
    ~SerialCommunicator();

    // 串口操作
    bool openPort(const QString &portName, int baudRate);
    void closePort();
    bool isPortOpen() const;
    void sendData(const QByteArray &data);
    void sendJsonObject(const QJsonObject &jsonObject);
    void sendXmlData(const QString &xmlString);
    void sendCsvData(const QString &csvString);

    // 配置
    QStringList availablePorts() const;
    void loadConfiguration(const QString &configFilePath);
    void saveConfiguration(const QString &configFilePath) const;

    // 设置参数
    void setReadTimeout(int msecs);
    void setWriteTimeout(int msecs);
    void setFlowControl(QSerialPort::FlowControl flowControl);
    void setParity(QSerialPort::Parity parity);
    void setDataBits(QSerialPort::DataBits dataBits);
    void setStopBits(QSerialPort::StopBits stopBits);
    void enableDTR(bool enable);
    void enableRTS(bool enable);

    // 模式
    void setJsonMode(bool enabled);
    bool isJsonMode() const;
    void setXmlMode(bool enabled);
    bool isXmlMode() const;
    void setCsvMode(bool enabled);
    bool isCsvMode() const;

    // 加密与压缩
    void setEncryptionKey(const QByteArray &key);
    void setCompressionEnabled(bool enabled);
    void setAutoReconnectEnabled(bool enabled);
    void setMaxPacketSize(int size);

    // 日志
    void setLogFile(const QString &filePath);
    void setLogLevel(LogLevel level);

public slots:
    void startPeriodicTransmission(const QByteArray &data, int intervalMs);
    void stopPeriodicTransmission();

signals:
    void dataReceived(const QByteArray &data);
    void jsonReceived(const QJsonObject &jsonObject);
    void xmlReceived(const QString &xmlString);
    void csvReceived(const QString &csvString);
    void errorOccurred(const QString &error);
    void portOpened();
    void portClosed();
    void dataSent(qint64 bytes);
    void reconnecting();
    void reconnected();
    void jsonObjectSent(const QJsonObject &jsonObject);

private slots:
    void handleReadyRead();
    void handleError(QSerialPort::SerialPortError error);
    void handleBytesWritten(qint64 bytes);
    void handleReadTimeout();
    void handleWriteTimeout();
    void transmitPeriodicData();
    void attemptReconnect();

private:
    QSerialPort *m_serialPort;
    QTimer *m_readTimeoutTimer;
    QTimer *m_writeTimeoutTimer;
    QTimer *m_periodicTransmitTimer;
    QTimer *m_reconnectTimer;
    QQueue<QByteArray> m_writeQueue;
    bool m_jsonMode;
    bool m_xmlMode;
    bool m_csvMode;
    QByteArray m_buffer;
    QByteArray m_encryptionKey;
    bool m_compressionEnabled;
    bool m_autoReconnectEnabled;
    int m_maxPacketSize;
    QFile m_logFile;
    QString m_currentPortName;
    int m_currentBaudRate;
    QElapsedTimer m_elapsedTimer;
    LogLevel m_logLevel;
    QMutex m_mutex;

    void processWriteQueue();
    void processReceivedData(const QByteArray &data);
    bool isValidJson(const QByteArray &data);
    bool isValidXml(const QByteArray &data);
    bool isValidCsv(const QByteArray &data);
    QByteArray encryptData(const QByteArray &data);
    QByteArray decryptData(const QByteArray &data);
    QByteArray compressData(const QByteArray &data);
    QByteArray decompressData(const QByteArray &data);
    void logMessage(const QString &message, LogLevel level);
    QList<QByteArray> splitPacket(const QByteArray &data);
    QByteArray reassemblePacket(const QList<QByteArray> &packets);
    quint32 calculateCRC32(const QByteArray &data);
};

#endif  // SERIALCOMMUNICATOR_H