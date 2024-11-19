#include "InfoWidget.h"

#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QVBoxLayout>

#include "ElaText.h"

InfoWidget::InfoWidget(QWidget *parent)
    : QWidget(parent),
      m_valueLabel(new ElaText("0.0")),
      m_unitLabel(new ElaText("Unit")),
      m_nameLabel(new ElaText("Name")),
      m_rangeLabel(new ElaText("Range: [0, 100]")),
      m_minValue(0.0),
      m_maxValue(100.0),
      m_currentValue(0.0),
      m_warningThreshold(80.0),
      m_animationDuration(300) {
    setupUI();
    setupAnimations();

    // 默认颜色设置
    m_normalColor = QColor("#2ecc71");
    m_warningColor = QColor("#f1c40f");
    m_errorColor = QColor("#e74c3c");

    // 添加阴影效果
    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 50));
    shadow->setOffset(0, 2);
    setGraphicsEffect(shadow);

    setMinimumSize(300, 180);
}

InfoWidget::~InfoWidget() {
    delete m_valueLabel;
    delete m_unitLabel;
    delete m_nameLabel;
    delete m_rangeLabel;
    delete m_valueAnimation;
}

void InfoWidget::setupUI() {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(10);

    // 名称布局
    auto *nameLayout = new QHBoxLayout;
    m_nameLabel->setAlignment(Qt::AlignLeft);
    nameLayout->addWidget(m_nameLabel);
    nameLayout->addStretch();

    // 值和单位布局
    auto *valueLayout = new QHBoxLayout;
    m_valueLabel->setAlignment(Qt::AlignRight);
    valueLayout->addWidget(m_valueLabel);
    valueLayout->addWidget(m_unitLabel);

    mainLayout->addLayout(nameLayout);
    mainLayout->addLayout(valueLayout);
    mainLayout->addWidget(m_rangeLabel);

    // 默认字体大小
    setFontSizes(14, 24, 14, 12);
}

void InfoWidget::setupAnimations() {
    m_valueAnimation = new QPropertyAnimation(this, "currentValue", this);
    m_valueAnimation->setDuration(m_animationDuration);
    m_valueAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

void InfoWidget::updateValue(double value) {
    m_valueAnimation->stop();
    m_valueAnimation->setStartValue(m_currentValue);
    m_valueAnimation->setEndValue(value);
    m_valueAnimation->start();
}

void InfoWidget::updateValueExternally(double value) {
    m_currentValue = value;
    refreshDisplay();
}

void InfoWidget::refreshDisplay() {
    m_valueLabel->setText(QString::number(m_currentValue, 'f', 1));

    QColor textColor;
    if (m_currentValue > m_maxValue) {
        textColor = m_errorColor;
    } else if (m_currentValue > m_warningThreshold) {
        textColor = m_warningColor;
    } else {
        textColor = m_normalColor;
    }

    m_valueLabel->setStyleSheet(QString("color: %1").arg(textColor.name()));
}


auto InfoWidget::getCurrentValue() const -> double {
    return m_currentValue;
}

auto InfoWidget::setCurrentValue(double value) -> void {
    m_currentValue = value;
    refreshDisplay();
}

auto InfoWidget::setFontSizes(int nameSize, int valueSize, int unitSize, int rangeSize) -> void {
    m_nameLabel->setTextPixelSize(nameSize);
    m_valueLabel->setTextPixelSize(valueSize);
    m_unitLabel->setTextPixelSize(unitSize);
    m_rangeLabel->setTextPixelSize(rangeSize);
}