#include "WebSocket.h"
#include <QDebug>

WebSocketClient::WebSocketClient(QObject *parent)
    : QObject(parent),
      m_webSocket(new QWebSocket),
      m_isConnected(false),
      m_initialReconnectInterval(1000),
      m_maxReconnectInterval(30000),
      m_sslVerificationEnabled(true),
      m_logLevel(Info) {
    connect(m_webSocket, &QWebSocket::connected, this,
            &WebSocketClient::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this,
            &WebSocketClient::onDisconnected);
    connect(m_webSocket, &QWebSocket::textMessageReceived, this,
            &WebSocketClient::onTextMessageReceived);
    connect(m_webSocket, &QWebSocket::binaryMessageReceived, this,
            &WebSocketClient::onBinaryMessageReceived);
    connect(m_webSocket,
            QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
            this, &WebSocketClient::onError);
    connect(m_webSocket, &QWebSocket::sslErrors, this,
            &WebSocketClient::onSslErrors);

    m_reconnectTimer.setSingleShot(true);
    connect(&m_reconnectTimer, &QTimer::timeout, this,
            &WebSocketClient::onReconnectTimer);

    connect(&m_heartbeatTimer, &QTimer::timeout, this,
            &WebSocketClient::sendHeartbeat);
}

WebSocketClient::~WebSocketClient() {
    closeConnection();
    delete m_webSocket;
}

void WebSocketClient::connectToServer(const QUrl &url,
                                      const QMap<QString, QString> &headers) {
    QMutexLocker locker(&m_mutex);
    m_serverUrl = url;
    m_customHeaders = headers;
    if (m_webSocket) {
        log(Debug, QString("Connecting to: %1").arg(url.toString()));
        QNetworkRequest request(url);
        for (auto it = m_customHeaders.constBegin();
             it != m_customHeaders.constEnd(); ++it) {
            request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
        }
        m_webSocket->open(request);
    }
}

void WebSocketClient::sendTextMessage(const QString &message) {
    QMutexLocker locker(&m_mutex);
    if (m_isConnected) {
        m_webSocket->sendTextMessage(message);
        log(Debug, "Sent text message");
    } else {
        log(Warning, "Cannot send text message. Not connected.");
        m_messageQueue.enqueue(qMakePair(true, message.toUtf8()));
    }
}

void WebSocketClient::sendBinaryMessage(const QByteArray &message) {
    QMutexLocker locker(&m_mutex);
    if (m_isConnected) {
        m_webSocket->sendBinaryMessage(message);
        log(Debug, "Sent binary message");
    } else {
        log(Warning, "Cannot send binary message. Not connected.");
        m_messageQueue.enqueue(qMakePair(false, message));
    }
}

void WebSocketClient::closeConnection() {
    QMutexLocker locker(&m_mutex);
    if (m_webSocket) {
        m_webSocket->close();
    }
}

bool WebSocketClient::isConnected() const { return m_isConnected; }

void WebSocketClient::setReconnectInterval(int initialMsecs, int maxMsecs) {
    QMutexLocker locker(&m_mutex);
    m_initialReconnectInterval = initialMsecs;
    m_maxReconnectInterval = maxMsecs;
}

void WebSocketClient::setHeartbeatInterval(int msecs) {
    QMutexLocker locker(&m_mutex);
    m_heartbeatTimer.setInterval(msecs);
    if (msecs > 0) {
        m_heartbeatTimer.start();
    } else {
        m_heartbeatTimer.stop();
    }
}

void WebSocketClient::enableSslCertificateVerification(bool enable) {
    QMutexLocker locker(&m_mutex);
    m_sslVerificationEnabled = enable;
}

void WebSocketClient::setLogLevel(LogLevel level) {
    QMutexLocker locker(&m_mutex);
    m_logLevel = level;
}

void WebSocketClient::setSslConfiguration(const QSslConfiguration &config) {
    QMutexLocker locker(&m_mutex);
    m_sslConfig = config;
    applySslConfiguration();
}

void WebSocketClient::onConnected() {
    QMutexLocker locker(&m_mutex);
    m_isConnected = true;
    log(Info, "Connected to server");
    emit connected();
    processMessageQueue();
    m_reconnectTimer.stop();
}

void WebSocketClient::onDisconnected() {
    QMutexLocker locker(&m_mutex);
    m_isConnected = false;
    log(Info, "Disconnected from server");
    emit disconnected();
    scheduleReconnect();
}

void WebSocketClient::onTextMessageReceived(const QString &message) {
    log(Debug, QString("Received text message: %1").arg(message));
    emit textMessageReceived(message);
}

void WebSocketClient::onBinaryMessageReceived(const QByteArray &message) {
    log(Debug, QString("Received binary message of size %1 bytes")
                   .arg(message.size()));
    emit binaryMessageReceived(message);
}

void WebSocketClient::onError(QAbstractSocket::SocketError error) {
    Q_UNUSED(error)
    log(Error, QString("WebSocket error: %1").arg(m_webSocket->errorString()));
    emit errorOccurred(m_webSocket->errorString());
    scheduleReconnect();
}

void WebSocketClient::onSslErrors(const QList<QSslError> &errors) {
    if (m_sslVerificationEnabled) {
        for (const QSslError &error : errors) {
            log(Error, QString("SSL Error: %1").arg(error.errorString()));
        }
    } else {
        m_webSocket->ignoreSslErrors();
        log(Warning, "Ignored SSL errors");
    }
}

void WebSocketClient::onReconnectTimer() {
    log(Info, "Attempting to reconnect...");
    connectToServer(m_serverUrl, m_customHeaders);
}

void WebSocketClient::sendHeartbeat() { sendTextMessage("PING"); }

void WebSocketClient::processMessageQueue() {
    while (!m_messageQueue.isEmpty()) {
        auto message = m_messageQueue.dequeue();
        if (message.first) {
            sendTextMessage(QString::fromUtf8(message.second));
        } else {
            sendBinaryMessage(message.second);
        }
    }
}

void WebSocketClient::scheduleReconnect() {
    if (!m_reconnectTimer.isActive()) {
        static int currentInterval = m_initialReconnectInterval;
        m_reconnectTimer.start(currentInterval);
        currentInterval = qMin(currentInterval * 2, m_maxReconnectInterval);
    }
}

void WebSocketClient::clearMessageQueue() {
    QMutexLocker locker(&m_mutex);
    m_messageQueue.clear();
}

void WebSocketClient::log(LogLevel level, const QString &message) {
    if (level >= m_logLevel) {
        emit logMessage(level, message);
        switch (level) {
            case Debug:
                qDebug() << message;
                break;
            case Info:
                qInfo() << message;
                break;
            case Warning:
                qWarning() << message;
                break;
            case Error:
                qCritical() << message;
                break;
        }
    }
}

void WebSocketClient::applySslConfiguration() {
    if (m_webSocket) {
        m_webSocket->setSslConfiguration(m_sslConfig);
    }
}