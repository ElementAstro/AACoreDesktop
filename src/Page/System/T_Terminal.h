#ifndef SSHTERMINALWIDGET_H
#define SSHTERMINALWIDGET_H

#include "T_BasePage.h"

#include <libssh/libssh.h>

#include <QCompleter>
#include <QProcess>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>

class ElaLineEdit;
class ElaPlainTextEdit;

class T_TerminalPage : public T_BasePage {
    Q_OBJECT

public:
    explicit T_TerminalPage(QWidget *parent = nullptr);
    ~T_TerminalPage();

    bool connectToSSH(const QString &host, int port, const QString &username,
                      const QString &password);
    bool connectToLocalTerminal();

private slots:
    void onCommandEntered();
    void onConnectButtonPressed();
    void onDisconnectButtonPressed();
    void onClearButtonPressed();
    void onSaveButtonPressed();

private:
    QString executeRemoteCommand(const QString &command);
    QString executeLocalCommand(const QString &command);
    void appendOutput(const QString &output, const QColor &color);
    void clearTerminal();
    auto eventFilter(QObject *obj, QEvent *event) -> bool;

    ElaPlainTextEdit *terminalDisplay;
    ElaLineEdit *commandInput;
    QStringList commandHistory;
    int historyIndex;
    ssh_session sshSession;  // SSH 会话
    QCompleter *completer;   // 命令自动补全
    QProcess *localTerminalProcess;  // 本地终端进程
};

#endif  // SSHTERMINALWIDGET_H