#ifndef SKELETONWIDGET_H
#define SKELETONWIDGET_H

#include <QTimer>
#include <QWidget>

class SkeletonWidget : public QWidget {
    Q_OBJECT

public:
    explicit SkeletonWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QTimer *m_animationTimer;
    int m_animationStep;
};

#endif  // SKELETONWIDGET_H
