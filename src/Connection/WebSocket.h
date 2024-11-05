#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QMap>
#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QSslConfiguration>
#include <QTimer>
#include <QUrl>
#include <QtWebSockets/QWebSocket>


class WebSocketClient : public QObject {
    Q_OBJECT

public:
    enum LogLevel { Debug, Info, Warning, Error };

    explicit WebSocketClient(QObject *parent = nullptr);
    ~WebSocketClient();

    void connectToServer(
        const QUrl &url,
        const QMap<QString, QString> &headers = QMap<QString, QString>());
    void sendTextMessage(const QString &message);
    void sendBinaryMessage(const QByteArray &message);
    void closeConnection();
    bool isConnected() const;

    // New methods
    void setReconnectInterval(int initialMsecs, int maxMsecs);
    void setHeartbeatInterval(int msecs);
    void enableSslCertificateVerification(bool enable);
    void setLogLevel(LogLevel level);
    void setSslConfiguration(const QSslConfiguration &config);

signals:
    void connected();
    void disconnected();
    void textMessageReceived(const QString &message);
    void binaryMessageReceived(const QByteArray &message);
    void errorOccurred(const QString &errorString);
    void logMessage(LogLevel level, const QString &message);

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);
    void onBinaryMessageReceived(const QByteArray &message);
    void onError(QAbstractSocket::SocketError error);
    void onSslErrors(const QList<QSslError> &errors);

    // New slots
    void onReconnectTimer();
    void sendHeartbeat();
    void processMessageQueue();

private:
    QWebSocket *m_webSocket;
    bool m_isConnected;
    QUrl m_serverUrl;
    QMap<QString, QString> m_customHeaders;

    // New members
    QTimer m_reconnectTimer;
    int m_initialReconnectInterval;
    int m_maxReconnectInterval;
    QTimer m_heartbeatTimer;
    QQueue<QPair<bool, QByteArray>> m_messageQueue;
    bool m_sslVerificationEnabled;
    LogLevel m_logLevel;
    QMutex m_mutex;
    QSslConfiguration m_sslConfig;

    void scheduleReconnect();
    void clearMessageQueue();
    void log(LogLevel level, const QString &message);
    void applySslConfiguration();
};

#endif  // WEBSOCKETCLIENT_H