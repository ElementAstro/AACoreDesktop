#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QUrl>
#include <QTimer>
#include <QQueue>

class WebSocketClient : public QObject
{
    Q_OBJECT

public:
    explicit WebSocketClient(QObject *parent = nullptr);
    ~WebSocketClient();

    void connectToServer(const QUrl &url);
    void sendTextMessage(const QString &message);
    void sendBinaryMessage(const QByteArray &message);
    void closeConnection();
    bool isConnected() const;

    // New methods
    void setReconnectInterval(int msecs);
    void setHeartbeatInterval(int msecs);
    void enableSslCertificateVerification(bool enable);

signals:
    void connected();
    void disconnected();
    void textMessageReceived(const QString &message);
    void binaryMessageReceived(const QByteArray &message);
    void errorOccurred(const QString &errorString);

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

    // New members
    QTimer m_reconnectTimer;
    QTimer m_heartbeatTimer;
    QQueue<QPair<bool, QByteArray>> m_messageQueue; // bool: true for text, false for binary
    bool m_sslVerificationEnabled;

    void scheduleReconnect();
    void clearMessageQueue();
};

#endif // WEBSOCKETCLIENT_H