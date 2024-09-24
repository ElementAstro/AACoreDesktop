#ifndef TCPCIENTWIDGET_H
#define TCPCIENTWIDGET_H

#include "Connection/Tcp.h"

#include <QWidget>

class ElaLineEdit;
class ElaPlainTextEdit;
class ElaPushButton;

class T_TcpClientPage : public QWidget {
    Q_OBJECT

public:
    explicit T_TcpClientPage(QWidget *parent = nullptr);
    ~T_TcpClientPage();

private slots:
    void on_connectButton_clicked();
    void on_sendButton_clicked();
    void on_disconnectButton_clicked();
    void handleDataReceived(const QByteArray &data);
    void handleError(QAbstractSocket::SocketError socketError);
    void handleConnectionStatus();

private:
    ElaLineEdit *hostLineEdit;
    ElaLineEdit *portLineEdit;
    ElaPlainTextEdit *messageInputTextEdit;
    ElaPlainTextEdit *receivedMessagesTextEdit;
    ElaPushButton *connectButton;
    ElaPushButton *sendButton;
    ElaPushButton *disconnectButton;

    TcpClient *m_tcpClient;

    void initLayout();
};

#endif  // TCPCIENTWIDGET_H
