#ifndef JSONEDITORWIDGET_H
#define JSONEDITORWIDGET_H

#include <QStack>
#include <QWidget>

class ElaTreeView;
class ElaPlainTextEdit;
class QStandardItemModel;
class QStandardItem;
class ElaText;

class JsonEditorWidget : public QWidget {
    Q_OBJECT

public:
    explicit JsonEditorWidget(QWidget *parent = nullptr);

private slots:
    void loadJson();
    void saveJson();
    void beautifyJson();
    void compressJson();
    void undo();
    void redo();
    void onNodeChanged(const QModelIndex &index);
    void changeNodeColor();
    void importCsv();
    void exportCsv();

private:
    ElaTreeView *treeView;
    ElaPlainTextEdit *plainTextEdit;
    QStandardItemModel *model;
    QStack<QByteArray> undoStack;
    ElaText *detailLabel;

    void setupUi();
    void loadJsonToTree(const QJsonObject &jsonObject,
                        QStandardItem *parentItem = nullptr);
    void loadJsonArrayToTree(const QJsonArray &jsonArray,
                             QStandardItem *parentItem);
    QVariant treeModelToVariant(QStandardItemModel *model);
};

#endif  // JSONEDITORWIDGET_H
