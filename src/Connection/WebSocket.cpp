#include "WebSocket.h"
#include <QDebug>

WebSocketClient::WebSocketClient(QObject *parent)
    : QObject(parent), m_webSocket(new QWebSocket), m_isConnected(false),
      m_sslVerificationEnabled(true) {
  connect(m_webSocket, &QWebSocket::connected, this,
          &WebSocketClient::onConnected);
  connect(m_webSocket, &QWebSocket::disconnected, this,
          &WebSocketClient::onDisconnected);
  connect(m_webSocket, &QWebSocket::textMessageReceived, this,
          &WebSocketClient::onTextMessageReceived);
  connect(m_webSocket, &QWebSocket::binaryMessageReceived, this,
          &WebSocketClient::onBinaryMessageReceived);
  connect(m_webSocket,
          QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this,
          &WebSocketClient::onError);
  connect(m_webSocket, &QWebSocket::sslErrors, this,
          &WebSocketClient::onSslErrors);

  m_reconnectTimer.setSingleShot(true);
  connect(&m_reconnectTimer, &QTimer::timeout, this,
          &WebSocketClient::onReconnectTimer);

  connect(&m_heartbeatTimer, &QTimer::timeout, this,
          &WebSocketClient::sendHeartbeat);
}

WebSocketClient::~WebSocketClient() {
  if (m_webSocket) {
    m_webSocket->close();
    delete m_webSocket;
  }
}

void WebSocketClient::connectToServer(const QUrl &url) {
  m_serverUrl = url;
  if (m_webSocket) {
    qDebug() << "WebSocketClient: Connecting to:" << url.toString();
    m_webSocket->open(url);
  }
}

void WebSocketClient::sendTextMessage(const QString &message) {
  if (m_isConnected) {
    m_webSocket->sendTextMessage(message);
  } else {
    qWarning()
        << "WebSocketClient: Cannot send message. Not connected to server.";
    m_messageQueue.enqueue(qMakePair(true, message.toUtf8()));
  }
}

void WebSocketClient::sendBinaryMessage(const QByteArray &message) {
  if (m_isConnected) {
    m_webSocket->sendBinaryMessage(message);
  } else {
    qWarning()
        << "WebSocketClient: Cannot send message. Not connected to server.";
    m_messageQueue.enqueue(qMakePair(false, message));
  }
}

void WebSocketClient::closeConnection() {
  if (m_webSocket) {
    m_webSocket->close();
  }
}

bool WebSocketClient::isConnected() const { return m_isConnected; }

void WebSocketClient::setReconnectInterval(int msecs) {
  m_reconnectTimer.setInterval(msecs);
}

void WebSocketClient::setHeartbeatInterval(int msecs) {
  m_heartbeatTimer.setInterval(msecs);
  if (msecs > 0) {
    m_heartbeatTimer.start();
  } else {
    m_heartbeatTimer.stop();
  }
}

void WebSocketClient::enableSslCertificateVerification(bool enable) {
  m_sslVerificationEnabled = enable;
}

void WebSocketClient::onConnected() {
  m_isConnected = true;
  qDebug() << "WebSocketClient: Connected to server";
  emit connected();
  processMessageQueue();
}

void WebSocketClient::onDisconnected() {
  m_isConnected = false;
  qDebug() << "WebSocketClient: Disconnected from server";
  emit disconnected();
  scheduleReconnect();
}

void WebSocketClient::onTextMessageReceived(const QString &message) {
  qDebug() << "WebSocketClient: Text message received:" << message;
  emit textMessageReceived(message);
}

void WebSocketClient::onBinaryMessageReceived(const QByteArray &message) {
  qDebug() << "WebSocketClient: Binary message received, size:"
           << message.size() << "bytes";
  emit binaryMessageReceived(message);
}

void WebSocketClient::onError(QAbstractSocket::SocketError error) {
  qDebug() << "WebSocketClient: Error:" << m_webSocket->errorString();
  emit errorOccurred(m_webSocket->errorString());
  scheduleReconnect();
}

void WebSocketClient::onSslErrors(const QList<QSslError> &errors) {
  if (m_sslVerificationEnabled) {
    for (const QSslError &error : errors) {
      qDebug() << "WebSocketClient: SSL Error:" << error.errorString();
    }
  } else {
    m_webSocket->ignoreSslErrors();
  }
}

void WebSocketClient::onReconnectTimer() {
  qDebug() << "WebSocketClient: Attempting to reconnect...";
  connectToServer(m_serverUrl);
}

void WebSocketClient::sendHeartbeat() {
  if (m_isConnected) {
    sendTextMessage("PING");
  }
}

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
    m_reconnectTimer.start();
  }
}

void WebSocketClient::clearMessageQueue() { m_messageQueue.clear(); }