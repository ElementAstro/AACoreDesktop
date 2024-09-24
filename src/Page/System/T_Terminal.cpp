#include "T_Terminal.h"

#include <QCompleter>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QScrollBar>

#include "ElaLineEdit.h"
#include "ElaPlainTextEdit.h"
#include "ElaPushButton.h"

namespace {
const int DEFAULT_PORT = 22;
const int BUFFER_SIZE = 256;
}  // namespace

T_TerminalPage::T_TerminalPage(QWidget *parent)
    : T_BasePage(parent), sshSession(nullptr), historyIndex(-1) {
    terminalDisplay = new ElaPlainTextEdit(this);
    terminalDisplay->setReadOnly(true);
    terminalDisplay->setStyleSheet(
        "background-color: black; color: white; font-family: Courier;");

    commandInput = new ElaLineEdit(this);
    connect(commandInput, &ElaLineEdit::returnPressed, this,
            &T_TerminalPage::onCommandEntered);

    auto *connectButton = new ElaPushButton("Connect", this);
    connect(connectButton, &ElaPushButton::clicked, this,
            &T_TerminalPage::onConnectButtonPressed);

    // 自动补全功能
    QStringList commandList = {"ls", "cd", "mkdir", "rm", "clear", "exit"};
    completer = new QCompleter(commandList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    commandInput->setCompleter(completer);

    auto *hLayout = new QHBoxLayout();
    hLayout->addWidget(commandInput);
    hLayout->addWidget(connectButton);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(terminalDisplay);
    layout->addLayout(hLayout);

    setLayout(layout);

    sshSession = ssh_new();
    if (sshSession == nullptr) {
        appendOutput("Failed to initialize SSH session.", Qt::red);
    }

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("终端");
    QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(layout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

T_TerminalPage::~T_TerminalPage() {
    if (sshSession != nullptr) {
        ssh_disconnect(sshSession);
        ssh_free(sshSession);
    }
}

auto T_TerminalPage::connectToSSH(const QString &host, int port,
                                  const QString &username,
                                  const QString &password) -> bool {
    if (sshSession == nullptr) {
        appendOutput("SSH session is not initialized.", Qt::red);
        return false;
    }

    ssh_options_set(sshSession, SSH_OPTIONS_HOST, host.toStdString().c_str());
    ssh_options_set(sshSession, SSH_OPTIONS_PORT, &port);
    ssh_options_set(sshSession, SSH_OPTIONS_USER,
                    username.toStdString().c_str());

    int connectionStatus = ssh_connect(sshSession);
    if (connectionStatus != SSH_OK) {
        appendOutput(
            "Error connecting to host: " + QString(ssh_get_error(sshSession)),
            Qt::red);
        return false;
    }

    int authStatus = ssh_userauth_password(sshSession, nullptr,
                                           password.toStdString().c_str());
    if (authStatus != SSH_AUTH_SUCCESS) {
        appendOutput(
            "Authentication failed: " + QString(ssh_get_error(sshSession)),
            Qt::red);
        ssh_disconnect(sshSession);
        return false;
    }

    appendOutput("SSH connection established to " + host, Qt::green);
    return true;
}

auto T_TerminalPage::executeRemoteCommand(const QString &command) -> QString {
    if (sshSession == nullptr) {
        appendOutput("SSH session is not connected.", Qt::red);
        return "Not connected";
    }

    ssh_channel channel = ssh_channel_new(sshSession);
    if (channel == nullptr) {
        appendOutput(
            "Error creating channel: " + QString(ssh_get_error(sshSession)),
            Qt::red);
        return "Channel error";
    }

    int resultCode = ssh_channel_open_session(channel);
    if (resultCode != SSH_OK) {
        appendOutput(
            "Error opening session: " + QString(ssh_get_error(sshSession)),
            Qt::red);
        ssh_channel_free(channel);
        return "Session error";
    }

    resultCode =
        ssh_channel_request_exec(channel, command.toStdString().c_str());
    if (resultCode != SSH_OK) {
        appendOutput(
            "Error executing command: " + QString(ssh_get_error(sshSession)),
            Qt::red);
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return "Command error";
    }

    std::array<char, BUFFER_SIZE> buffer;
    QString result;
    int nbytes;
    while ((nbytes = ssh_channel_read(channel, buffer.data(), buffer.size(),
                                      0)) > 0) {
        result.append(QString::fromUtf8(buffer.data(), nbytes));
    }

    if (nbytes < 0) {
        appendOutput(
            "Error reading from channel: " + QString(ssh_get_error(sshSession)),
            Qt::red);
    }

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);

    return result;
}

void T_TerminalPage::onCommandEntered() {
    QString command = commandInput->text();
    if (command.isEmpty()) {
        return;
    }

    commandHistory.append(command);
    historyIndex = -1;

    appendOutput("> " + command, Qt::green);

    QString output = executeRemoteCommand(command);
    appendOutput(output, Qt::white);

    commandInput->clear();
    auto *scrollBar = terminalDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void T_TerminalPage::appendOutput(const QString &output, const QColor &color) {
    auto cursor = terminalDisplay->textCursor();
    cursor.movePosition(QTextCursor::End);
    terminalDisplay->setTextCursor(cursor);

    terminalDisplay->insertPlainText(output + "\n");
}

void T_TerminalPage::clearTerminal() { terminalDisplay->clear(); }

void T_TerminalPage::onConnectButtonPressed() {
    // 这里可以是一个对话框来输入 host、username 和
    // password，简化示例这里只是清理内容
    clearTerminal();
    connectToSSH("your_ssh_host", DEFAULT_PORT, "your_username",
                 "your_password");  // 替换为实际值
}