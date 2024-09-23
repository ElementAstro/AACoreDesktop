#ifndef SSHTERMINALWIDGET_H
#define SSHTERMINALWIDGET_H

#include "T_BasePage.h"

#include <libssh/libssh.h>

#include <QCompleter>
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

private slots:
    void onCommandEntered();
    void onConnectButtonPressed();

private:
    QString executeRemoteCommand(const QString &command);
    void appendOutput(const QString &output, const QColor &color);
    void clearTerminal();

    ElaPlainTextEdit *terminalDisplay;
    ElaLineEdit *commandInput;
    QStringList commandHistory;
    int historyIndex;
    ssh_session sshSession;  // SSH 会话
    QCompleter *completer;   // 命令自动补全
};

#endif  // SSHTERMINALWIDGET_H
