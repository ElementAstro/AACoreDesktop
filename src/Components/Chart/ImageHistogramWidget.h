#ifndef IMAGEHISTOGRAMWIDGET_H
#define IMAGEHISTOGRAMWIDGET_H

#include <QImage>
#include <QVector>
#include <QWidget>


class ImageHistogramWidget : public QWidget {
    Q_OBJECT

public:
    explicit ImageHistogramWidget(QWidget *parent = nullptr);
    void setImage(const QImage &image);
    QImage equalizeHistogram(const QImage &image);  // 直方图均衡化功能

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;  // 鼠标移动事件
    void wheelEvent(QWheelEvent *event) override;  // 滚轮事件（用于缩放）

private:
    void calculateHistograms(const QImage &image);
    void drawHistogram(QPainter &painter, const QVector<int> &histogram,
                       const QColor &color, int offsetX);

    QVector<int> redHistogram;
    QVector<int> greenHistogram;
    QVector<int> blueHistogram;
    QVector<int> grayHistogram;  // 灰度直方图
    int histogramMaxValue;
    int zoomFactor;  // 缩放因子
};

#endif  // IMAGEHISTOGRAMWIDGET_H
