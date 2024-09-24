#include "C_InfoCard.h"

#include <QPainter>
#include <QStyleOption>
#include <QVBoxLayout>

#include "ElaText.h"

namespace {
constexpr int MinimumHeight = 80;
constexpr int MarginSize = 10;
constexpr int TitleTextPixelSize = 18;
constexpr int ValueTextPixelSize = 16;
}  // namespace

InfoCard::InfoCard(QString title, QString value, QWidget *parent)
    : QWidget(parent), m_title(std::move(title)), m_value(std::move(value)) {
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setMinimumHeight(MinimumHeight);

    // 创建布局
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(MarginSize, MarginSize, MarginSize,
                               MarginSize);  // 设置内边距

    // 创建标题标签
    auto *titleLabel = new ElaText(m_title, this);
    titleLabel->setTextPixelSize(TitleTextPixelSize);

    // 创建值标签
    auto *valueLabel = new ElaText(m_value, this);
    valueLabel->setTextPixelSize(ValueTextPixelSize);

    // 将标签添加到布局
    layout->addWidget(titleLabel);
    layout->addWidget(valueLabel);

    // 设置布局
    setLayout(layout);
}

// 设置标题
void InfoCard::setTitle(const QString &title) {
    m_title = title;
    findChild<ElaText *>()->setText(m_title);  // 更新标题 ElaText
}

// 设置值
void InfoCard::setValue(const QString &value) {
    m_value = value;
    findChildren<ElaText *>()[1]->setText(m_value);  // 更新值 ElaText
}