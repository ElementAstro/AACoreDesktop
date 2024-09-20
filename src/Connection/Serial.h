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

Q_DECLARE_LOGGING_CATEGORY(serialComm)

class SerialCommunicator : public QObject {
    Q_OBJECT

public:
    explicit SerialCommunicator(QObject *parent = nullptr);
    ~SerialCommunicator();

    bool openPort(const QString &portName, int baudRate);
    void closePort();
    bool isPortOpen() const;
    void sendData(const QByteArray &data);

    QStringList availablePorts() const;
    void setReadTimeout(int msecs);
    void setWriteTimeout(int msecs);
    void setFlowControl(QSerialPort::FlowControl flowControl);
    void setParity(QSerialPort::Parity parity);
    void setDataBits(QSerialPort::DataBits dataBits);
    void setStopBits(QSerialPort::StopBits stopBits);
    void enableDTR(bool enable);
    void enableRTS(bool enable);

    void sendJsonObject(const QJsonObject &jsonObject);
    void setJsonMode(bool enabled);
    bool isJsonMode() const;

    void setEncryptionKey(const QByteArray &key);
    void setCompressionEnabled(bool enabled);
    void setAutoReconnectEnabled(bool enabled);
    void setMaxPacketSize(int size);
    void setLogFile(const QString &filePath);

public slots:
    void startPeriodicTransmission(const QByteArray &data, int intervalMs);
    void stopPeriodicTransmission();

signals:
    void dataReceived(const QByteArray &data);
    void jsonReceived(const QJsonObject &jsonObject);
    void errorOccurred(const QString &error);
    void portOpened();
    void portClosed();
    void dataSent(qint64 bytes);
    void jsonSent(const QJsonObject &jsonObject);
    void reconnecting();
    void reconnected();

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
    QByteArray m_buffer;
    QByteArray m_encryptionKey;
    bool m_compressionEnabled;
    bool m_autoReconnectEnabled;
    int m_maxPacketSize;
    QFile m_logFile;
    QString m_currentPortName;
    int m_currentBaudRate;
    QElapsedTimer m_elapsedTimer;

    void processWriteQueue();
    void processReceivedData(const QByteArray &data);
    bool isValidJson(const QByteArray &data);
    QByteArray encryptData(const QByteArray &data);
    QByteArray decryptData(const QByteArray &data);
    QByteArray compressData(const QByteArray &data);
    QByteArray decompressData(const QByteArray &data);
    void logMessage(const QString &message);
    QList<QByteArray> splitPacket(const QByteArray &data);
    QByteArray reassemblePacket(const QList<QByteArray> &packets);
};

#endif  // SERIALCOMMUNICATOR_H
