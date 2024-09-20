#include "MarkdownHighlighter.h"

#include <QFont>

// 构造函数：初始化Markdown高亮规则
MarkdownHighlighter::MarkdownHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent) {
    setupHighlightingRules();
}

// 设置不同Markdown语法的高亮规则
void MarkdownHighlighter::setupHighlightingRules() {
    addRule(R"(^#{1,6} .*)", Qt::darkBlue, QFont::Bold);  // 标题1-6级
    addRule(R"(\*\*\*.*\*\*\*|___.*___)", Qt::darkRed, QFont::Bold,
            true);  // 粗斜体
    addRule(R"(\*\*[^*]+\*\*|__[^_]+__)", Qt::darkGreen, QFont::Bold);  // 粗体
    addRule(R"(\*[^*]+\*|_[^_]+_)", Qt::darkMagenta, QFont::Bold,
            true);  // 斜体
    addRule(R"(`[^`]*`)", Qt::darkGreen, QFont::Bold, true,
            "Courier");                             // 行内代码块
    addRule(R"(^>.*)", Qt::darkGray, QFont::Bold);  // 引用
    addRule(R"(^(-|\*|\+) .*)", Qt::darkYellow);    // 无序列表
    addRule(R"(^\d+\..*)", Qt::darkYellow);         // 有序列表
    addRule(R"(

\[.*\]\(.*\))",
            Qt::darkBlue, QFont::Bold);  // 链接
    addRule(R"(!

\[[^\]]*\]\([^)]*\))",
            Qt::darkCyan, QFont::Bold);                         // 图片
    addRule(R"(^-{3,}$|^\*{3,}$)", Qt::darkGray, QFont::Bold);  // 水平线
}

// 添加单个Markdown语法的高亮规则
void MarkdownHighlighter::addRule(const QString& pattern, QColor color,
                                  QFont::Weight weight, bool italic,
                                  const QString& fontFamily) {
    HighlightingRule rule;
    rule.pattern = QRegularExpression(pattern);
    QTextCharFormat format;
    format.setForeground(color);
    format.setFontWeight(weight);
    format.setFontItalic(italic);
    if (!fontFamily.isEmpty()) {
        format.setFontFamily(fontFamily);
    }
    rule.format = format;
    highlightingRules.append(rule);
}

// 高亮文本块，应用Markdown高亮规则
void MarkdownHighlighter::highlightBlock(const QString& text) {
    for (const HighlightingRule& rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator =
            rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(),
                      rule.format);
        }
    }
    highlightMultilineCode(text);
}

// 高亮多行代码块
void MarkdownHighlighter::highlightMultilineCode(const QString& text) {
    static QRegularExpression startPattern(R"(^```)");
    static QRegularExpression endPattern(R"(^```)");
    static bool isInsideCodeBlock = false;

    if (isInsideCodeBlock) {
        setFormat(0, text.length(), getMultilineCodeFormat());
        if (endPattern.match(text).hasMatch()) {
            isInsideCodeBlock = false;
        }
    } else if (startPattern.match(text).hasMatch()) {
        setFormat(0, text.length(), getMultilineCodeFormat());
        isInsideCodeBlock = true;
    }
}

// 返回多行代码块的格式
QTextCharFormat MarkdownHighlighter::getMultilineCodeFormat() {
    QTextCharFormat codeFormat;
    codeFormat.setForeground(Qt::darkGreen);
    codeFormat.setFontFamily("Courier");
    return codeFormat;
}
