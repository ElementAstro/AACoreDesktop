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

namespace {
constexpr int kMaxLogLines = 1000;
constexpr int kTruncateIntervalMs = 60000;
}  // namespace

T_LogPanelPage::T_LogPanelPage(QWidget *parent)
    : T_BasePage(parent),
      logTextEdit(new ElaPlainTextEdit(this)),
      filterComboBox(new ElaComboBox(this)),
      clearButton(new ElaPushButton("清空日志", this)),
      saveButton(new ElaPushButton("保存日志", this)),
      saveHtmlButton(new ElaPushButton("导出为HTML", this)),
      searchLineEdit(new ElaLineEdit(this)),
      searchButton(new ElaPushButton("搜索", this)),
      autoScrollCheckBox(new ElaCheckBox("自动滚动", this)),
      pauseLoggingCheckBox(new ElaCheckBox("暂停日志记录", this)),
      currentFilterLevel(Info),
      autoScrollEnabled(true),
      loggingPaused(false),
      truncateTimer(new QTimer(this)) {
    logTextEdit->setReadOnly(true);

    filterComboBox->addItem("全部", QVariant::fromValue(-1));
    filterComboBox->addItem("信息", QVariant::fromValue(Info));
    filterComboBox->addItem("警告", QVariant::fromValue(Warning));
    filterComboBox->addItem("错误", QVariant::fromValue(Error));

    auto *mainLayout = new QVBoxLayout;
    auto *controlLayout = new QHBoxLayout;
    controlLayout->addWidget(filterComboBox);
    controlLayout->addWidget(searchLineEdit);
    controlLayout->addWidget(searchButton);
    controlLayout->addWidget(autoScrollCheckBox);
    controlLayout->addWidget(pauseLoggingCheckBox);  // 新增暂停复选框
    controlLayout->addWidget(clearButton);
    controlLayout->addWidget(saveButton);
    controlLayout->addWidget(saveHtmlButton);  // 新增HTML按钮

    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(logTextEdit);

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("日志面板");
    auto *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);

    connect(filterComboBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(filterLogs()));
    connect(clearButton, &ElaPushButton::clicked, this,
            &T_LogPanelPage::clearLogs);
    connect(saveButton, &ElaPushButton::clicked, this,
            &T_LogPanelPage::saveLogs);
    connect(saveHtmlButton, &ElaPushButton::clicked, this,
            &T_LogPanelPage::saveLogsAsHtml);
    connect(searchButton, &ElaPushButton::clicked, this,
            &T_LogPanelPage::searchLogs);
    connect(autoScrollCheckBox, &ElaCheckBox::toggled, this,
            &T_LogPanelPage::autoScrollToggled);
    connect(pauseLoggingCheckBox, &ElaCheckBox::toggled, this,
            &T_LogPanelPage::pauseLoggingToggled);
    connect(truncateTimer, &QTimer::timeout, this,
            &T_LogPanelPage::truncateLogs);

    autoScrollCheckBox->setChecked(true);
    truncateTimer->start(kTruncateIntervalMs);

    qInstallMessageHandler(T_LogPanelPage::messageHandler);
}

T_LogPanelPage::~T_LogPanelPage() = default;

void T_LogPanelPage::addLogMessage(const QString &message, LogLevel level) {
    if (loggingPaused) {
        return;
    }

    QMutexLocker locker(&logMutex);

    QString timeStamp =
        QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss]");
    QString formattedMessage = timeStamp + " " + message;

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

    QString coloredMessage = QString("<font color=\"%1\">%2</font>")
                                 .arg(colorCode, formattedMessage);
    logTextEdit->appendHtml(coloredMessage);

    if (autoScrollEnabled) {
        logTextEdit->moveCursor(QTextCursor::End);
    }
}

void T_LogPanelPage::filterLogs() {
    QMutexLocker locker(&logMutex);
    currentFilterLevel =
        static_cast<LogLevel>(filterComboBox->currentData().toInt());

    logTextEdit->moveCursor(QTextCursor::Start);
    QStringList lines = logTextEdit->toPlainText().split("\n");

    logTextEdit->clear();
    for (const QString &line : lines) {
        if ((line.contains("[信息]") && currentFilterLevel <= Info) ||
            (line.contains("[警告]") && currentFilterLevel <= Warning) ||
            (line.contains("[错误]") && currentFilterLevel <= Error) ||
            currentFilterLevel == static_cast<LogLevel>(-1)) {
            logTextEdit->appendPlainText(line);
        }
    }
}

void T_LogPanelPage::clearLogs() { logTextEdit->clear(); }

void T_LogPanelPage::saveLogs() {
    QString fileName = QFileDialog::getSaveFileName(
        this, "保存日志", "", "文本文件 (*.txt);;所有文件 (*)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << logTextEdit->toPlainText();
        file.close();
    } else {
        QMessageBox::warning(this, "保存日志", "无法保存日志文件。");
    }
}

void T_LogPanelPage::saveLogsAsHtml() {
    QString fileName = QFileDialog::getSaveFileName(
        this, "导出为HTML", "", "HTML文件 (*.html);;所有文件 (*)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        QString html;
        QTextStream stream(&html);
        stream << "<html><body><pre>" << logTextEdit->toPlainText()
               << "</pre></body></html>";
        out << html;
        file.close();
    } else {
        QMessageBox::warning(this, "导出为HTML", "无法保存HTML文件。");
    }
}

void T_LogPanelPage::searchLogs() {
    QString searchText = searchLineEdit->text();
    if (searchText.isEmpty()) {
        QMessageBox::information(this, "搜索", "请输入搜索内容");
        return;
    }

    logTextEdit->moveCursor(QTextCursor::Start);
}

void T_LogPanelPage::autoScrollToggled(bool checked) {
    autoScrollEnabled = checked;
}

void T_LogPanelPage::pauseLoggingToggled(bool checked) {
    loggingPaused = checked;
}

void T_LogPanelPage::truncateLogs() {
    QMutexLocker locker(&logMutex);

    QStringList lines = logTextEdit->toPlainText().split("\n");
    if (lines.size() > kMaxLogLines) {
        lines = lines.mid(lines.size() - kMaxLogLines);
        logTextEdit->clear();
        logTextEdit->appendPlainText(lines.join("\n"));
    }
}

void T_LogPanelPage::messageHandler(QtMsgType type,
                                    const QMessageLogContext & /*context*/,
                                    const QString &msg) {
    QString level;
    LogLevel logLevel;

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

    auto *instance =
        qobject_cast<T_LogPanelPage *>(QApplication::activeWindow());
    if (instance != nullptr) {
        instance->addLogMessage(level + " " + msg, logLevel);
    }
}
