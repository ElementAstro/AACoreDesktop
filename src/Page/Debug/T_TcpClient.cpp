#include "T_TcpClient.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QVBoxLayout>

#include "ElaLineEdit.h"
#include "ElaPlainTextEdit.h"
#include "ElaPushButton.h"
#include "ElaText.h"

T_TcpClientPage::T_TcpClientPage(QWidget *parent)
    : QWidget(parent), m_tcpClient(new TcpClient(this)) {
    initLayout();

    connect(m_tcpClient, &TcpClient::dataReceived, this,
            &T_TcpClientPage::handleDataReceived);
    connect(m_tcpClient, &TcpClient::error, this,
            &T_TcpClientPage::handleError);
    connect(m_tcpClient, &TcpClient::connected, this,
            &T_TcpClientPage::handleConnectionStatus);
    connect(m_tcpClient, &TcpClient::disconnected, this,
            &T_TcpClientPage::handleConnectionStatus);
}

T_TcpClientPage::~T_TcpClientPage() {}

void T_TcpClientPage::initLayout() {
    // Initialize UI elements
    hostLineEdit = new ElaLineEdit(this);
    portLineEdit = new ElaLineEdit(this);
    messageInputTextEdit = new ElaPlainTextEdit(this);
    receivedMessagesTextEdit = new ElaPlainTextEdit(this);
    connectButton = new ElaPushButton("Connect", this);
    sendButton = new ElaPushButton("Send", this);
    disconnectButton = new ElaPushButton("Disconnect", this);
    disconnectButton->setEnabled(false);  // Initially disabled

    // Set up layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *hostLayout = new QHBoxLayout();

    auto *hostLabel = new ElaText("Host:", this);
    hostLabel->setTextPixelSize(15);
    hostLayout->addWidget(hostLabel);
    hostLayout->addWidget(hostLineEdit);

    auto *portLabel = new ElaText("Port:", this);
    portLabel->setTextPixelSize(15);
    hostLayout->addWidget(portLabel);
    hostLayout->addWidget(portLineEdit);
    hostLayout->addWidget(connectButton);
    hostLayout->addWidget(disconnectButton);

    mainLayout->addLayout(hostLayout);
    auto *messageLabel = new ElaText("Message to Send:", this);
    messageLabel->setTextPixelSize(15);
    mainLayout->addWidget(messageLabel);
    mainLayout->addWidget(messageInputTextEdit);
    mainLayout->addWidget(sendButton);
    auto *receivedLabel = new ElaText("Received Messages:", this);
    receivedLabel->setTextPixelSize(15);
    mainLayout->addWidget(receivedLabel);
    mainLayout->addWidget(receivedMessagesTextEdit);

    setLayout(mainLayout);

    // Connect buttons to slots
    connect(connectButton, &ElaPushButton::clicked, this,
            &T_TcpClientPage::on_connectButton_clicked);
    connect(sendButton, &ElaPushButton::clicked, this,
            &T_TcpClientPage::on_sendButton_clicked);
    connect(disconnectButton, &ElaPushButton::clicked, this,
            &T_TcpClientPage::on_disconnectButton_clicked);
}

void T_TcpClientPage::on_connectButton_clicked() {
    QString host = hostLineEdit->text();
    quint16 port = portLineEdit->text().toUInt();

    if (!host.isEmpty() && port != 0) {
        m_tcpClient->connectToHost(host, port);
        connectButton->setEnabled(false);
        disconnectButton->setEnabled(true);
    } else {
        QMessageBox::warning(this, "Invalid Input",
                             "Please enter a valid host and port.");
    }
}

void T_TcpClientPage::on_sendButton_clicked() {
    QByteArray data = messageInputTextEdit->toPlainText().toUtf8();
    if (!data.isEmpty()) {
        m_tcpClient->sendData(data);
        messageInputTextEdit->clear();
    } else {
        QMessageBox::warning(this, "Empty Message",
                             "Please enter a message to send.");
    }
}

void T_TcpClientPage::on_disconnectButton_clicked() {
    m_tcpClient->disconnectFromHost();
    connectButton->setEnabled(true);
    disconnectButton->setEnabled(false);
}

void T_TcpClientPage::handleDataReceived(const QByteArray &data) {
    receivedMessagesTextEdit->insertPlainText(
        QString("Received: %1").arg(QString(data)));
}

void T_TcpClientPage::handleError(QAbstractSocket::SocketError socketError) {
    // QMessageBox::warning(this, "Error", m_tcpClient->);
}

void T_TcpClientPage::handleConnectionStatus() {
    if (m_tcpClient->isConnected()) {
        receivedMessagesTextEdit->insertPlainText(
            "Connected to the TCP server.");
    } else {
        receivedMessagesTextEdit->insertPlainText(
            "Disconnected from the TCP server.");
    }
}
