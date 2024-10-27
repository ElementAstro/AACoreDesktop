#include "PluginManagerPage.h"
#include "PluginPage.h"

#include <QFrame>
#include <QVBoxLayout>

#include "ElaLineEdit.h"
#include "ElaListView.h"


PluginManagerPage::PluginManagerPage(QWidget *parent) : QWidget(parent) {
    setupUI();
    populatePlugins();
}

void PluginManagerPage::setupUI() {
    splitter = new QSplitter(Qt::Horizontal, this);

    pluginListView = new ElaListView(this);
    model = new QStandardItemModel(this);
    pluginListView->setModel(model);

    stackedWidget = new QStackedWidget(this);

    searchBox = new ElaLineEdit(this);
    searchBox->setPlaceholderText("Search Plugins...");
    connect(searchBox, &QLineEdit::textChanged, this,
            &PluginManagerPage::onSearchTextChanged);

    // Add a dividing line for visual separation
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    splitter->addWidget(pluginListView);
    splitter->addWidget(stackedWidget);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(searchBox);
    layout->addWidget(line);  // Add dividing line under search box
    layout->addWidget(splitter);
    setLayout(layout);

    // Add a default no-selection label in the stacked widget
    noSelectionLabel =
        new QLabel("Please select a plugin to view details.", this);
    noSelectionLabel->setAlignment(Qt::AlignCenter);
    stackedWidget->addWidget(noSelectionLabel);

    connect(pluginListView->selectionModel(),
            &QItemSelectionModel::currentChanged, this,
            &PluginManagerPage::onPluginSelected);
}

void PluginManagerPage::populatePlugins() {
    QList<QStandardItem *> pluginItems;
    pluginItems.append(new QStandardItem("10 Micron Tools"));
    pluginItems.append(new QStandardItem("ASA Tools"));
    pluginItems.append(new QStandardItem("Astro-Physics Tools"));
    pluginItems.append(new QStandardItem("Autofocus Report Analysis"));
    pluginItems.append(new QStandardItem("Connector"));

    for (auto &item : pluginItems) {
        model->appendRow(item);
    }

    // Create plugin pages dynamically
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

    // Select the first item by default
    if (model->rowCount() > 0) {
        pluginListView->setCurrentIndex(model->index(0, 0));
    }
}

void PluginManagerPage::onPluginSelected(const QModelIndex &current,
                                         const QModelIndex &previous) {
    Q_UNUSED(previous);

    int index = current.row();
    if (index >= 0 && index < stackedWidget->count()) {
        stackedWidget->setCurrentIndex(index +
                                       1);  // Adjust for no-selection label
    } else {
        stackedWidget->setCurrentWidget(noSelectionLabel);
    }
}

void PluginManagerPage::onSearchTextChanged(const QString &text) {
    filterPlugins(text);
}

void PluginManagerPage::filterPlugins(const QString &filterText) {
    bool anyVisible = false;
    for (int i = 0; i < model->rowCount(); ++i) {
        QStandardItem *pluginItem = model->item(i);
        if (pluginItem) {
            bool match =
                pluginItem->text().toLower().contains(filterText.toLower());
            pluginListView->setRowHidden(i, !match);
            anyVisible |= match;
        }
    }
    if (!anyVisible) {
        stackedWidget->setCurrentWidget(noSelectionLabel);
    } else if (pluginListView->selectionModel()->currentIndex().isValid()) {
        onPluginSelected(pluginListView->selectionModel()->currentIndex(),
                         QModelIndex());
    }
}