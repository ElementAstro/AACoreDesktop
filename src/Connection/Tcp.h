#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QQueue>
#include <QTimer>
#include <QtNetwork/QTcpSocket>

class TcpClient : public QObject {
    Q_OBJECT

public:
    explicit TcpClient(QObject *parent = nullptr);
    ~TcpClient();

    void connectToHost(const QString &host, quint16 port);
    void disconnectFromHost();
    void sendData(const QByteArray &data);

    bool isConnected() const;
    void setAutoReconnect(bool enable);
    void setReconnectInterval(int msecs);
    void setHeartbeatInterval(int msecs);

signals:
    void connected();
    void disconnected();
    void dataReceived(const QByteArray &data);
    void error(QAbstractSocket::SocketError socketError);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);
    void onReconnectTimer();
    void sendHeartbeat();

private:
    QTcpSocket *m_socket;
    QString m_host;
    quint16 m_port;
    bool m_autoReconnect;
    QTimer m_reconnectTimer;
    QTimer m_heartbeatTimer;
    QQueue<QByteArray> m_sendQueue;

    void processQueue();
};

#endif  // TCPCLIENT_H