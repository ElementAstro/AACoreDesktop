#ifndef PLUGINMANAGERPAGE_H
#define PLUGINMANAGERPAGE_H

#include <QSplitter>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QWidget>

class ElaLineEdit;
class ElaListView;

class PluginManagerPage : public QWidget {
    Q_OBJECT

public:
    explicit PluginManagerPage(QWidget *parent = nullptr);

private:
    ElaListView *pluginListView;      // 插件列表视图
    QStackedWidget *stackedWidget;  // 右侧的详细页面
    QSplitter *splitter;            // 用于调整左侧和右侧的布局
    QStandardItemModel *model;      // 插件列表数据模型
    ElaLineEdit *searchBox;           // 搜索框

    void setupUI();                                 // 设置界面
    void populatePlugins();                         // 填充插件列表
    void filterPlugins(const QString &filterText);  // 过滤插件列表

private slots:
    void onPluginSelected(const QModelIndex &current,
                          const QModelIndex &previous);  // 插件选择槽函数
    void onSearchTextChanged(const QString &text);  // 搜索框文本变化槽函数
};

#endif  // PLUGINMANAGERPAGE_H
