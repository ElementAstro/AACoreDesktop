#include "PluginManagerPage.h"
#include "PluginPage.h"

#include <QVBoxLayout>

#include "ElaLineEdit.h"
#include "ElaListView.h"

PluginManagerPage::PluginManagerPage(QWidget *parent) : QWidget(parent) {
    setupUI();          // 设置界面
    populatePlugins();  // 填充插件列表
}

void PluginManagerPage::setupUI() {
    // 初始化 QSplitter（用于左右布局）
    splitter = new QSplitter(Qt::Horizontal, this);

    // 创建插件列表
    pluginListView = new ElaListView(this);
    model = new QStandardItemModel(this);
    pluginListView->setModel(model);

    // 创建右侧详细信息视图
    stackedWidget = new QStackedWidget(this);

    // 搜索框
    searchBox = new ElaLineEdit(this);
    searchBox->setPlaceholderText("Search Plugins...");
    connect(searchBox, &QLineEdit::textChanged, this,
            &PluginManagerPage::onSearchTextChanged);

    // 将插件列表和详细信息页面添加到 QSplitter
    splitter->addWidget(pluginListView);  // 左侧插件列表
    splitter->addWidget(stackedWidget);   // 右侧详情视图
    splitter->setStretchFactor(0, 1);     // 左侧占 1/4 宽度
    splitter->setStretchFactor(1, 3);     // 右侧占 3/4 宽度

    // 布局管理器
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(searchBox);  // 搜索框在顶部
    layout->addWidget(splitter);   // splitter 在下方

    setLayout(layout);  // 设置布局

    // 连接插件选择信号和槽函数
    connect(pluginListView->selectionModel(),
            &QItemSelectionModel::currentChanged, this,
            &PluginManagerPage::onPluginSelected);
}

void PluginManagerPage::populatePlugins() {
    // 添加插件列表项
    QList<QStandardItem *> pluginItems;
    pluginItems.append(new QStandardItem("10 Micron Tools"));
    pluginItems.append(new QStandardItem("ASA Tools"));
    pluginItems.append(new QStandardItem("Astro-Physics Tools"));
    pluginItems.append(new QStandardItem("Autofocus Report Analysis"));
    pluginItems.append(new QStandardItem("Connector"));

    // 将插件添加到模型
    for (auto &item : pluginItems) {
        model->appendRow(item);
    }

    // 为不同的插件创建详细页面并添加到 QStackedWidget
    stackedWidget->addWidget(
        new PluginPage("10 Micron Tools", "George Hilios", "2.1.0.2",
                       "10 Micron Mount Tools, including model building",
                       QStringList() << "DependencyA" << "DependencyB"));
    stackedWidget->addWidget(new PluginPage("ASA Tools", "Author Name", "1.0.0",
                                            "Tools for ASA mounts",
                                            QStringList() << "DependencyX"));
    stackedWidget->addWidget(
        new PluginPage("Astro-Physics Tools", "Another Author", "3.2.1",
                       "Tools for Astro-Physics mounts",
                       QStringList() << "DependencyY" << "DependencyZ"));
    stackedWidget->addWidget(
        new PluginPage("Autofocus Report Analysis", "AuthorX", "1.5.0",
                       "Tools for analyzing autofocus reports", QStringList()));
    stackedWidget->addWidget(
        new PluginPage("Connector", "AuthorZ", "0.9.1",
                       "Generic connector for various plugins", QStringList()));
}

void PluginManagerPage::onPluginSelected(const QModelIndex &current,
                                         const QModelIndex &previous) {
    Q_UNUSED(previous);

    // 获取当前选中的行号
    int index = current.row();

    // 检查索引是否有效
    if (index >= 0 && index < stackedWidget->count()) {
        // 切换到对应的插件详细页面
        stackedWidget->setCurrentIndex(index);
    }
}

// 处理搜索框文本变化
void PluginManagerPage::onSearchTextChanged(const QString &text) {
    filterPlugins(text);
}

// 过滤插件列表
void PluginManagerPage::filterPlugins(const QString &filterText) {
    // 遍历插件列表并隐藏不匹配的插件
    for (int i = 0; i < model->rowCount(); ++i) {
        QStandardItem *pluginItem = model->item(i);
        if (pluginItem) {
            bool match =
                pluginItem->text().toLower().contains(filterText.toLower());
            pluginListView->setRowHidden(i, !match);  // 隐藏不匹配的项
        }
    }
}