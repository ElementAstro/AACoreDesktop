#ifndef IMAGEEDITORWIDGET_H
#define IMAGEEDITORWIDGET_H

#include <QImage>
#include <QVBoxLayout>
#include <QWidget>

#include <opencv2/opencv.hpp>

class ElaText;
class ElaSlider;

class T_ImageViewerPage : public QWidget {
    Q_OBJECT

public:
    T_ImageViewerPage(QWidget *parent = nullptr);

private slots:
    void openImage();  // 打开图像文件
    void applyGaussianBlur();
    void applyCannyEdgeDetection();
    void convertToGrayscale();
    void applySharpening();
    void rotateImage(int angle);
    void saveImage();  // 保存图像

private:
    QImage currentImage;
    cv::Mat originalMat;  // 使用OpenCV进行处理的原始图像

    ElaText *imageLabel;
    ElaSlider *rotateSlider;

    void updateImageDisplay();
    cv::Mat QImageToMat(const QImage &image);
    QImage MatToQImage(const cv::Mat &mat);
};

#endif  // IMAGEEDITORWIDGET_H
