#ifndef PLUGINMANAGERPAGE_H
#define PLUGINMANAGERPAGE_H

#include <QFrame>
#include <QLabel>
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
    ElaListView *pluginListView;
    QStackedWidget *stackedWidget;
    QSplitter *splitter;
    QStandardItemModel *model;
    ElaLineEdit *searchBox;
    QLabel *noSelectionLabel;

    void setupUI();
    void populatePlugins();
    void filterPlugins(const QString &filterText);

private slots:
    void onPluginSelected(const QModelIndex &current,
                          const QModelIndex &previous);
    void onSearchTextChanged(const QString &text);
};

#endif  // PLUGINMANAGERPAGE_H
