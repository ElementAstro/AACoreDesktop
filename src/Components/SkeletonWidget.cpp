#include "SkeletonWidget.h"
#include <QLinearGradient>
#include <QPainter>

SkeletonWidget::SkeletonWidget(QWidget *parent)
    : QWidget(parent), m_animationStep(0) {
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, [this]() {
        m_animationStep = (m_animationStep + 10) % (width() + width() / 4);
        update();
    });
    m_animationTimer->start(30);  // 每30ms更新一次动画
}

void SkeletonWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制背景
    QColor baseColor(230, 230, 230);  // 基础灰色
    painter.fillRect(rect(), baseColor);

    // 创建渐变效果
    QLinearGradient gradient(QPointF(m_animationStep, 0),
                             QPointF(m_animationStep + width() / 4, 0));
    gradient.setColorAt(0.0, QColor(255, 255, 255, 150));
    gradient.setColorAt(0.5, QColor(255, 255, 255, 220));
    gradient.setColorAt(1.0, QColor(255, 255, 255, 150));

    // 模拟文本占位符
    QRectF textRect(20, 20, width() - 40, 20);
    painter.fillRect(textRect, baseColor.darker(110));
    painter.fillRect(QRectF(textRect.x(), textRect.y(), textRect.width() * 0.5,
                            textRect.height()),
                     gradient);

    // 模拟图片占位符
    QRectF imageRect(20, 60, width() - 40, height() / 2);
    painter.fillRect(imageRect, baseColor.darker(120));
    painter.fillRect(QRectF(imageRect.x(), imageRect.y(),
                            imageRect.width() * 0.8, imageRect.height()),
                     gradient);

    // 模拟按钮占位符
    QRectF buttonRect(20, height() - 40, width() / 3, 30);
    painter.fillRect(buttonRect, baseColor.darker(130));
    painter.fillRect(QRectF(buttonRect.x(), buttonRect.y(),
                            buttonRect.width() * 0.6, buttonRect.height()),
                     gradient);
}

void SkeletonWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    m_animationStep = 0;  // 可选：重置动画步数
}
