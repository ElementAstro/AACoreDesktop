#include "T_Terminal.h"

#include <QCompleter>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QProcess>
#include <QScrollBar>


#include "ElaLineEdit.h"
#include "ElaPlainTextEdit.h"
#include "ElaPushButton.h"

namespace {
const int DEFAULT_PORT = 22;
const int BUFFER_SIZE = 256;
}  // namespace

T_TerminalPage::T_TerminalPage(QWidget *parent)
    : T_BasePage(parent),
      sshSession(nullptr),
      historyIndex(-1),
      localTerminalProcess(nullptr) {
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

    auto *disconnectButton = new ElaPushButton("Disconnect", this);
    connect(disconnectButton, &ElaPushButton::clicked, this,
            &T_TerminalPage::onDisconnectButtonPressed);

    auto *clearButton = new ElaPushButton("Clear", this);
    connect(clearButton, &ElaPushButton::clicked, this,
            &T_TerminalPage::onClearButtonPressed);

    auto *saveButton = new ElaPushButton("Save", this);
    connect(saveButton, &ElaPushButton::clicked, this,
            &T_TerminalPage::onSaveButtonPressed);

    // 自动补全功能
    QStringList commandList = {"ls", "cd", "mkdir", "rm", "clear", "exit"};
    completer = new QCompleter(commandList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    commandInput->setCompleter(completer);

    auto *hLayout = new QHBoxLayout();
    hLayout->addWidget(commandInput);
    hLayout->addWidget(connectButton);
    hLayout->addWidget(disconnectButton);
    hLayout->addWidget(clearButton);
    hLayout->addWidget(saveButton);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(terminalDisplay);
    layout->addLayout(hLayout);

    setLayout(layout);

    sshSession = ssh_new();
    if (sshSession == nullptr) {
        appendOutput("Failed to initialize SSH session.", Qt::red);
    }

    auto *centralWidget = new QWidget(this);
    auto *centerLayout = new QVBoxLayout(centralWidget);
    centralWidget->setWindowTitle("终端");
    centerLayout->addLayout(layout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);

    commandInput->installEventFilter(this);

    // 默认连接本地终端
    connectToLocalTerminal();
}

T_TerminalPage::~T_TerminalPage() {
    if (sshSession != nullptr) {
        ssh_disconnect(sshSession);
        ssh_free(sshSession);
    }
    if (localTerminalProcess != nullptr) {
        localTerminalProcess->terminate();
        localTerminalProcess->waitForFinished();
        delete localTerminalProcess;
    }
}

bool T_TerminalPage::connectToSSH(const QString &host, int port,
                                  const QString &username,
                                  const QString &password) {
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

bool T_TerminalPage::connectToLocalTerminal() {
    localTerminalProcess = new QProcess(this);
    connect(localTerminalProcess, &QProcess::readyReadStandardOutput, this,
            [this]() {
                appendOutput(localTerminalProcess->readAllStandardOutput(),
                             Qt::white);
            });
    connect(localTerminalProcess, &QProcess::readyReadStandardError, this,
            [this]() {
                appendOutput(localTerminalProcess->readAllStandardError(),
                             Qt::red);
            });

    localTerminalProcess->start("cmd.exe");
    if (!localTerminalProcess->waitForStarted()) {
        appendOutput("Failed to start local terminal.", Qt::red);
        return false;
    }

    appendOutput("Local terminal started.", Qt::green);
    return true;
}

QString T_TerminalPage::executeRemoteCommand(const QString &command) {
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

    std::array<char, BUFFER_SIZE> buffer{};
    QString result;
    int nbytes;
#pragma unroll
    while ((nbytes = ssh_channel_read(channel, buffer.data(), buffer.size(),
                                      0)) > 0) {
        result.append(QString::fromUtf8(buffer.data(), nbytes));
    }

    if (nbytes < 0) {
        appendOutput(
            "Error reading from channel: " + QString(ssh_get_error(sshSession)),
            Qt::red);
    } else {
        appendOutput("Command executed successfully.", Qt::green);
    }

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);

    return result;
}

QString T_TerminalPage::executeLocalCommand(const QString &command) {
    if (localTerminalProcess == nullptr) {
        appendOutput("Local terminal is not started.", Qt::red);
        return "Not started";
    }

    localTerminalProcess->write(command.toUtf8() + "\n");
    return "";
}

void T_TerminalPage::onCommandEntered() {
    QString command = commandInput->text();
    if (command.isEmpty()) {
        return;
    }

    commandHistory.append(command);
    historyIndex = static_cast<int>(commandHistory.size());

    appendOutput("> " + command, Qt::green);

    QString output;
    if (sshSession != nullptr && (ssh_is_connected(sshSession) != 0)) {
        output = executeRemoteCommand(command);
    } else {
        output = executeLocalCommand(command);
    }
    appendOutput(output, Qt::white);

    commandInput->clear();
    auto *scrollBar = terminalDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void T_TerminalPage::appendOutput(const QString &output, const QColor &color) {
    auto cursor = terminalDisplay->textCursor();
    cursor.movePosition(QTextCursor::End);
    terminalDisplay->setTextCursor(cursor);

    QTextCharFormat format;
    format.setForeground(color);
    terminalDisplay->mergeCurrentCharFormat(format);
    terminalDisplay->insertPlainText(output + "\n");
    format.setForeground(Qt::white);
    terminalDisplay->mergeCurrentCharFormat(format);
}

void T_TerminalPage::clearTerminal() { terminalDisplay->clear(); }

void T_TerminalPage::onConnectButtonPressed() {
    clearTerminal();

    bool isDialogAccepted;
    QString host = QInputDialog::getText(
        this, tr("SSH Host"), tr("Enter SSH host:"), QLineEdit::Normal,
        "your_ssh_host", &isDialogAccepted);
    if (!isDialogAccepted || host.isEmpty()) {
        return;
    }

    QString username = QInputDialog::getText(
        this, tr("SSH Username"), tr("Enter SSH username:"), QLineEdit::Normal,
        "your_username", &isDialogAccepted);
    if (!isDialogAccepted || username.isEmpty()) {
        return;
    }

    QString password = QInputDialog::getText(
        this, tr("SSH Password"), tr("Enter SSH password:"),
        QLineEdit::Password, "your_password", &isDialogAccepted);
    if (!isDialogAccepted || password.isEmpty()) {
        return;
    }

    connectToSSH(host, DEFAULT_PORT, username, password);
}

void T_TerminalPage::onDisconnectButtonPressed() {
    if (sshSession != nullptr) {
        ssh_disconnect(sshSession);
        ssh_free(sshSession);
        sshSession = nullptr;
        appendOutput("SSH connection closed.", Qt::red);
    }
}

void T_TerminalPage::onClearButtonPressed() { clearTerminal(); }

void T_TerminalPage::onSaveButtonPressed() {
    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Save File"), "", tr("Text Files (*.txt);;All Files (*)"));
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"),
                             tr("Cannot save file: ") + file.errorString());
        return;
    }

    QTextStream out(&file);
    out << terminalDisplay->toPlainText();
    file.close();
}

auto T_TerminalPage::eventFilter(QObject *obj, QEvent *event) -> bool {
    if (obj == commandInput && event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Up) {
            if (historyIndex > 0) {
                historyIndex--;
                commandInput->setText(commandHistory.at(historyIndex));
            }
            return true;
        }
        if (keyEvent->key() == Qt::Key_Down) {
            if (historyIndex < commandHistory.size() - 1) {
                historyIndex++;
                commandInput->setText(commandHistory.at(historyIndex));
            } else {
                historyIndex = static_cast<int>(commandHistory.size());
                commandInput->clear();
            }
            return true;
        }
    }
    return T_BasePage::eventFilter(obj, event);
}