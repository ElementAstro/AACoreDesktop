#ifndef PROCESSMANAGERWIDGET_H
#define PROCESSMANAGERWIDGET_H

#include <QStandardItemModel>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "T_BasePage.h"

#include "Utils/Process.h"

class ElaComboBox;
class ElaLineEdit;
class ElaPushButton;
class ElaMenu;
class ElaTableView;

class T_ProcessPage : public T_BasePage {
    Q_OBJECT

public:
    T_ProcessPage(QWidget *parent = nullptr);

private slots:
    void refreshProcessList();
    void startNewProcess();
    void killSelectedProcess();
    void filterProcesses(const QString &text);
    void showProcessDetails(const QModelIndex &index);
    void showContextMenu(const QPoint &pos);
    void setProcessPriority(int priority);

private:
    void setupUI();
    void populateTable(const QStringList &processes);

    ProcessManager processManager;
    ElaTableView *processTableView;
    QStandardItemModel *tableModel;
    ElaPushButton *refreshButton;
    ElaPushButton *startButton;
    ElaPushButton *killButton;
    ElaLineEdit *searchBox;
    ElaComboBox *refreshRateComboBox;
    QTimer *refreshTimer;
    ElaMenu *contextMenu;
    QAction *lowPriorityAction;
    QAction *normalPriorityAction;
    QAction *highPriorityAction;
    int selectedProcessID;
};

#endif  // PROCESSMANAGERWIDGET_H
