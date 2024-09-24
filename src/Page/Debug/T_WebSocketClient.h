#ifndef WEBSOCKETCLIENTWIDGET_H
#define WEBSOCKETCLIENTWIDGET_H

#include "Connection/WebSocket.h"

#include <QWidget>

class ElaLineEdit;
class ElaPlainTextEdit;
class ElaPushButton;

class T_WebSocketClientPage : public QWidget {
    Q_OBJECT

public:
    explicit T_WebSocketClientPage(QWidget *parent = nullptr);
    ~T_WebSocketClientPage();

private slots:
    void on_connectButton_clicked();
    void on_sendButton_clicked();
    void on_disconnectButton_clicked();
    void handleReceivedTextMessage(const QString &message);
    void handleReceivedBinaryMessage(const QByteArray &message);
    void handleErrorOccurred(const QString &errorString);
    void handleConnectionStatus();

private:
    ElaLineEdit *urlLineEdit;
    ElaPlainTextEdit *messageInputTextEdit;
    ElaPlainTextEdit *receivedMessagesTextEdit;
    ElaPushButton *connectButton;
    ElaPushButton *sendButton;
    ElaPushButton *disconnectButton;

    WebSocketClient *m_webSocketClient;

    void initLayout();
};

#endif  // WEBSOCKETCLIENTWIDGET_H
