#ifndef CONFIGMANAGEMENTWIDGET_H
#define CONFIGMANAGEMENTWIDGET_H

#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTimer>
#include <QWidget>
#include "T_BasePage.h"

class T_ConfigPanel : public T_BasePage {
    Q_OBJECT

public:
    explicit T_ConfigPanel(QWidget *parent = nullptr);

private:
    // UI Elements
    QListWidget *storeListWidget;
    QLineEdit *keyLineEdit;
    QLineEdit *valueLineEdit;
    QLineEdit *searchLineEdit;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QPushButton *resetButton;
    QPushButton *subscribeButton;
    QPushButton *unsubscribeButton;
    QPushButton *importButton;
    QPushButton *exportButton;
    QPushButton *batchUpdateButton;
    QPushButton *saveTimerButton;
    QLabel *statusLabel;
    QTimer *saveTimer;

    // UI Setup Methods
    void setupUI();
    void connectSignals();
    void loadStores();
    void refreshStoreList();

private slots:
    void addOrUpdateStore();
    void deleteStore();
    void resetStore();
    void subscribeToStore();
    void unsubscribeFromStore();
    void importConfig();
    void exportConfig();
    void onStoreSelectionChanged();
    void searchStores();
    void batchUpdate();
    void toggleAutoSave();

    // Utility Methods
    void autoSave();
};

#endif  // CONFIGMANAGEMENTWIDGET_H
