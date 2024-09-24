#include "T_Process.hpp"

#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QThread>

#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaMenu.h"
#include "ElaPushButton.h"
#include "ElaTableView.h" 
#include "ElaText.h"

T_ProcessPage::T_ProcessPage(QWidget *parent)
    : T_BasePage(parent),
      processTableView(new ElaTableView(this)),
      tableModel(new QStandardItemModel(this)),
      refreshButton(new ElaPushButton("Refresh", this)),
      startButton(new ElaPushButton("Start Process", this)),
      killButton(new ElaPushButton("Kill Process", this)),
      refreshTimer(new QTimer(this)),
      contextMenu(new ElaMenu(this)) {
    setupUI();
    refreshProcessList();  // 初始化时刷新进程列表

    // 连接按钮点击事件
    connect(refreshButton, &ElaPushButton::clicked, this,
            &T_ProcessPage::refreshProcessList);
    connect(startButton, &ElaPushButton::clicked, this,
            &T_ProcessPage::startNewProcess);
    connect(killButton, &ElaPushButton::clicked, this,
            &T_ProcessPage::killSelectedProcess);
    connect(searchBox, &ElaLineEdit::textChanged, this,
            &T_ProcessPage::filterProcesses);

    // 定时器连接到刷新进程列表
    connect(refreshTimer, &QTimer::timeout, this,
            &T_ProcessPage::refreshProcessList);
    refreshTimer->start(1000);  // 默认1秒刷新一次

    // 刷新频率下拉框处理
    connect(refreshRateComboBox, &ElaComboBox::currentTextChanged,
            [this](const QString &text) {
                if (text == "1 Second") {
                    refreshTimer->start(1000);
                } else if (text == "5 Seconds") {
                    refreshTimer->start(5000);
                } else if (text == "10 Seconds") {
                    refreshTimer->start(10000);
                }
            });

    // 右键菜单操作
    connect(processTableView, &QTableView::customContextMenuRequested, this,
            &T_ProcessPage::showContextMenu);
    lowPriorityAction = contextMenu->addAction("Set Low Priority");
    normalPriorityAction = contextMenu->addAction("Set Normal Priority");
    highPriorityAction = contextMenu->addAction("Set High Priority");

    connect(lowPriorityAction, &QAction::triggered,
            [this]() { setProcessPriority(QThread::LowPriority); });
    connect(normalPriorityAction, &QAction::triggered,
            [this]() { setProcessPriority(QThread::NormalPriority); });
    connect(highPriorityAction, &QAction::triggered,
            [this]() { setProcessPriority(QThread::HighPriority); });
}

void T_ProcessPage::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 搜索框
    searchBox = new ElaLineEdit(this);
    searchBox->setPlaceholderText("Search for process...");
    layout->addWidget(searchBox);

    // 设置表头
    tableModel->setHorizontalHeaderLabels({"Process Name", "Process ID"});
    processTableView->setModel(tableModel);
    processTableView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
    processTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    processTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    processTableView->setContextMenuPolicy(
        Qt::CustomContextMenu);  // 启用右键菜单

    layout->addWidget(processTableView);

    // 刷新按钮
    layout->addWidget(refreshButton);

    // 启动进程按钮
    layout->addWidget(startButton);

    // 终止进程按钮
    layout->addWidget(killButton);

    // 刷新频率选择框
    refreshRateComboBox = new ElaComboBox(this);
    refreshRateComboBox->addItems({"1 Second", "5 Seconds", "10 Seconds"});
    layout->addWidget(refreshRateComboBox);

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("进程管理");
    auto *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(layout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

void T_ProcessPage::refreshProcessList() {
    QStringList processes = processManager.listProcesses();
    populateTable(processes);
}

void T_ProcessPage::populateTable(const QStringList &processes) {
    tableModel->clear();
    tableModel->setHorizontalHeaderLabels({"Process Name", "Process ID"});

    for (const QString &processInfo : processes) {
        QStringList details =
            processInfo.split(" ");  // 假设进程信息是以空格分隔的
        if (details.size() == 2) {
            QList<QStandardItem *> items;
            items.append(new QStandardItem(details[0]));  // 进程名
            items.append(new QStandardItem(details[1]));  // 进程ID
            tableModel->appendRow(items);
        }
    }
}

void T_ProcessPage::startNewProcess() {
    bool ok;
    QString program = QInputDialog::getText(
        this, "Start New Process", "Program:", ElaLineEdit::Normal, "", &ok);
    if (ok && !program.isEmpty()) {
        if (processManager.startProcess(program)) {
            QMessageBox::information(this, "Success",
                                     "Process started successfully.");
        } else {
            QMessageBox::warning(this, "Failure",
                                 "Failed to start the process.");
        }
    }
}

void T_ProcessPage::killSelectedProcess() {
    QModelIndexList selectedRows =
        processTableView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        QMessageBox::warning(this, "No selection",
                             "Please select one or more processes to kill.");
        return;
    }

    for (const QModelIndex &index : selectedRows) {
        QString processID = tableModel->item(index.row(), 1)->text();
        int pid = processID.toInt();
        processManager.killProcess(pid);
    }

    refreshProcessList();  // 重新刷新进程列表
}

void T_ProcessPage::filterProcesses(const QString &text) {
    for (int i = 0; i < tableModel->rowCount(); ++i) {
        bool match = tableModel->item(i, 0)->text().contains(
            text, Qt::CaseInsensitive);  // 匹配进程名
        processTableView->setRowHidden(i, !match);
    }
}

void T_ProcessPage::showProcessDetails(const QModelIndex &index) {
    QString processID = tableModel->item(index.row(), 1)->text();
    int pid = processID.toInt();
    QString details = processManager.getProcessInfo(pid);  // 获取进程详细信息
    QMessageBox::information(this, "Process Details", details);
}

void T_ProcessPage::showContextMenu(const QPoint &pos) {
    QModelIndex index = processTableView->indexAt(pos);
    if (index.isValid()) {
        selectedProcessID =
            tableModel->item(index.row(), 1)->text().toInt();  // 获取选中进程ID
        contextMenu->exec(processTableView->viewport()->mapToGlobal(pos));
    }
}

void T_ProcessPage::setProcessPriority(int priority) {
    if (processManager.setProcessPriority(selectedProcessID, priority)) {
        QMessageBox::information(this, "Success",
                                 "Process priority changed successfully.");
    } else {
        QMessageBox::warning(this, "Failure",
                             "Failed to change process priority.");
    }
}
