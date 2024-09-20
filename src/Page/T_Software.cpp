#include "T_Software.hpp"
#include "Utils/Software.h"

#include <QApplication>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QWidget>

#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaListView.h"
#include "ElaPushButton.h"
#include "ElaText.h"


T_SoftwarePage::T_SoftwarePage(QWidget *parent) : T_BasePage(parent) {
    // 创建搜索框和标签
    auto *searchLabel = new ElaText("搜索应用:", this);
    searchLabel->setTextPixelSize(15);
    searchBox = new ElaLineEdit(this);

    // 创建过滤条件组合框
    auto *filterLabel = new ElaText("筛选条件:", this);
    filterLabel->setTextPixelSize(15);
    filterComboBox = new ElaComboBox(this);
    filterComboBox->addItems({"所有应用", "版本 >= 1.0", "安装位置存在"});

    // 创建排序规则组合框
    auto *sortLabel = new ElaText("排序依据:", this);
    sortLabel->setTextPixelSize(15);
    sortComboBox = new ElaComboBox(this);
    sortComboBox->addItems({"名称 (A 到 Z)", "安装日期 (最新)"});

    // 创建应用列表视图
    appListView = new ElaListView(this);
    appListModel = new QStandardItemModel(this);
    appListView->setModel(appListModel);

    // 初始化软件管理器并加载软件数据
    softwareManager = new SoftwareManager();
    loadInstalledSoftware();

    // 创建安装和卸载按钮
    auto *installButton = new ElaPushButton("安装", this);
    auto *uninstallButton = new ElaPushButton("卸载", this);

    // 创建主布局并添加控件
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(searchLabel);
    mainLayout->addWidget(searchBox);
    mainLayout->addWidget(filterLabel);
    mainLayout->addWidget(filterComboBox);
    mainLayout->addWidget(sortLabel);
    mainLayout->addWidget(sortComboBox);
    mainLayout->addWidget(appListView);
    mainLayout->addWidget(installButton);
    mainLayout->addWidget(uninstallButton);

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("软件管理");
    auto *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);

    // 连接信号和槽
    connect(searchBox, &ElaLineEdit::textChanged, this,
            &T_SoftwarePage::updateAppList);
    connect(filterComboBox, &ElaComboBox::currentTextChanged, this,
            &T_SoftwarePage::updateAppList);
    connect(sortComboBox, &ElaComboBox::currentTextChanged, this,
            &T_SoftwarePage::updateAppList);
    connect(appListView, &ElaListView::clicked, this,
            &T_SoftwarePage::showAppDetails);
    connect(installButton, &ElaPushButton::clicked, this,
            &T_SoftwarePage::onInstallButtonClicked);
    connect(uninstallButton, &ElaPushButton::clicked, this,
            &T_SoftwarePage::onUninstallButtonClicked);
}

T_SoftwarePage::~T_SoftwarePage() { delete softwareManager; }

void T_SoftwarePage::loadInstalledSoftware() {
    softwareData = softwareManager->getAllInstalledSoftware();
    updateAppList();
}

void T_SoftwarePage::updateAppList() {
    QString searchText = searchBox->text();
    QString filter = filterComboBox->currentText();
    QString sortCriteria = sortComboBox->currentText();

    // 筛选软件列表
    QList<SoftwareManager::SoftwareInfo> filteredSoftware =
        filterSoftware(softwareData, filter, searchText);

    // 排序软件列表
    sortSoftware(filteredSoftware, sortCriteria);

    // 更新列表显示
    appListModel->clear();
    for (const auto &software : filteredSoftware) {
        QString itemText =
            QString("%1\n版本: %2\n安装位置: %3\n发布者: %4\n安装日期: %5")
                .arg(software.displayName)
                .arg(software.displayVersion)
                .arg(software.installLocation.isEmpty()
                         ? "未知"
                         : software.installLocation)
                .arg(software.publisher)
                .arg(software.installDate);
        QStandardItem *item = new QStandardItem(itemText);
        appListModel->appendRow(item);
    }
}

QList<SoftwareManager::SoftwareInfo> T_SoftwarePage::filterSoftware(
    const QList<SoftwareManager::SoftwareInfo> &softwareList,
    const QString &searchText, const QString &filter) {
    QList<SoftwareManager::SoftwareInfo> result;
    for (const auto &software : softwareList) {
        bool matchesFilter = true;
        if (filter == "版本 >= 1.0" &&
            software.displayVersion.toDouble() < 1.0) {
            matchesFilter = false;
        }
        if (filter == "安装位置存在" && software.installLocation.isEmpty()) {
            matchesFilter = false;
        }

        bool matchesSearch =
            software.displayName.contains(searchText, Qt::CaseInsensitive);

        if (matchesFilter && matchesSearch) {
            result.append(software);
        }
    }
    return result;
}

void T_SoftwarePage::sortSoftware(
    QList<SoftwareManager::SoftwareInfo> &softwareList,
    const QString &criteria) {
    if (criteria == "名称 (A 到 Z)") {
        std::sort(softwareList.begin(), softwareList.end(),
                  [](const SoftwareManager::SoftwareInfo &softwareA,
                     const SoftwareManager::SoftwareInfo &softwareB) {
                      return softwareA.displayName < softwareB.displayName;
                  });
    } else if (criteria == "安装日期 (最新)") {
        std::sort(softwareList.begin(), softwareList.end(),
                  [](const SoftwareManager::SoftwareInfo &softwareA,
                     const SoftwareManager::SoftwareInfo &softwareB) {
                      return softwareA.installDate > softwareB.installDate;
                  });
    }
}

void T_SoftwarePage::showAppDetails(const QModelIndex &index) {
    QString displayName =
        appListModel->itemFromIndex(index)->text().split("\n")[0];
    if (softwareManager->isSoftwareInstalled(displayName)) {
        SoftwareManager::SoftwareInfo software =
            softwareManager->getSoftwareInfo(displayName);
        QString details =
            QString(
                "名称: %1\n版本: %2\n安装位置: %3\n发布者: %4\n安装日期: %5")
                .arg(software.displayName)
                .arg(software.displayVersion)
                .arg(software.installLocation.isEmpty()
                         ? "未知"
                         : software.installLocation)
                .arg(software.publisher)
                .arg(software.installDate);
        QMessageBox::information(this, "软件详情", details);
    } else {
        QMessageBox::warning(this, "软件详情", "无法找到软件信息。");
    }
}

void T_SoftwarePage::onInstallButtonClicked() {
    // 安装软件的逻辑
    QMessageBox::information(this, "安装", "安装软件功能尚未实现。");
}

void T_SoftwarePage::onUninstallButtonClicked() {
    // 卸载软件的逻辑
    QMessageBox::information(this, "卸载", "卸载软件功能尚未实现。");
}