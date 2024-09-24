#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include "T_BasePage.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QVBoxLayout>
#include <QWidget>

#include "ElaListView.h"
#include "ElaPushButton.h"
#include "ElaTableView.h"


#include "Config/GlobalConfig.h"

class T_ConfigPanel : public T_BasePage {
    Q_OBJECT

        public : T_ConfigPanel(QWidget *parent = nullptr);
    ~T_ConfigPanel() override = default;

private slots:
    void loadStores();
    void onStoreSelected();
    void onSaveButtonClicked();
    void onResetStoreClicked();
    void onImportConfig();
    void onExportConfig();
    void subscribeToCurrentStore();

private:
    void loadStoreData(const QString &storeName);
    void updateState(const QString &storeName, const QString &key,
                     const QVariant &value);

    ElaListView *m_storeListView;
    ElaTableView *m_keyValueTableView;
    ElaPushButton *m_saveButton;
    ElaPushButton *m_resetButton;
    ElaPushButton *m_subscribeButton;
    ElaPushButton *m_importButton;
    ElaPushButton *m_exportButton;

    QStringListModel *m_storeListModel;
    QStandardItemModel *m_keyValueModel;

    QString m_currentStore;
};

#endif  // CONFIGWIDGET_H
