#ifndef MESSAGEBUS_H
#define MESSAGEBUS_H

#include <QHash>
#include <QObject>
#include <QQueue>
#include <QVariant>
#include <QtSql/QSqlDatabase>

#include "Connection/Http.h"
#include "Connection/Tcp.h"
#include "Connection/WebSocket.h"

class MessageBus : public QObject {
    Q_OBJECT

public:
    enum Protocol { WebSocket, TCP, HTTP };

    enum Priority { Low, Normal, High, Critical };

    struct Message {
        QString channel;
        QVariant data;
        Protocol protocol;
        Priority priority;
        QString messageId;
        bool requiresAck;
    };

    explicit MessageBus(QObject *parent = nullptr);
    ~MessageBus();

    // 连接到不同协议的服务器
    void connectWebSocket(const QString &url);
    void connectTcp(const QString &host, quint16 port);
    void configureHttp(const QString &baseUrl);

    // 发送消息
    void sendMessage(const QString &channel, const QVariant &message,
                     Protocol protocol = WebSocket, Priority priority = Normal,
                     bool requiresAck = false);

    // 订阅和取消订阅频道
    void subscribe(const QString &channel, QObject *receiver,
                   const char *method);
    void unsubscribe(const QString &channel, QObject *receiver,
                     const char *method);

    // 设置自动重连
    void setAutoReconnect(bool enable, Protocol protocol);

    // 新增方法
    void setPersistenceEnabled(bool enable);
    void setMessageFilter(const QString &channel,
                          std::function<bool(const QVariant &)> filter);
    void setRouteRule(const QString &sourceChannel,
                      const QString &targetChannel, Protocol targetProtocol);
    void acknowledgeMessage(const QString &messageId);

signals:
    void messageReceived(const QString &channel, const QVariant &message);
    void connected(Protocol protocol);
    void disconnected(Protocol protocol);
    void error(Protocol protocol, const QString &errorMessage);
    void messageAcknowledged(const QString &messageId);

private slots:
    void onWebSocketConnected();
    void onWebSocketDisconnected();
    void onWebSocketError(QAbstractSocket::SocketError error);
    void onWebSocketMessageReceived(const QString &message);

    void onTcpConnected();
    void onTcpDisconnected();
    void onTcpError(QAbstractSocket::SocketError error);
    void onTcpDataReceived(const QByteArray &data);

    void onHttpRequestFinished(HttpRequest *request, int statusCode,
                               const QByteArray &response);
    void onHttpRequestError(HttpRequest *request, const QString &errorString);

    void processMessageQueue();

private:
    WebSocketClient *m_webSocketClient;
    TcpClient *m_tcpClient;
    HttpRequestCenter *m_httpRequestCenter;

    QHash<QString, QList<QPair<QObject *, const char *>>> m_subscribers;
    QHash<QString, std::function<bool(const QVariant &)>> m_filters;
    QHash<QString, QPair<QString, Protocol>> m_routeRules;

    QQueue<Message> m_messageQueue;
    QTimer m_queueProcessTimer;

    bool m_persistenceEnabled;
    QSqlDatabase m_database;

    void distributeMessage(const Message &message);
    void persistMessage(const Message &message);
    void loadPersistedMessages();
    QString generateMessageId();
    void enqueueMessage(const Message &message);
    QJsonValue variantToJson(const QVariant &val);
    QVariant jsonToVariant(const QJsonValue &val);
};

#endif  // MESSAGEBUS_H