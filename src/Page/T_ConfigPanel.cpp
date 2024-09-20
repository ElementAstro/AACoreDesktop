#include "T_ConfigPanel.h"
#include "Config/GlobalConfig.h"
#include "T_BasePage.h"
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QVBoxLayout>

#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "ElaText.h"


T_ConfigPanel::T_ConfigPanel(QWidget *parent)
    : T_BasePage(parent), saveTimer(new QTimer(this)) {
  setupUI();
  connectSignals();
  loadStores();
}

void T_ConfigPanel::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  // Store 列表部分
  QHBoxLayout *listLayout = new QHBoxLayout();
  storeListWidget = new QListWidget(this);
  searchLineEdit = new ElaLineEdit(this);
  searchLineEdit->setPlaceholderText("Search stores...");
  listLayout->addWidget(new ElaText("Stores:", this));
  listLayout->addWidget(storeListWidget);
  listLayout->addWidget(searchLineEdit);

  // Store 编辑部分
  QHBoxLayout *editLayout = new QHBoxLayout();
  keyLineEdit = new QLineEdit(this);
  valueLineEdit = new QLineEdit(this);
  editLayout->addWidget(new ElaText("Key:", this));
  editLayout->addWidget(keyLineEdit);
  editLayout->addWidget(new ElaText("Value:", this));
  editLayout->addWidget(valueLineEdit);

  // 操作按钮部分
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  addButton = new ElaPushButton("Add/Update Store", this);
  deleteButton = new ElaPushButton("Delete Store", this);
  resetButton = new ElaPushButton("Reset Store", this);
  subscribeButton = new ElaPushButton("Subscribe", this);
  unsubscribeButton = new ElaPushButton("Unsubscribe", this);
  importButton = new ElaPushButton("Import Config", this);
  exportButton = new ElaPushButton("Export Config", this);
  batchUpdateButton = new ElaPushButton("Batch Update", this);
  saveTimerButton = new ElaPushButton("Toggle Auto Save", this);

  buttonLayout->addWidget(addButton);
  buttonLayout->addWidget(deleteButton);
  buttonLayout->addWidget(resetButton);
  buttonLayout->addWidget(subscribeButton);
  buttonLayout->addWidget(unsubscribeButton);
  buttonLayout->addWidget(importButton);
  buttonLayout->addWidget(exportButton);
  buttonLayout->addWidget(batchUpdateButton);
  buttonLayout->addWidget(saveTimerButton);

  // 状态显示部分
  statusLabel = new ElaText(this);

  mainLayout->addLayout(listLayout);
  mainLayout->addLayout(editLayout);
  mainLayout->addLayout(buttonLayout);
  mainLayout->addWidget(statusLabel);

  QWidget *centralWidget = new QWidget(this);
  centralWidget->setWindowTitle("设备连接");
  QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
  centerLayout->addLayout(mainLayout);
  centerLayout->addStretch();
  centerLayout->setContentsMargins(10, 10, 10, 10);
  addCentralWidget(centralWidget, true, true, 0);
}

void T_ConfigPanel::connectSignals() {
  connect(addButton, &ElaPushButton::clicked, this,
          &T_ConfigPanel::addOrUpdateStore);
  connect(deleteButton, &ElaPushButton::clicked, this,
          &T_ConfigPanel::deleteStore);
  connect(resetButton, &ElaPushButton::clicked, this,
          &T_ConfigPanel::resetStore);
  connect(subscribeButton, &ElaPushButton::clicked, this,
          &T_ConfigPanel::subscribeToStore);
  connect(unsubscribeButton, &ElaPushButton::clicked, this,
          &T_ConfigPanel::unsubscribeFromStore);
  connect(importButton, &ElaPushButton::clicked, this,
          &T_ConfigPanel::importConfig);
  connect(exportButton, &ElaPushButton::clicked, this,
          &T_ConfigPanel::exportConfig);
  connect(batchUpdateButton, &ElaPushButton::clicked, this,
          &T_ConfigPanel::batchUpdate);
  connect(saveTimerButton, &ElaPushButton::clicked, this,
          &T_ConfigPanel::toggleAutoSave);
  connect(storeListWidget, &QListWidget::itemSelectionChanged, this,
          &T_ConfigPanel::onStoreSelectionChanged);
  connect(searchLineEdit, &QLineEdit::textChanged, this,
          &T_ConfigPanel::searchStores);
  connect(saveTimer, &QTimer::timeout, this, &T_ConfigPanel::autoSave);
}

void T_ConfigPanel::loadStores() {
  QStringList stores = globalConfig.getStoreNames();
  storeListWidget->addItems(stores);
}

void T_ConfigPanel::refreshStoreList() {
  storeListWidget->clear();
  loadStores();
}

void T_ConfigPanel::addOrUpdateStore() {
  QString storeName = storeListWidget->currentItem()->text();
  QString key = keyLineEdit->text();
  QString value = valueLineEdit->text();
  if (storeName.isEmpty() || key.isEmpty()) {
    statusLabel->setText("Please select a store and enter a key.");
    return;
  }

  globalConfig.setState(storeName, key, value);
  statusLabel->setText("Store updated successfully.");
}

void T_ConfigPanel::deleteStore() {
  QString storeName = storeListWidget->currentItem()->text();
  if (storeName.isEmpty()) {
    statusLabel->setText("Please select a store to delete.");
    return;
  }

  globalConfig.resetStore(storeName);
  storeListWidget->takeItem(storeListWidget->currentRow());
  statusLabel->setText("Store deleted.");
}

void T_ConfigPanel::resetStore() {
  QString storeName = storeListWidget->currentItem()->text();
  if (storeName.isEmpty()) {
    statusLabel->setText("Please select a store to reset.");
    return;
  }

  globalConfig.resetStore(storeName);
  statusLabel->setText("Store reset.");
}

void T_ConfigPanel::subscribeToStore() {
  QString storeName = storeListWidget->currentItem()->text();
  if (storeName.isEmpty()) {
    statusLabel->setText("Please select a store to subscribe.");
    return;
  }

  globalConfig.subscribeToStore(
      storeName, [this, storeName](const QString &key) {
        statusLabel->setText("Store " + storeName + " changed key: " + key);
      });

  statusLabel->setText("Subscribed to store: " + storeName);
}

void T_ConfigPanel::unsubscribeFromStore() {
  QString storeName = storeListWidget->currentItem()->text();
  if (storeName.isEmpty()) {
    statusLabel->setText("Please select a store to unsubscribe.");
    return;
  }

  globalConfig.unsubscribeFromStore(storeName, this);
  statusLabel->setText("Unsubscribed from store: " + storeName);
}

void T_ConfigPanel::importConfig() {
  QString filePath = QFileDialog::getOpenFileName(this, "Import Config", "",
                                                  "JSON Files (*.json)");
  if (filePath.isEmpty()) {
    statusLabel->setText("No file selected.");
    return;
  }

  globalConfig.importConfig(filePath);
  refreshStoreList();
  statusLabel->setText("Configuration imported successfully.");
}

void T_ConfigPanel::exportConfig() {
  QString filePath = QFileDialog::getSaveFileName(this, "Export Config", "",
                                                  "JSON Files (*.json)");
  if (filePath.isEmpty()) {
    statusLabel->setText("No file selected.");
    return;
  }

  globalConfig.exportConfig(filePath);
  statusLabel->setText("Configuration exported successfully.");
}

void T_ConfigPanel::onStoreSelectionChanged() {
  QString storeName = storeListWidget->currentItem()->text();
  // Load the keys and values from the selected store (not implemented here for
  // brevity)
}

void T_ConfigPanel::searchStores() {
  QString searchText = searchLineEdit->text().trimmed();
  QStringList stores = globalConfig.getStoreNames();
  storeListWidget->clear();

  // 实现搜索功能：过滤 Store 列表
  for (const QString &store : stores) {
    if (store.contains(searchText, Qt::CaseInsensitive)) {
      storeListWidget->addItem(store);
    }
  }
}

void T_ConfigPanel::batchUpdate() {
  // 实现批量更新功能
  QString storeName = storeListWidget->currentItem()->text();
  if (storeName.isEmpty()) {
    statusLabel->setText("Please select a store to perform batch update.");
    return;
  }

  // 假设用户输入的键值对以 "key1:value1,key2:value2" 的格式
  QString batchUpdateData = valueLineEdit->text();
  QStringList keyValuePairs = batchUpdateData.split(',');

  for (const QString &pair : keyValuePairs) {
    QStringList keyValue = pair.split(':');
    if (keyValue.size() == 2) {
      QString key = keyValue[0].trimmed();
      QString value = keyValue[1].trimmed();
      globalConfig.setState(storeName, key, value);
    }
  }

  statusLabel->setText("Batch update completed.");
}

void T_ConfigPanel::toggleAutoSave() {
  // 切换定时保存功能
  if (saveTimer->isActive()) {
    saveTimer->stop();
    statusLabel->setText("Auto-save disabled.");
    saveTimerButton->setText("Enable Auto Save");
  } else {
    saveTimer->start(300000); // 每5分钟自动保存一次
    statusLabel->setText("Auto-save enabled.");
    saveTimerButton->setText("Disable Auto Save");
  }
}

void T_ConfigPanel::autoSave() {
  // 定时保存当前配置到默认路径
  globalConfig.exportConfig(QDir::homePath() + "/.myapp/auto_save_config.json");
  statusLabel->setText("Configuration auto-saved.");
}
