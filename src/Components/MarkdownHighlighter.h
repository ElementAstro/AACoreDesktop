#ifndef MARKDOWNHIGHLIGHTER_H
#define MARKDOWNHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QVector>

class MarkdownHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    explicit MarkdownHighlighter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString& text) override;

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    
    QVector<HighlightingRule> highlightingRules;
    void setupHighlightingRules();
    void addRule(const QString& pattern, QColor color, QFont::Weight weight = QFont::Normal,
                 bool italic = false, const QString& fontFamily = "");
    void highlightMultilineCode(const QString& text);
    QTextCharFormat getMultilineCodeFormat();
};

#endif // MARKDOWNHIGHLIGHTER_H
