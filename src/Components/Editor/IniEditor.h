#ifndef INIEDITORWIDGET_H
#define INIEDITORWIDGET_H

#include <QStack>
#include <QStandardItemModel>
#include <QWidget>

class ElaTreeView;
class ElaPlainTextEdit;
class ElaText;

class IniEditorWidget : public QWidget {
    Q_OBJECT

public:
    explicit IniEditorWidget(QWidget *parent = nullptr);

private slots:
    void loadIni();
    void saveIni();
    void onNodeChanged(const QModelIndex &index);
    void addSection();
    void addKey();
    void deleteNode();
    void editValue();

private:
    ElaTreeView *treeView;
    ElaPlainTextEdit *plainTextEdit;
    QStandardItemModel *model;
    ElaText *detailLabel;

    void setupUi();
    void loadIniToTree(const QString &iniContent);
    QString iniToString() const;
};

#endif  // INIEDITORWIDGET_H
