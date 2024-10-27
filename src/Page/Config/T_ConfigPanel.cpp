#include "T_ConfigPanel.h"

#include <QHeaderView>
#include <QMessageBox>

T_ConfigPanel::T_ConfigPanel(QWidget *parent)
    : T_BasePage(parent),
      m_storeListView(new ElaListView(this)),
      m_keyValueTableView(new ElaTableView(this)),
      m_saveButton(new ElaPushButton("Save", this)),
      m_resetButton(new ElaPushButton("Reset Store", this)),
      m_subscribeButton(new ElaPushButton("Subscribe", this)),
      m_importButton(new ElaPushButton("Import Config", this)),
      m_exportButton(new ElaPushButton("Export Config", this)),
      m_storeListModel(new QStringListModel(this)),
      m_keyValueModel(new QStandardItemModel(this)),
      m_currentStore("") {
    auto *mainLayout = new QVBoxLayout(this);
    auto *buttonLayout = new QHBoxLayout();

    buttonLayout->addWidget(m_importButton);
    buttonLayout->addWidget(m_exportButton);
    buttonLayout->addWidget(m_saveButton);
    buttonLayout->addWidget(m_resetButton);
    buttonLayout->addWidget(m_subscribeButton);

    mainLayout->addWidget(m_storeListView);
    mainLayout->addWidget(m_keyValueTableView);
    mainLayout->addLayout(buttonLayout);

    m_storeListView->setModel(m_storeListModel);
    m_keyValueTableView->setModel(m_keyValueModel);

    m_keyValueModel->setColumnCount(2);
    m_keyValueModel->setHorizontalHeaderLabels({"Key", "Value"});
    m_keyValueTableView->horizontalHeader()->setStretchLastSection(true);

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("配置面板");
    auto *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);

    connect(m_storeListView->selectionModel(),
            &QItemSelectionModel::selectionChanged, this,
            &T_ConfigPanel::onStoreSelected);
    connect(m_saveButton, &ElaPushButton::clicked, this,
            &T_ConfigPanel::onSaveButtonClicked);
    connect(m_resetButton, &ElaPushButton::clicked, this,
            &T_ConfigPanel::onResetStoreClicked);
    connect(m_importButton, &ElaPushButton::clicked, this,
            &T_ConfigPanel::onImportConfig);
    connect(m_exportButton, &ElaPushButton::clicked, this,
            &T_ConfigPanel::onExportConfig);
    connect(m_subscribeButton, &ElaPushButton::clicked, this,
            &T_ConfigPanel::subscribeToCurrentStore);

    loadStores();
}

void T_ConfigPanel::loadStores() {
    QStringList storeNames = globalConfig.getStoreNames();
    m_storeListModel->setStringList(storeNames);
}

void T_ConfigPanel::onStoreSelected() {
    QModelIndexList selectedIndexes =
        m_storeListView->selectionModel()->selectedIndexes();
    if (!selectedIndexes.isEmpty()) {
        m_currentStore =
            m_storeListModel->data(selectedIndexes.first(), Qt::DisplayRole)
                .toString();
        loadStoreData(m_currentStore);
    }
}

void T_ConfigPanel::loadStoreData(const QString &storeName) {
    m_keyValueModel->clear();
    m_keyValueModel->setHorizontalHeaderLabels({"Key", "Value"});

    /*
    auto store = globalConfig.useStore(storeName).value<QMap<QString, QVariant>>();
    if (store) {
        int row = 0;
        for (auto it = store->constBegin(); it != store->constEnd(); ++it) {
            auto *keyItem = new QStandardItem(it.key());
            auto *valueItem = new QStandardItem(it.value().toString());
            m_keyValueModel->setItem(row, 0, keyItem);
            m_keyValueModel->setItem(row, 1, valueItem);
            ++row;
        }
    }
    */
    
}

void T_ConfigPanel::onSaveButtonClicked() {
    if (m_currentStore.isEmpty()) {
        QMessageBox::warning(this, "Error", "No store selected.");
        return;
    }

    for (int row = 0; row < m_keyValueModel->rowCount(); ++row) {
        QString key = m_keyValueModel->item(row, 0)->text();
        QVariant value = m_keyValueModel->item(row, 1)->text();
        globalConfig.setState(m_currentStore, key, value);
    }
}

void T_ConfigPanel::onResetStoreClicked() {
    if (m_currentStore.isEmpty()) {
        QMessageBox::warning(this, "Error", "No store selected.");
        return;
    }

    globalConfig.resetStore(m_currentStore);
    loadStores();
}

void T_ConfigPanel::onImportConfig() {
    QString filePath = QFileDialog::getOpenFileName(this, "Import Config", "",
                                                    "JSON Files (*.json)");
    if (!filePath.isEmpty()) {
        globalConfig.importConfig(filePath);
        loadStores();
    }
}

void T_ConfigPanel::onExportConfig() {
    QString filePath = QFileDialog::getSaveFileName(this, "Export Config", "",
                                                    "JSON Files (*.json)");
    if (!filePath.isEmpty()) {
        globalConfig.exportConfig(filePath);
    }
}

void T_ConfigPanel::subscribeToCurrentStore() {
    if (m_currentStore.isEmpty()) {
        QMessageBox::warning(this, "Error", "No store selected.");
        return;
    }

    auto callback = std::make_shared<GlobalConfig::Callback>(
        [this](const QString &key) { loadStoreData(m_currentStore); });

    globalConfig.subscribeToStore(m_currentStore, callback);
    QMessageBox::information(this, "Subscribed",
                             "Successfully subscribed to store updates.");
}