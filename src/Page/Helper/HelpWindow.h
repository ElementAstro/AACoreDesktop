#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <qcontainerfwd.h>
#include <qstandarditemmodel.h>
#include <qstringlistmodel.h>
#include "ElaListView.h"
#include "T_BasePage.h"

#include <QMap>
#include <QSplitter>
#include <QStack>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QStringList>
#include <QtPrintSupport/QPrinter>

class ElaPushButton;
class ElaComboBox;
class ElaLineEdit;

class HelpWindow : public T_BasePage {
    Q_OBJECT

public:
    HelpWindow(QWidget *parent = nullptr);
    ~HelpWindow();

private slots:
    void onSearchClicked();
    void onBookmarkClicked();
    void onLanguageChanged(int index);
    void onBackClicked();
    void onForwardClicked();
    void onPrintClicked();
    void onExportPdfClicked();
    void onContentSelected(const QModelIndex &text);
    void onMarkDocumentClicked();
    void onHighlightSearchResults(const QString &searchTerm);
    void onLoadMarkdownFile(const QString &filePath);

private:
    void setupUi();
    void loadContent(const QString &text);
    void highlightText(const QString &text);
    void applyMarkdown(const QString &markdown);

    QSplitter *splitter;
    ElaListView *listView;
    QStringListModel *listModel;
    QTextBrowser *textBrowser;
    ElaLineEdit *searchLineEdit;
    ElaPushButton *searchButton;
    ElaPushButton *bookmarkButton;
    ElaPushButton *backButton;
    ElaPushButton *forwardButton;
    ElaPushButton *printButton;
    ElaPushButton *exportPdfButton;
    ElaPushButton *markButton;
    ElaComboBox *languageComboBox;
    ElaListView *bookmarkListWidget;

    QVBoxLayout *mainLayout;
    QHBoxLayout *searchLayout;

    QStack<QString> history;
    QString currentPage;
    QMap<QString, QTextCursor> bookmarks;  // 保存文档标记
};

#endif  // HELPWINDOW_H
