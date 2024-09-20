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

    // 添加日志消息
    void addLogMessage(const QString &message, LogLevel level = Info);

    // 重定向 Qt 的调试信息
    static void messageHandler(QtMsgType type,
                               const QMessageLogContext &context,
                               const QString &msg);

private slots:
    void filterLogs();             // 过滤日志
    void clearLogs();              // 清空日志
    void saveLogs();               // 保存日志
    void searchLogs();             // 搜索日志
    void autoScrollToggled(bool);  // 切换自动滚动
    void truncateLogs();           // 自动截断日志

private:
    ElaPlainTextEdit *logTextEdit;    // 日志显示区域
    ElaComboBox *filterComboBox;      // 日志级别过滤下拉框
    ElaPushButton *clearButton;       // 清空日志按钮
    ElaPushButton *saveButton;        // 保存日志按钮
    ElaLineEdit *searchLineEdit;      // 搜索输入框
    ElaPushButton *searchButton;      // 搜索按钮
    ElaCheckBox *autoScrollCheckBox;  // 自动滚动选项
    QMutex logMutex;                  // 日志锁，保证线程安全
    QTimer *truncateTimer;            // 日志自动截断定时器
    LogLevel currentFilterLevel;      // 当前过滤级别
    bool autoScrollEnabled;           // 是否启用自动滚动
    const int MaxLogLines = 1000;     // 最大日志行数，超过时截断
};

#endif  // T_LOGPANEL_H
