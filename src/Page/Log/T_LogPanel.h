#ifndef T_LOGPANEL_H
#define T_LOGPANEL_H

#include "T_BasePage.h"

#include <QMutex>
#include <QTimer>

class ElaPlainTextEdit;
class ElaComboBox;
class ElaPushButton;
class ElaCheckBox;
class ElaLineEdit;

enum LogLevel { Info, Warning, Error };

class T_LogPanelPage : public T_BasePage {
    Q_OBJECT

public:
    explicit T_LogPanelPage(QWidget *parent = nullptr);
    ~T_LogPanelPage();

    void addLogMessage(const QString &message, LogLevel level = Info);
    static void messageHandler(QtMsgType type,
                               const QMessageLogContext &context,
                               const QString &msg);

private slots:
    void filterLogs();
    void clearLogs();
    void saveLogs();
    void saveLogsAsHtml();  // 新增保存为HTML功能
    void searchLogs();
    void autoScrollToggled(bool);
    void pauseLoggingToggled(bool);  // 新增暂停日志记录功能
    void truncateLogs();

private:
    ElaPlainTextEdit *logTextEdit;
    ElaComboBox *filterComboBox;
    ElaPushButton *clearButton;
    ElaPushButton *saveButton;
    ElaPushButton *saveHtmlButton;  // 新增保存HTML按钮
    ElaLineEdit *searchLineEdit;
    ElaPushButton *searchButton;
    ElaCheckBox *autoScrollCheckBox;
    ElaCheckBox *pauseLoggingCheckBox;  // 新增暂停日志复选框
    QMutex logMutex;
    QTimer *truncateTimer;
    LogLevel currentFilterLevel;
    bool autoScrollEnabled;
    bool loggingPaused;  // 新增是否暂停日志记录
    const int MaxLogLines = 1000;
};

#endif  // T_LOGPANEL_H
