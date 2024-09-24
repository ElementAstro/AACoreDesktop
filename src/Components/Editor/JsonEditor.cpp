#include "JsonEditor.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QTextStream>
#include <QVBoxLayout>

#include "ElaColorDialog.h"
#include "ElaPlainTextEdit.h"
#include "ElaText.h"
#include "ElaToolBar.h"
#include "ElaTreeView.h"

namespace {
constexpr int DefaultTextPixelSize = 15;
}

JsonEditorWidget::JsonEditorWidget(QWidget *parent)
    : QWidget(parent),
      treeView(new ElaTreeView(this)),
      plainTextEdit(new ElaPlainTextEdit(this)),
      model(new QStandardItemModel(this)),
      detailLabel(new ElaText(this)) {
    setupUi();
}

void JsonEditorWidget::setupUi() {
    auto *layout = new QVBoxLayout(this);

    treeView->setModel(model);
    layout->addWidget(treeView);

    connect(treeView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &JsonEditorWidget::onNodeChanged);

    plainTextEdit->setPlainText("Empty");
    layout->addWidget(plainTextEdit);

    detailLabel->setTextPixelSize(DefaultTextPixelSize);
    layout->addWidget(detailLabel);

    auto *toolbar = new ElaToolBar(this);
    layout->addWidget(toolbar);  // Add toolbar to the layout

    toolbar->addAction("Load JSON", this, &JsonEditorWidget::loadJson);
    toolbar->addAction("Save JSON", this, &JsonEditorWidget::saveJson);
    toolbar->addAction("Beautify JSON", this, &JsonEditorWidget::beautifyJson);
    toolbar->addAction("Compress JSON", this, &JsonEditorWidget::compressJson);
    toolbar->addAction("Undo", this, &JsonEditorWidget::undo);
    toolbar->addAction("Redo", this, &JsonEditorWidget::redo);
    toolbar->addAction("Change Node Color", this,
                       &JsonEditorWidget::changeNodeColor);
    toolbar->addAction("Import CSV", this, &JsonEditorWidget::importCsv);
    toolbar->addAction("Export CSV", this, &JsonEditorWidget::exportCsv);
}

void JsonEditorWidget::loadJson() {
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Open JSON File"), "", tr("JSON Files (*.json)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray data = file.readAll();
            file.close();

            loadJsonToTree(QJsonDocument::fromJson(data).object());
        }
    }
}

void JsonEditorWidget::saveJson() {
    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Save JSON File"), "", tr("JSON Files (*.json)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            QByteArray jsonData =
                QJsonDocument::fromVariant(treeModelToVariant(model)).toJson();
            file.write(jsonData);
            file.close();
        }
    }
}

void JsonEditorWidget::beautifyJson() {
    QString jsonText = plainTextEdit->toPlainText();
    QJsonDocument document = QJsonDocument::fromJson(jsonText.toUtf8());
    plainTextEdit->setPlainText(document.toJson(QJsonDocument::Indented));
}

void JsonEditorWidget::compressJson() {
    QString jsonText = plainTextEdit->toPlainText();
    QJsonDocument document = QJsonDocument::fromJson(jsonText.toUtf8());
    plainTextEdit->setPlainText(document.toJson(QJsonDocument::Compact));
}

void JsonEditorWidget::undo() {
    if (!undoStack.isEmpty()) {
        QJsonDocument doc = QJsonDocument::fromJson(undoStack.pop());
        loadJsonToTree(doc.object());
    } else {
        QMessageBox::information(this, "Info", "Nothing to undo");
    }
}

void JsonEditorWidget::redo() {
    // Redo functionality can be implemented here
}

void JsonEditorWidget::onNodeChanged(const QModelIndex &index) {
    if (!index.isValid()) {
        return;
    }
    QStandardItem *item = model->itemFromIndex(index);
    detailLabel->setText(QString("Node: %1\nValue: %2")
                             .arg(item->text())
                             .arg(item->data().toString()));
}

void JsonEditorWidget::changeNodeColor() {
    QStandardItem *selectedItem =
        model->itemFromIndex(treeView->currentIndex());
    if (!selectedItem) {
        return;
    }

    ElaColorDialog colorDialog(this);
    if (colorDialog.exec() == QDialog::Accepted) {
        QColor color = colorDialog.getCurrentColor();
        if (color.isValid()) {
            selectedItem->setBackground(color);
        }
    }
}

void JsonEditorWidget::importCsv() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Import CSV"), "",
                                                    tr("CSV Files (*.csv)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream inputStream(&file);
            while (!inputStream.atEnd()) {
                QString line = inputStream.readLine();
                QStringList fields = line.split(",");
                // Assuming the first field is the key
                if (!fields.isEmpty()) {
                    auto *item = new QStandardItem(fields[0]);
                    model->invisibleRootItem()->appendRow(item);
                }
            }
            file.close();
        }
    }
}

void JsonEditorWidget::exportCsv() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export CSV"), "",
                                                    tr("CSV Files (*.csv)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream outputStream(&file);
            // Exporting the model to CSV
            for (int i = 0; i < model->rowCount(); ++i) {
                QStandardItem *item = model->item(i);
                outputStream << item->text() << "\n";
            }
            file.close();
        }
    }
}

void JsonEditorWidget::loadJsonToTree(const QJsonObject &jsonObject,
                                      QStandardItem *parentItem) {
    model->clear();  // Clear existing model
    for (const QString &key : jsonObject.keys()) {
        auto *item = new QStandardItem(key);
        if (parentItem != nullptr) {
            parentItem->appendRow(item);
        } else {
            model->appendRow(item);
        }
        QVariant value = jsonObject.value(key).toVariant();
        if (value.canConvert<QJsonObject>()) {
            loadJsonToTree(value.toJsonObject(), item);
        } else if (value.canConvert<QJsonArray>()) {
            loadJsonArrayToTree(value.toJsonArray(), item);
        } else {
            item->setData(value);
        }
    }
}

void JsonEditorWidget::loadJsonArrayToTree(const QJsonArray &jsonArray,
                                           QStandardItem *parentItem) {
    for (int i = 0; i < jsonArray.size(); ++i) {
        auto *item = new QStandardItem(QString("[%1]").arg(i));
        parentItem->appendRow(item);
        QVariant value = jsonArray.at(i).toVariant();
        if (value.canConvert<QJsonObject>()) {
            loadJsonToTree(value.toJsonObject(), item);
        } else if (value.canConvert<QJsonArray>()) {
            loadJsonArrayToTree(value.toJsonArray(), item);
        } else {
            item->setData(value);
        }
    }
}

auto JsonEditorWidget::treeModelToVariant(QStandardItemModel *model)
    -> QVariant {
    QVariantMap jsonObj;

    for (int i = 0; i < model->rowCount(); ++i) {
        QStandardItem *item = model->item(i);
        jsonObj[item->text()] = item->data();
    }
    return jsonObj;
}