#ifndef T_SOFTWARE_HPP
#define T_SOFTWARE_HPP

#include "T_BasePage.h"

#include <QList>
#include <QStandardItemModel>
#include <QStringList>
#include <QWidget>

#include "Utils/Software.h"

class ElaLineEdit;
class ElaListView;
class ElaComboBox;

class T_SoftwarePage : public T_BasePage {
    Q_OBJECT

public:
    explicit T_SoftwarePage(QWidget *parent = nullptr);
    ~T_SoftwarePage() override;

private slots:
    void updateAppList();
    void showAppDetails(const QModelIndex &index);
    void onInstallButtonClicked();
    void onUninstallButtonClicked();

private:
    ElaLineEdit *searchBox;
    ElaComboBox *filterComboBox;
    ElaComboBox *sortComboBox;
    ElaListView *appListView;
    QStandardItemModel *appListModel;
    SoftwareManager *softwareManager;  // 软件管理器对象
    QList<SoftwareManager::SoftwareInfo>
        softwareData;  // 存储所有已安装的软件信息

    void loadInstalledSoftware();
    static QList<SoftwareManager::SoftwareInfo> filterSoftware(
        const QList<SoftwareManager::SoftwareInfo> &softwareList,
        const QString &searchText, const QString &filter);
    static void sortSoftware(QList<SoftwareManager::SoftwareInfo> &softwareList,
                             const QString &criteria);
};

#endif  // T_SOFTWARE_HPP