#include "T_LogPanel.h"

#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QMessageBox>
#include <QTextCursor>
#include <QTextStream>
#include <QVBoxLayout>

#include "ElaCheckBox.h"
#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaPlainTextEdit.h"
#include "ElaPushButton.h"
#include "T_BasePage.h"

T_LogPanelPage::T_LogPanelPage(QWidget *parent)
    : T_BasePage(parent),
      logTextEdit(new ElaPlainTextEdit(this)),
      filterComboBox(new ElaComboBox(this)),
      clearButton(new ElaPushButton("清空日志", this)),
      saveButton(new ElaPushButton("保存日志", this)),
      searchLineEdit(new ElaLineEdit(this)),
      searchButton(new ElaPushButton("搜索", this)),
      autoScrollCheckBox(new ElaCheckBox("自动滚动", this)),
      currentFilterLevel(Info),
      autoScrollEnabled(true),
      truncateTimer(new QTimer(this)) {
    // 设置日志面板为只读
    logTextEdit->setReadOnly(true);

    // 设置过滤下拉框
    filterComboBox->addItem("全部", QVariant::fromValue(-1));
    filterComboBox->addItem("信息", QVariant::fromValue(Info));
    filterComboBox->addItem("警告", QVariant::fromValue(Warning));
    filterComboBox->addItem("错误", QVariant::fromValue(Error));

    // 布局设置
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->addWidget(filterComboBox);
    controlLayout->addWidget(searchLineEdit);
    controlLayout->addWidget(searchButton);
    controlLayout->addWidget(autoScrollCheckBox);
    controlLayout->addWidget(clearButton);
    controlLayout->addWidget(saveButton);

    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(logTextEdit);

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("日志面板");
    auto *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);

    // 连接信号槽
    connect(filterComboBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(filterLogs()));
    connect(clearButton, &ElaPushButton::clicked, this,
            &T_LogPanelPage::clearLogs);
    connect(saveButton, &ElaPushButton::clicked, this,
            &T_LogPanelPage::saveLogs);
    connect(searchButton, &ElaPushButton::clicked, this,
            &T_LogPanelPage::searchLogs);
    connect(autoScrollCheckBox, &ElaCheckBox::toggled, this,
            &T_LogPanelPage::autoScrollToggled);
    connect(truncateTimer, &QTimer::timeout, this,
            &T_LogPanelPage::truncateLogs);

    // 初始化设置
    autoScrollCheckBox->setChecked(true);
    truncateTimer->start(60000);  // 每分钟检查一次日志是否需要截断

    // 设置 Qt 的全局消息处理函数
    qInstallMessageHandler(T_LogPanelPage::messageHandler);
}

T_LogPanelPage::~T_LogPanelPage() {
    // 析构函数
}

void T_LogPanelPage::addLogMessage(const QString &message, LogLevel level) {
    QMutexLocker locker(&logMutex);  // 确保线程安全

    // 获取当前时间并格式化
    QString timeStamp =
        QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss]");
    QString formattedMessage = timeStamp + " " + message;

    // 根据日志级别设定颜色
    QString colorCode;
    switch (level) {
        case Info:
            colorCode = "black";
            break;
        case Warning:
            colorCode = "darkyellow";
            break;
        case Error:
            colorCode = "red";
            break;
    }

    // 添加带颜色的日志信息
    QString coloredMessage = QString("<font color=\"%1\">%2</font>")
                                 .arg(colorCode, formattedMessage);
    logTextEdit->appendHtml(coloredMessage);

    // 自动滚动到最新日志
    if (autoScrollEnabled) {
        logTextEdit->moveCursor(QTextCursor::End);
    }
}

void T_LogPanelPage::filterLogs() {
    QMutexLocker locker(&logMutex);  // 确保线程安全
    currentFilterLevel =
        static_cast<LogLevel>(filterComboBox->currentData().toInt());

    // 显示或隐藏日志
    logTextEdit->moveCursor(QTextCursor::Start);
    QStringList lines = logTextEdit->toPlainText().split("\n");

    logTextEdit->clear();

    for (const QString &line : lines) {
        if ((line.contains("[信息]") && currentFilterLevel <= Info) ||
            (line.contains("[警告]") && currentFilterLevel <= Warning) ||
            (line.contains("[错误]") && currentFilterLevel <= Error) ||
            currentFilterLevel == -1) {
            logTextEdit->appendPlainText(line);
        }
    }
}

void T_LogPanelPage::clearLogs() { logTextEdit->clear(); }

void T_LogPanelPage::saveLogs() {
    QString fileName = QFileDialog::getSaveFileName(
        this, "保存日志", "", "文本文件 (*.txt);;所有文件 (*)");
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << logTextEdit->toPlainText();
        file.close();
    } else {
        QMessageBox::warning(this, "保存日志", "无法保存日志文件。");
    }
}

void T_LogPanelPage::searchLogs() {
    QString searchText = searchLineEdit->text();
    if (searchText.isEmpty()) {
        QMessageBox::information(this, "搜索", "请输入搜索内容");
        return;
    }

    QTextCursor cursor = logTextEdit->textCursor();
    cursor.movePosition(QTextCursor::Start);
    logTextEdit->setTextCursor(cursor);

    // 查找并高亮显示搜索内容
    if (!logTextEdit->find(searchText)) {
        QMessageBox::information(this, "搜索", "未找到匹配内容");
    }
}

void T_LogPanelPage::autoScrollToggled(bool checked) {
    autoScrollEnabled = checked;
}

void T_LogPanelPage::truncateLogs() {
    QMutexLocker locker(&logMutex);  // 确保线程安全

    // 如果日志行数超过最大值，则截断
    QStringList lines = logTextEdit->toPlainText().split("\n");
    if (lines.size() > MaxLogLines) {
        lines =
            lines.mid(lines.size() - MaxLogLines);  // 保留最后的MaxLogLines行
        logTextEdit->clear();
        logTextEdit->appendPlainText(lines.join("\n"));
    }
}

void T_LogPanelPage::messageHandler(QtMsgType type,
                                    const QMessageLogContext & /*context*/,
                                    const QString &msg) {
    QString level;
    LogLevel logLevel;

    // 根据消息类型设置日志级别
    switch (type) {
        case QtDebugMsg:
            level = "[信息]";
            logLevel = Info;
            break;
        case QtWarningMsg:
            level = "[警告]";
            logLevel = Warning;
            break;
        case QtCriticalMsg:
        case QtFatalMsg:
            level = "[错误]";
            logLevel = Error;
            break;
        default:
            level = "[未知]";
            logLevel = Info;
            break;
    }

    // 获取当前活跃的主窗口实例并添加日志
    auto instance =
        qobject_cast<T_LogPanelPage *>(QApplication::activeWindow());
    if (instance) {
        instance->addLogMessage(level + " " + msg, logLevel);
    }
}