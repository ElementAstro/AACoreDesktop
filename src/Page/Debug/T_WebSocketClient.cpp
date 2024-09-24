#include "T_WebSocketClient.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QVBoxLayout>

#include "ElaLineEdit.h"
#include "ElaPlainTextEdit.h"
#include "ElaPushButton.h"
#include "ElaText.h"

T_WebSocketClientPage::T_WebSocketClientPage(QWidget *parent)
    : QWidget(parent), m_webSocketClient(new WebSocketClient(this)) {
    initLayout();

    connect(m_webSocketClient, &WebSocketClient::textMessageReceived, this,
            &T_WebSocketClientPage::handleReceivedTextMessage);
    connect(m_webSocketClient, &WebSocketClient::binaryMessageReceived, this,
            &T_WebSocketClientPage::handleReceivedBinaryMessage);
    connect(m_webSocketClient, &WebSocketClient::errorOccurred, this,
            &T_WebSocketClientPage::handleErrorOccurred);
    connect(m_webSocketClient, &WebSocketClient::connected, this,
            &T_WebSocketClientPage::handleConnectionStatus);
    connect(m_webSocketClient, &WebSocketClient::disconnected, this,
            &T_WebSocketClientPage::handleConnectionStatus);
}

T_WebSocketClientPage::~T_WebSocketClientPage() {}

void T_WebSocketClientPage::initLayout() {
    // Initialize UI elements
    urlLineEdit = new ElaLineEdit(this);
    messageInputTextEdit = new ElaPlainTextEdit(this);
    receivedMessagesTextEdit = new ElaPlainTextEdit(this);
    connectButton = new ElaPushButton("Connect", this);
    sendButton = new ElaPushButton("Send", this);
    disconnectButton = new ElaPushButton("Disconnect", this);
    disconnectButton->setEnabled(false);  // Initially disabled

    // Set up layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *urlLayout = new QHBoxLayout();

    auto *websocketUrlLable = new ElaText("WebSocket URL:", this);
    websocketUrlLable->setTextPixelSize(15);
    urlLayout->addWidget(websocketUrlLable);

    urlLayout->addWidget(urlLineEdit);
    urlLayout->addWidget(connectButton);
    urlLayout->addWidget(disconnectButton);

    mainLayout->addLayout(urlLayout);
    auto *messageLable = new ElaText("Message to Send:", this);
    messageLable->setTextPixelSize(15);
    mainLayout->addWidget(messageLable);

    mainLayout->addWidget(messageInputTextEdit);
    mainLayout->addWidget(sendButton);

    auto *receivedMessagesLable = new ElaText("Received Messages:", this);
    receivedMessagesLable->setTextPixelSize(15);
    mainLayout->addWidget(receivedMessagesLable);
    mainLayout->addWidget(receivedMessagesTextEdit);

    setLayout(mainLayout);

    // Connect buttons to slots
    connect(connectButton, &ElaPushButton::clicked, this,
            &T_WebSocketClientPage::on_connectButton_clicked);
    connect(sendButton, &ElaPushButton::clicked, this,
            &T_WebSocketClientPage::on_sendButton_clicked);
    connect(disconnectButton, &ElaPushButton::clicked, this,
            &T_WebSocketClientPage::on_disconnectButton_clicked);
}

void T_WebSocketClientPage::on_connectButton_clicked() {
    QUrl url(urlLineEdit->text());
    if (url.isValid()) {
        m_webSocketClient->connectToServer(url);
        connectButton->setEnabled(false);
        disconnectButton->setEnabled(true);
    } else {
        QMessageBox::warning(this, "Invalid URL",
                             "Please enter a valid WebSocket URL.");
    }
}

void T_WebSocketClientPage::on_sendButton_clicked() {
    QString message = messageInputTextEdit->toPlainText();
    if (!message.isEmpty()) {
        m_webSocketClient->sendTextMessage(message);
        messageInputTextEdit->clear();
    } else {
        QMessageBox::warning(this, "Empty Message",
                             "Please enter a message to send.");
    }
}

void T_WebSocketClientPage::on_disconnectButton_clicked() {
    m_webSocketClient->closeConnection();
    connectButton->setEnabled(true);
    disconnectButton->setEnabled(false);
}

void T_WebSocketClientPage::handleReceivedTextMessage(const QString &message) {
    receivedMessagesTextEdit->insertPlainText("Text: " + message);
}

void T_WebSocketClientPage::handleReceivedBinaryMessage(
    const QByteArray &message) {
    receivedMessagesTextEdit->insertPlainText("Binary: " + message.toHex());
}

void T_WebSocketClientPage::handleErrorOccurred(const QString &errorString) {
    QMessageBox::warning(this, "Error", errorString);
}

void T_WebSocketClientPage::handleConnectionStatus() {
    if (m_webSocketClient->isConnected()) {
        receivedMessagesTextEdit->insertPlainText(
            "Connected to the WebSocket server.");
    } else {
        receivedMessagesTextEdit->insertPlainText(
            "Disconnected from the WebSocket server.");
    }
}
