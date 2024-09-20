#include "MessageBus.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUuid>
#include <QtSql/QSqlQuery>


MessageBus::MessageBus(QObject *parent)
    : QObject(parent),
      m_webSocketClient(new WebSocketClient(this)),
      m_tcpClient(new TcpClient(this)),
      m_httpRequestCenter(new HttpRequestCenter(this)),
      m_persistenceEnabled(false) {
    connect(m_webSocketClient, &WebSocketClient::connected, this,
            &MessageBus::onWebSocketConnected);
    connect(m_webSocketClient, &WebSocketClient::disconnected, this,
            &MessageBus::onWebSocketDisconnected);
    // connect(m_webSocketClient, &WebSocketClient::error, this,
    //         &MessageBus::onWebSocketError);
    connect(m_webSocketClient, &WebSocketClient::textMessageReceived, this,
            &MessageBus::onWebSocketMessageReceived);

    connect(m_tcpClient, &TcpClient::connected, this,
            &MessageBus::onTcpConnected);
    connect(m_tcpClient, &TcpClient::disconnected, this,
            &MessageBus::onTcpDisconnected);
    connect(m_tcpClient, &TcpClient::error, this, &MessageBus::onTcpError);
    connect(m_tcpClient, &TcpClient::dataReceived, this,
            &MessageBus::onTcpDataReceived);

    connect(m_httpRequestCenter, &HttpRequestCenter::requestFinished, this,
            &MessageBus::onHttpRequestFinished);
    connect(m_httpRequestCenter, &HttpRequestCenter::requestError, this,
            &MessageBus::onHttpRequestError);

    m_queueProcessTimer.setInterval(100);  // Process queue every 100ms
    connect(&m_queueProcessTimer, &QTimer::timeout, this,
            &MessageBus::processMessageQueue);
    m_queueProcessTimer.start();
}

MessageBus::~MessageBus() {
    if (m_persistenceEnabled) {
        m_database.close();
    }
}

void MessageBus::connectWebSocket(const QString &url) {
    m_webSocketClient->connectToServer(QUrl(url));
}

void MessageBus::connectTcp(const QString &host, quint16 port) {
    m_tcpClient->connectToHost(host, port);
}

void MessageBus::configureHttp(const QString &baseUrl) {
    // Configure HTTP client base URL
}

void MessageBus::sendMessage(const QString &channel, const QVariant &message,
                             Protocol protocol, Priority priority,
                             bool requiresAck) {
    Message msg;
    msg.channel = channel;
    msg.data = message;
    msg.protocol = protocol;
    msg.priority = priority;
    msg.messageId = generateMessageId();
    msg.requiresAck = requiresAck;

    enqueueMessage(msg);
}

void MessageBus::subscribe(const QString &channel, QObject *receiver,
                           const char *method) {
    m_subscribers[channel].append(qMakePair(receiver, method));
}

void MessageBus::unsubscribe(const QString &channel, QObject *receiver,
                             const char *method) {
    if (m_subscribers.contains(channel)) {
        auto &list = m_subscribers[channel];
        list.removeAll(qMakePair(receiver, method));
        if (list.isEmpty()) {
            m_subscribers.remove(channel);
        }
    }
}

void MessageBus::setAutoReconnect(bool enable, Protocol protocol) {
    switch (protocol) {
        case WebSocket:
            // m_webSocketClient->setAutoReconnect(enable);
            break;
        case TCP:
            m_tcpClient->setAutoReconnect(enable);
            break;
        case HTTP:
            // HTTP typically doesn't need persistent connections
            break;
    }
}

void MessageBus::setPersistenceEnabled(bool enable) {
    m_persistenceEnabled = enable;
    if (enable) {
        m_database = QSqlDatabase::addDatabase("QSQLITE");
        m_database.setDatabaseName("messagebus.db");
        if (m_database.open()) {
            QSqlQuery query;
            query.exec(
                "CREATE TABLE IF NOT EXISTS messages "
                "(id TEXT PRIMARY KEY, channel TEXT, data BLOB, protocol "
                "INTEGER, priority INTEGER, requires_ack INTEGER)");
        }
        loadPersistedMessages();
    } else {
        m_database.close();
    }
}

void MessageBus::setMessageFilter(
    const QString &channel, std::function<bool(const QVariant &)> filter) {
    m_filters[channel] = filter;
}

void MessageBus::setRouteRule(const QString &sourceChannel,
                              const QString &targetChannel,
                              Protocol targetProtocol) {
    m_routeRules[sourceChannel] = qMakePair(targetChannel, targetProtocol);
}

void MessageBus::acknowledgeMessage(const QString &messageId) {
    // Remove the message from the queue if it's still there
    m_messageQueue.removeIf(
        [messageId](const Message &msg) { return msg.messageId == messageId; });

    // Remove the message from the database if persistence is enabled
    if (m_persistenceEnabled) {
        QSqlQuery query;
        query.prepare("DELETE FROM messages WHERE id = :id");
        query.bindValue(":id", messageId);
        query.exec();
    }

    emit messageAcknowledged(messageId);
}

void MessageBus::onWebSocketConnected() { emit connected(WebSocket); }

void MessageBus::onWebSocketDisconnected() { emit disconnected(WebSocket); }

void MessageBus::onWebSocketError(QAbstractSocket::SocketError error) {
    // emit this->error(WebSocket, m_webSocketClient->errorString());
}

void MessageBus::onWebSocketMessageReceived(const QString &message) {
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        Message msg;
        msg.channel = obj["channel"].toString();
        msg.data = obj["data"].toVariant();
        msg.protocol = WebSocket;
        msg.priority = static_cast<Priority>(obj["priority"].toInt());
        msg.messageId = obj["messageId"].toString();
        msg.requiresAck = obj["requiresAck"].toBool();
        distributeMessage(msg);
    }
}

void MessageBus::onTcpConnected() { emit connected(TCP); }

void MessageBus::onTcpDisconnected() { emit disconnected(TCP); }

void MessageBus::onTcpError(QAbstractSocket::SocketError error) {
    // emit this->error(TCP, m_tcpClient->errorString());
}

void MessageBus::onTcpDataReceived(const QByteArray &data) {
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        Message msg;
        msg.channel = obj["channel"].toString();
        msg.data = obj["data"].toVariant();
        msg.protocol = TCP;
        msg.priority = static_cast<Priority>(obj["priority"].toInt());
        msg.messageId = obj["messageId"].toString();
        msg.requiresAck = obj["requiresAck"].toBool();
        distributeMessage(msg);
    }
}

void MessageBus::onHttpRequestFinished(HttpRequest *request, int statusCode,
                                       const QByteArray &response) {
    QJsonDocument doc = QJsonDocument::fromJson(response);
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        Message msg;
        msg.channel = obj["channel"].toString();
        msg.data = obj["data"].toVariant();
        msg.protocol = HTTP;
        msg.priority = static_cast<Priority>(obj["priority"].toInt());
        msg.messageId = obj["messageId"].toString();
        msg.requiresAck = obj["requiresAck"].toBool();
        distributeMessage(msg);
    }
}

void MessageBus::onHttpRequestError(HttpRequest *request,
                                    const QString &errorString) {
    emit error(HTTP, errorString);
}

void MessageBus::distributeMessage(const Message &message) {
    // Apply filter if exists
    if (m_filters.contains(message.channel)) {
        if (!m_filters[message.channel](message.data)) {
            return;  // Message filtered out
        }
    }

    // Check for routing rules
    if (m_routeRules.contains(message.channel)) {
        const auto &rule = m_routeRules[message.channel];
        sendMessage(rule.first, message.data, rule.second, message.priority,
                    message.requiresAck);
        return;
    }

    emit messageReceived(message.channel, message.data);

    if (m_subscribers.contains(message.channel)) {
        for (const auto &subscriber : m_subscribers[message.channel]) {
            QMetaObject::invokeMethod(subscriber.first, subscriber.second,
                                      Q_ARG(QVariant, message.data));
        }
    }

    if (message.requiresAck) {
        acknowledgeMessage(message.messageId);
    }
}

void MessageBus::persistMessage(const Message &message) {
    if (!m_persistenceEnabled)
        return;

    QSqlQuery query;
    query.prepare(
        "INSERT INTO messages (id, channel, data, protocol, priority, "
        "requires_ack) "
        "VALUES (:id, :channel, :data, :protocol, :priority, :requires_ack)");
    query.bindValue(":id", message.messageId);
    query.bindValue(":channel", message.channel);
    query.bindValue(":data", QJsonDocument::fromVariant(message.data).toJson());
    query.bindValue(":protocol", static_cast<int>(message.protocol));
    query.bindValue(":priority", static_cast<int>(message.priority));
    query.bindValue(":requires_ack", message.requiresAck);
    query.exec();
}

void MessageBus::loadPersistedMessages() {
    if (!m_persistenceEnabled)
        return;

    QSqlQuery query("SELECT * FROM messages ORDER BY priority DESC");
    while (query.next()) {
        Message msg;
        msg.messageId = query.value("id").toString();
        msg.channel = query.value("channel").toString();
        msg.data = QJsonDocument::fromJson(query.value("data").toByteArray())
                       .toVariant();
        msg.protocol = static_cast<Protocol>(query.value("protocol").toInt());
        msg.priority = static_cast<Priority>(query.value("priority").toInt());
        msg.requiresAck = query.value("requires_ack").toBool();
        enqueueMessage(msg);
    }
}

QString MessageBus::generateMessageId() {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

void MessageBus::enqueueMessage(const Message &message) {
    // Find the correct position to insert the message based on priority
    auto it = std::lower_bound(m_messageQueue.begin(), m_messageQueue.end(),
                               message, [](const Message &a, const Message &b) {
                                   return a.priority >
                                          b.priority;  // Higher priority first
                               });
    m_messageQueue.insert(it, message);

    // Persist the message if enabled
    if (m_persistenceEnabled) {
        persistMessage(message);
    }
}

QJsonObject variantMapToJsonObject(const QVariantMap &variantMap) {
    QJsonObject jsonObject;
    for (auto it = variantMap.constBegin(); it != variantMap.constEnd(); ++it) {
        jsonObject.insert(it.key(), QJsonValue::fromVariant(it.value()));
    }
    return jsonObject;
}

void MessageBus::processMessageQueue() {
    while (!m_messageQueue.isEmpty()) {
        Message msg = m_messageQueue.dequeue();

        QJsonObject jsonMessage;
        jsonMessage["channel"] = msg.channel;
        jsonMessage["data"] = QJsonValue::fromVariant(msg.data);
        jsonMessage["priority"] = static_cast<int>(msg.priority);
        jsonMessage["messageId"] = msg.messageId;
        jsonMessage["requiresAck"] = msg.requiresAck;

        QByteArray data =
            QJsonDocument(jsonMessage).toJson(QJsonDocument::Compact);

        bool sent = false;
        switch (msg.protocol) {
            case WebSocket:
                if (m_webSocketClient->isConnected()) {
                    m_webSocketClient->sendTextMessage(QString::fromUtf8(data));
                    sent = true;
                }
                break;
            case TCP:
                if (m_tcpClient->isConnected()) {
                    m_tcpClient->sendData(data);
                    sent = true;
                }
                break;
            case HTTP:
                QVariantMap jsonMessageMap = jsonMessage.toVariantMap();
                QJsonObject jsonMessage =
                    variantMapToJsonObject(jsonMessageMap);
                m_httpRequestCenter->post(msg.channel, jsonMessage);
                sent = true;  // Assume HTTP requests are always "sent"
                break;
        }

        if (sent) {
            if (m_persistenceEnabled && !msg.requiresAck) {
                // Remove the message from persistence if it doesn't require
                // acknowledgment
                QSqlQuery query;
                query.prepare("DELETE FROM messages WHERE id = :id");
                query.bindValue(":id", msg.messageId);
                query.exec();
            }
        } else {
            // If the message couldn't be sent, put it back in the queue
            m_messageQueue.prepend(msg);
            break;  // Stop processing for now
        }
    }
}

// Add these utility methods to help with JSON conversion
QJsonValue MessageBus::variantToJson(const QVariant &val) {
    if (val.canConvert<QVariantMap>()) {
        return QJsonObject::fromVariantMap(val.toMap());
    } else if (val.canConvert<QVariantList>()) {
        return QJsonArray::fromVariantList(val.toList());
    } else {
        return QJsonValue::fromVariant(val);
    }
}

QVariant MessageBus::jsonToVariant(const QJsonValue &val) {
    switch (val.type()) {
        case QJsonValue::Object:
            return val.toObject().toVariantMap();
        case QJsonValue::Array:
            return val.toArray().toVariantList();
        default:
            return val.toVariant();
    }
}