#include "HelpWindow.h"

#include <QFile>
#include <QMessageBox>
#include <QStringListModel>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextStream>
#include <QUrl>
#include <QtPrintSupport/QPrintDialog>


#include "Def.h"
#include "ElaComboBox.h"
#include "ElaIcon.h"
#include "ElaLineEdit.h"
#include "ElaListView.h"
#include "ElaPushButton.h"

HelpWindow::HelpWindow(QWidget *parent) : T_BasePage(parent), currentPage("") {
    setupUi();
}

HelpWindow::~HelpWindow() {}

void HelpWindow::setupUi() {
    splitter = new QSplitter(this);

    listView = new ElaListView(splitter);
    QStringList topics;
    topics << "Introduction" << "Getting Started" << "Advanced Topics";
    listModel = new QStringListModel(topics, this);
    listView->setModel(listModel);

    textBrowser = new QTextBrowser(splitter);
    textBrowser->setHtml(
        "<h1>Welcome to the Help Document</h1><p>Select a topic to begin.</p>");

    searchLineEdit = new ElaLineEdit(this);
    searchLineEdit->setPlaceholderText("Search...");
    searchButton = new ElaPushButton("Search", this);

    bookmarkButton = new ElaPushButton("Add Bookmark", this);
    backButton = new ElaPushButton("Back", this);
    forwardButton = new ElaPushButton("Forward", this);
    printButton = new ElaPushButton("Print", this);
    exportPdfButton = new ElaPushButton("Export as PDF", this);
    markButton = new ElaPushButton("Mark Document", this);

    languageComboBox = new ElaComboBox(this);
    languageComboBox->addItem("English", "en");
    languageComboBox->addItem("中文", "zh");

    bookmarkListWidget = new ElaListView(this);
    splitter->addWidget(bookmarkListWidget);

    searchLayout = new QHBoxLayout;
    searchLayout->addWidget(searchLineEdit);
    searchLayout->addWidget(searchButton);

    mainLayout = new QVBoxLayout;
    mainLayout->addLayout(searchLayout);
    mainLayout->addWidget(splitter);
    mainLayout->addWidget(bookmarkButton);
    mainLayout->addWidget(backButton);
    mainLayout->addWidget(forwardButton);
    mainLayout->addWidget(printButton);
    mainLayout->addWidget(exportPdfButton);
    mainLayout->addWidget(markButton);
    mainLayout->addWidget(languageComboBox);

    auto centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("简单序列面板");
    auto centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);

    // 连接信号槽
    connect(searchButton, &ElaPushButton::clicked, this,
            &HelpWindow::onSearchClicked);
    connect(bookmarkButton, &ElaPushButton::clicked, this,
            &HelpWindow::onBookmarkClicked);
    connect(languageComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &HelpWindow::onLanguageChanged);
    connect(backButton, &ElaPushButton::clicked, this,
            &HelpWindow::onBackClicked);
    connect(forwardButton, &ElaPushButton::clicked, this,
            &HelpWindow::onForwardClicked);
    connect(printButton, &ElaPushButton::clicked, this,
            &HelpWindow::onPrintClicked);
    connect(exportPdfButton, &ElaPushButton::clicked, this,
            &HelpWindow::onExportPdfClicked);
    connect(listView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &HelpWindow::onContentSelected);
    connect(markButton, &ElaPushButton::clicked, this,
            &HelpWindow::onMarkDocumentClicked);
}

void HelpWindow::onSearchClicked() {
    QString searchTerm = searchLineEdit->text();
    if (!searchTerm.isEmpty()) {
        onHighlightSearchResults(searchTerm);
    }
}

void HelpWindow::onBookmarkClicked() {
    QString currentTitle = listView->currentIndex().data().toString();
    bookmarkListWidget->addAction(currentTitle);
}

void HelpWindow::onLanguageChanged(int index) {
    QString lang = languageComboBox->itemData(index).toString();
    if (lang == "en") {
        onLoadMarkdownFile(":/docs/english/introduction.md");
    } else if (lang == "zh") {
        onLoadMarkdownFile(":/docs/chinese/introduction.md");
    }
}

void HelpWindow::onBackClicked() {
    if (!history.isEmpty()) {
        QString lastPage = history.pop();
        QModelIndex index =
            listModel->match(listModel->index(0, 0), Qt::DisplayRole, lastPage)
                .value(0);
        listView->setCurrentIndex(index);
    }
}

void HelpWindow::onForwardClicked() {
    // This can be implemented with a forward stack similar to the history
    // stack.
}

void HelpWindow::onPrintClicked() {
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
        textBrowser->print(&printer);
    }
}

void HelpWindow::onExportPdfClicked() {
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName("help_document.pdf");
    textBrowser->print(&printer);
}

void HelpWindow::onContentSelected(const QModelIndex &index) {
    QString text = index.data().toString();
    if (!currentPage.isEmpty() && currentPage != text) {
        history.push(currentPage);
    }
    currentPage = text;
    loadContent(text);
}

void HelpWindow::onMarkDocumentClicked() {
    QTextCursor cursor = textBrowser->textCursor();
    if (!cursor.hasSelection()) {
        QMessageBox::warning(this, "Mark Document",
                             "Please select some text to mark.");
        return;
    }
    QString markName = QString("Mark-%1").arg(bookmarks.size() + 1);
    bookmarks[markName] = cursor;  // 保存当前光标位置
    bookmarkListWidget->addAction(markName);
}

void HelpWindow::onHighlightSearchResults(const QString &searchTerm) {
    QTextDocument *document = textBrowser->document();
    QTextCursor cursor(document);

    QTextCharFormat plainFormat(cursor.charFormat());
    QTextCharFormat colorFormat = plainFormat;
    colorFormat.setBackground(Qt::yellow);

    while (!cursor.isNull() && !cursor.atEnd()) {
        cursor = document->find(searchTerm, cursor);

        if (!cursor.isNull()) {
            cursor.movePosition(QTextCursor::NoMove, QTextCursor::KeepAnchor);
            cursor.mergeCharFormat(colorFormat);
        }
    }
}

void HelpWindow::onLoadMarkdownFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Cannot load the markdown file.");
        return;
    }

    QTextStream in(&file);
    QString markdownContent = in.readAll();
    file.close();

    applyMarkdown(markdownContent);
}

void HelpWindow::loadContent(const QString &text) {
    if (text == "Introduction") {
        onLoadMarkdownFile(":/docs/english/introduction.md");
    } else if (text == "Getting Started") {
        onLoadMarkdownFile(":/docs/english/getting_started.md");
    } else if (text == "Advanced Topics") {
        onLoadMarkdownFile(":/docs/english/advanced_topics.md");
    }
}

void HelpWindow::applyMarkdown(const QString &markdown) {
    // 将Markdown转换为HTML，这里使用Qt自带的功能
    // 注意：你需要Qt 5.14+ 版本，如果你的版本较低，需使用其他Markdown解析库。
    QString htmlContent = QTextDocument().toMarkdown();
    htmlContent.append(markdown);
    textBrowser->setHtml(htmlContent);
}

void HelpWindow::highlightText(const QString &text) {
    QTextCursor cursor = textBrowser->textCursor();
    QTextCharFormat format;
    format.setBackground(Qt::yellow);

    while (!cursor.isNull() && !cursor.atEnd()) {
        cursor = textBrowser->document()->find(text, cursor);

        if (!cursor.isNull()) {
            cursor.mergeCharFormat(format);
        }
    }
}