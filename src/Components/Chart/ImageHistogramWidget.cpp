#include "ImageHistogramWidget.h"
#include <QPainter>
#include <QToolTip>
#include <QWheelEvent>
#include <algorithm>

ImageHistogramWidget::ImageHistogramWidget(QWidget *parent)
    : QWidget(parent),
      histogramMaxValue(0),
      zoomFactor(1)  // 初始缩放因子为1
{}

void ImageHistogramWidget::setImage(const QImage &image) {
    calculateHistograms(image);
    update();  // 触发重绘
}

void ImageHistogramWidget::calculateHistograms(const QImage &image) {
    redHistogram.fill(0, 256);
    greenHistogram.fill(0, 256);
    blueHistogram.fill(0, 256);
    grayHistogram.fill(0, 256);  // 初始化灰度直方图

    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QColor pixelColor = image.pixelColor(x, y);
            int red = pixelColor.red();
            int green = pixelColor.green();
            int blue = pixelColor.blue();

            redHistogram[red]++;
            greenHistogram[green]++;
            blueHistogram[blue]++;

            // 计算灰度值：常用公式 (R*0.299 + G*0.587 + B*0.114)
            int gray = qGray(red, green, blue);
            grayHistogram[gray]++;
        }
    }

    histogramMaxValue = std::max({
        *std::max_element(redHistogram.begin(), redHistogram.end()),
        *std::max_element(greenHistogram.begin(), greenHistogram.end()),
        *std::max_element(blueHistogram.begin(), blueHistogram.end()),
        *std::max_element(grayHistogram.begin(),
                          grayHistogram.end())  // 包括灰度的最大值
    });
}

void ImageHistogramWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    if (histogramMaxValue == 0) {
        return;  // 无效的直方图数据
    }

    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);  // 清除背景

    int histogramWidth = width() / 4;  // 四个通道
    int histogramHeight = height();

    // 绘制红、绿、蓝、灰直方图
    drawHistogram(painter, redHistogram, Qt::red, 0);
    drawHistogram(painter, greenHistogram, Qt::green, histogramWidth);
    drawHistogram(painter, blueHistogram, Qt::blue, 2 * histogramWidth);
    drawHistogram(painter, grayHistogram, Qt::black, 3 * histogramWidth);

    // 绘制坐标轴和图例
    painter.setPen(Qt::black);
    painter.drawLine(0, histogramHeight - 1, width(),
                     histogramHeight - 1);  // 横轴

    painter.drawText(10, histogramHeight + 20, "Red");
    painter.drawText(histogramWidth + 10, histogramHeight + 20, "Green");
    painter.drawText(2 * histogramWidth + 10, histogramHeight + 20, "Blue");
    painter.drawText(3 * histogramWidth + 10, histogramHeight + 20, "Gray");
}

void ImageHistogramWidget::drawHistogram(QPainter &painter,
                                         const QVector<int> &histogram,
                                         const QColor &color, int offsetX) {
    painter.setPen(color);

    int histogramHeight = height();
    int histogramWidth = width() / 4;

    for (int i = 0; i < 256; ++i) {
        int value = histogram[i];
        int barHeight =
            (value * histogramHeight * zoomFactor) / histogramMaxValue;
        painter.drawLine(offsetX + i * histogramWidth / 256, histogramHeight,
                         offsetX + i * histogramWidth / 256,
                         histogramHeight - barHeight);
    }
}

void ImageHistogramWidget::mouseMoveEvent(QMouseEvent *event) {
    int mouseX = event->pos().x();
    int histogramWidth = width() / 4;
    int bin = (mouseX % histogramWidth) * 256 / histogramWidth;
    QString tooltipText;

    // 判断鼠标在哪个直方图上
    if (mouseX < histogramWidth) {
        tooltipText =
            QString("Red Value: %1, Count: %2").arg(bin).arg(redHistogram[bin]);
    } else if (mouseX < 2 * histogramWidth) {
        tooltipText = QString("Green Value: %1, Count: %2")
                          .arg(bin)
                          .arg(greenHistogram[bin]);
    } else if (mouseX < 3 * histogramWidth) {
        tooltipText = QString("Blue Value: %1, Count: %2")
                          .arg(bin)
                          .arg(blueHistogram[bin]);
    } else {
        tooltipText = QString("Gray Value: %1, Count: %2")
                          .arg(bin)
                          .arg(grayHistogram[bin]);
    }

    QToolTip::showText(event->globalPos(), tooltipText);
}

void ImageHistogramWidget::wheelEvent(QWheelEvent *event) {
    int delta =
        event->angleDelta().y() / 120;  // 每次滚动鼠标滚轮增加或减少比例
    zoomFactor = qBound(1, zoomFactor + delta, 10);  // 限制缩放范围在1到10之间
    update();                                        // 重绘直方图
}

QImage ImageHistogramWidget::equalizeHistogram(const QImage &image) {
    QVector<int> cumulativeRed(256, 0), cumulativeGreen(256, 0),
        cumulativeBlue(256, 0);
    int totalPixels = image.width() * image.height();

    // 计算累积分布函数 (CDF)
    for (int i = 1; i < 256; ++i) {
        cumulativeRed[i] = cumulativeRed[i - 1] + redHistogram[i];
        cumulativeGreen[i] = cumulativeGreen[i - 1] + greenHistogram[i];
        cumulativeBlue[i] = cumulativeBlue[i - 1] + blueHistogram[i];
    }

    // 生成均衡化后的图像
    QImage equalizedImage = image.copy();
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QColor pixelColor = image.pixelColor(x, y);
            int red = pixelColor.red();
            int green = pixelColor.green();
            int blue = pixelColor.blue();

            int newRed = (cumulativeRed[red] * 255) / totalPixels;
            int newGreen = (cumulativeGreen[green] * 255) / totalPixels;
            int newBlue = (cumulativeBlue[blue] * 255) / totalPixels;

            equalizedImage.setPixelColor(x, y,
                                         QColor(newRed, newGreen, newBlue));
        }
    }

    return equalizedImage;
}
