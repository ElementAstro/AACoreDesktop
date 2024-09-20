#include "Tcp.h"
#include <QDebug>

TcpClient::TcpClient(QObject *parent)
    : QObject(parent),
      m_socket(new QTcpSocket(this)),
      m_port(0),
      m_autoReconnect(false) {
    connect(m_socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this,
            &TcpClient::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &TcpClient::onError);

    m_reconnectTimer.setSingleShot(true);
    connect(&m_reconnectTimer, &QTimer::timeout, this,
            &TcpClient::onReconnectTimer);

    connect(&m_heartbeatTimer, &QTimer::timeout, this,
            &TcpClient::sendHeartbeat);
}

TcpClient::~TcpClient() { disconnectFromHost(); }

void TcpClient::connectToHost(const QString &host, quint16 port) {
    m_host = host;
    m_port = port;
    m_socket->connectToHost(host, port);
}

void TcpClient::disconnectFromHost() { m_socket->disconnectFromHost(); }

void TcpClient::sendData(const QByteArray &data) {
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->write(data);
    } else {
        m_sendQueue.enqueue(data);
    }
}

bool TcpClient::isConnected() const {
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

void TcpClient::setAutoReconnect(bool enable) { m_autoReconnect = enable; }

void TcpClient::setReconnectInterval(int msecs) {
    m_reconnectTimer.setInterval(msecs);
}

void TcpClient::setHeartbeatInterval(int msecs) {
    if (msecs > 0) {
        m_heartbeatTimer.start(msecs);
    } else {
        m_heartbeatTimer.stop();
    }
}

void TcpClient::onConnected() {
    qDebug() << "Connected to host";
    emit connected();
    processQueue();
}

void TcpClient::onDisconnected() {
    qDebug() << "Disconnected from host";
    emit disconnected();

    if (m_autoReconnect) {
        m_reconnectTimer.start();
    }
}

void TcpClient::onReadyRead() {
    QByteArray data = m_socket->readAll();
    emit dataReceived(data);
}

void TcpClient::onError(QAbstractSocket::SocketError socketError) {
    qDebug() << "Socket error:" << m_socket->errorString();
    emit error(socketError);

    if (m_autoReconnect) {
        m_reconnectTimer.start();
    }
}

void TcpClient::onReconnectTimer() {
    qDebug() << "Attempting to reconnect...";
    m_socket->connectToHost(m_host, m_port);
}

void TcpClient::sendHeartbeat() { sendData("HEARTBEAT"); }

void TcpClient::processQueue() {
    while (!m_sendQueue.isEmpty()) {
        QByteArray data = m_sendQueue.dequeue();
        m_socket->write(data);
    }
}