#include "IniEditor.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QStandardItem>
#include <QString>
#include <QTextStream>
#include <QVBoxLayout>

#include "ElaPlainTextEdit.h"
#include "ElaText.h"
#include "ElaToolBar.h"
#include "ElaTreeView.h"

namespace {
constexpr int DefaultTextPixelSize = 15;
}

IniEditorWidget::IniEditorWidget(QWidget *parent)
    : QWidget(parent),
      treeView(new ElaTreeView(this)),
      plainTextEdit(new ElaPlainTextEdit(this)),
      model(new QStandardItemModel(this)),
      detailLabel(new ElaText(this)) {
    setupUi();
}

void IniEditorWidget::setupUi() {
    auto *layout = new QVBoxLayout(this);

    treeView->setModel(model);
    layout->addWidget(treeView);

    connect(treeView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &IniEditorWidget::onNodeChanged);

    plainTextEdit->setPlainText("Empty");
    layout->addWidget(plainTextEdit);

    detailLabel->setTextPixelSize(DefaultTextPixelSize);
    layout->addWidget(detailLabel);

    auto *toolbar = new ElaToolBar(this);
    layout->addWidget(toolbar);

    toolbar->addAction("Load INI", this, &IniEditorWidget::loadIni);
    toolbar->addAction("Save INI", this, &IniEditorWidget::saveIni);
    toolbar->addAction("Add Section", this, &IniEditorWidget::addSection);
    toolbar->addAction("Add Key", this, &IniEditorWidget::addKey);
    toolbar->addAction("Delete Node", this, &IniEditorWidget::deleteNode);
    toolbar->addAction("Edit Value", this, &IniEditorWidget::editValue);
}

void IniEditorWidget::loadIni() {
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Open INI File"), "", tr("INI Files (*.ini)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString data = file.readAll();
            file.close();
            loadIniToTree(data);
        }
    }
}

void IniEditorWidget::saveIni() {
    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Save INI File"), "", tr("INI Files (*.ini)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QByteArray iniData = iniToString().toUtf8();
            file.write(iniData);
            file.close();
        }
    }
}

void IniEditorWidget::loadIniToTree(const QString &iniContent) {
    model->clear();

    QStringList lines = iniContent.split('\n', Qt::SkipEmptyParts);
    QStandardItem *currentSection = nullptr;

    for (const QString &line : lines) {
        if (line.startsWith('[') && line.endsWith(']')) {
            QString sectionName = line.mid(1, line.length() - 2).trimmed();
            currentSection = new QStandardItem(sectionName);
            model->appendRow(currentSection);
        } else {
            QStringList keyValue = line.split('=', Qt::SkipEmptyParts);
            if (keyValue.size() == 2 && currentSection != nullptr) {
                QString key = keyValue[0].trimmed();
                QString value = keyValue[1].trimmed();
                auto *item = new QStandardItem(key);
                item->setData(value);
                currentSection->appendRow(item);
            }
        }
    }
}

auto IniEditorWidget::iniToString() const -> QString {
    QStringList lines;
    for (int i = 0; i < model->rowCount(); ++i) {
        QStandardItem *sectionItem = model->item(i);
        lines.append(QString("[%1]").arg(sectionItem->text()));
        for (int j = 0; j < sectionItem->rowCount(); ++j) {
            QStandardItem *keyItem = sectionItem->child(j);
            lines.append(QString("%1=%2")
                             .arg(keyItem->text())
                             .arg(keyItem->data().toString()));
        }
    }
    return lines.join("\n");
}

void IniEditorWidget::onNodeChanged(const QModelIndex &index) {
    if (!index.isValid()) {
        return;
    }
    QStandardItem *item = model->itemFromIndex(index);
    detailLabel->setText(QString("Node: %1\nValue: %2")
                             .arg(item->text())
                             .arg(item->data().toString()));
}

void IniEditorWidget::addSection() {
    bool isConfirmed;
    QString sectionName =
        QInputDialog::getText(this, tr("Add Section"), tr("Section name:"),
                              QLineEdit::Normal, "", &isConfirmed);
    if (isConfirmed && !sectionName.isEmpty()) {
        auto *item = new QStandardItem(sectionName);
        model->appendRow(item);
    }
}

void IniEditorWidget::addKey() {
    QModelIndex index = treeView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    QStandardItem *sectionItem = model->itemFromIndex(index);

    bool isConfirmed;
    QString keyName =
        QInputDialog::getText(this, tr("Add Key"), tr("Key name:"),
                              QLineEdit::Normal, "", &isConfirmed);
    if (isConfirmed && !keyName.isEmpty()) {
        auto *item = new QStandardItem(keyName);
        sectionItem->appendRow(item);
    }
}

void IniEditorWidget::deleteNode() {
    QModelIndex index = treeView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    QStandardItem *item = model->itemFromIndex(index);
    if (item->parent() != nullptr) {
        item->parent()->removeRow(item->row());
    } else {
        model->removeRow(item->row());
    }
}

void IniEditorWidget::editValue() {
    QModelIndex index = treeView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    QStandardItem *item = model->itemFromIndex(index);

    bool isConfirmed;
    QString newValue = QInputDialog::getText(
        this, tr("Edit Value"), tr("New value:"), QLineEdit::Normal,
        item->data().toString(), &isConfirmed);
    if (isConfirmed) {
        item->setData(newValue);
    }
}