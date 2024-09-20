#include "C_InfoCard.h"

#include <QPainter>
#include <QStyleOption>
#include <QVBoxLayout>

#include "ElaText.h"

InfoCard::InfoCard(const QString &title, const QString &value, QWidget *parent)
    : QWidget(parent), m_title(title), m_value(value) {
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  setMinimumHeight(80);

  // 创建布局
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(10, 10, 10, 10); // 设置内边距

  // 创建标题标签
  ElaText *titleLabel = new ElaText(m_title, this);
  titleLabel->setTextPixelSize(18);

  // 创建值标签
  ElaText *valueLabel = new ElaText(m_value, this);
  valueLabel->setTextPixelSize(16);

  // 将标签添加到布局
  layout->addWidget(titleLabel);
  layout->addWidget(valueLabel);

  // 设置布局
  setLayout(layout);
}

// 设置标题
void InfoCard::setTitle(const QString &title) {
  m_title = title;
  findChild<ElaText *>()->setText(m_title); // 更新标题 ElaText
}

// 设置值
void InfoCard::setValue(const QString &value) {
  m_value = value;
  findChildren<ElaText *>()[1]->setText(m_value); // 更新值 ElaText
}