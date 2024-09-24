#include "T_ImageViewer.h"
#include <qboxlayout.h>

#include <QFileDialog>
#include <QHBoxLayout>
#include <QImageReader>
#include <QVBoxLayout>

#include "ElaPushButton.h"
#include "ElaSlider.h"
#include "ElaText.h"

namespace {
const int kRotateSliderRange = 360;
const int kTextPixelSize = 15;
const int kGaussianBlurKernelSize = 15;
const int kCannyThreshold1 = 100;
const int kCannyThreshold2 = 200;
const float kSharpenKernelValue = 5.0F;
}  // namespace

T_ImageViewerPage::T_ImageViewerPage(QWidget *parent)
    : QWidget(parent), imageLabel(new ElaText(this)) {
    // 创建布局
    auto *mainLayout = new QVBoxLayout(this);
    auto *toolkitLayout = new QVBoxLayout();
    auto *buttonLayout = new QHBoxLayout();
    auto *sliderLayout = new QHBoxLayout();

    auto *openButton = new ElaPushButton("打开图像", this);
    auto *blurButton = new ElaPushButton("高斯模糊", this);
    auto *edgeButton = new ElaPushButton("边缘检测", this);
    auto *grayButton = new ElaPushButton("灰度化", this);
    auto *sharpenButton = new ElaPushButton("锐化", this);
    auto *saveButton = new ElaPushButton("保存图像", this);

    rotateSlider = new ElaSlider(Qt::Horizontal, this);
    rotateSlider->setRange(0, kRotateSliderRange);
    rotateSlider->setValue(0);

    // 将控件添加到布局
    buttonLayout->addWidget(openButton);
    buttonLayout->addWidget(blurButton);
    buttonLayout->addWidget(edgeButton);
    buttonLayout->addWidget(grayButton);
    buttonLayout->addWidget(sharpenButton);
    buttonLayout->addWidget(saveButton);

    auto *roratorLabel = new ElaText("旋转角度: ");
    roratorLabel->setTextPixelSize(kTextPixelSize);
    sliderLayout->addWidget(roratorLabel);
    sliderLayout->addWidget(rotateSlider);

    toolkitLayout->addLayout(buttonLayout);
    toolkitLayout->addLayout(sliderLayout);

    mainLayout->addLayout(toolkitLayout);
    mainLayout->addWidget(imageLabel);

    // 连接信号与槽
    connect(openButton, &ElaPushButton::clicked, this,
            &T_ImageViewerPage::openImage);
    connect(blurButton, &ElaPushButton::clicked, this,
            &T_ImageViewerPage::applyGaussianBlur);
    connect(edgeButton, &ElaPushButton::clicked, this,
            &T_ImageViewerPage::applyCannyEdgeDetection);
    connect(grayButton, &ElaPushButton::clicked, this,
            &T_ImageViewerPage::convertToGrayscale);
    connect(sharpenButton, &ElaPushButton::clicked, this,
            &T_ImageViewerPage::applySharpening);
    connect(rotateSlider, &ElaSlider::valueChanged, this,
            &T_ImageViewerPage::rotateImage);
    connect(saveButton, &ElaPushButton::clicked, this,
            &T_ImageViewerPage::saveImage);

    setLayout(mainLayout);
}

void T_ImageViewerPage::openImage() {
    QString fileName = QFileDialog::getOpenFileName(
        this, "打开图像", "", "Images (*.png *.jpg *.bmp)");
    if (!fileName.isEmpty()) {
        currentImage.load(fileName);
        originalMat = QImageToMat(currentImage);
        updateImageDisplay();
    }
}

void T_ImageViewerPage::applyGaussianBlur() {
    if (originalMat.empty()) {
        return;
    }

    cv::Mat blurredMat;
    cv::GaussianBlur(originalMat, blurredMat,
                     cv::Size(kGaussianBlurKernelSize, kGaussianBlurKernelSize),
                     0);
    currentImage = MatToQImage(blurredMat);
    updateImageDisplay();
}

void T_ImageViewerPage::applyCannyEdgeDetection() {
    if (originalMat.empty()) {
        return;
    }

    cv::Mat grayMat;
    cv::Mat edges;
    cv::cvtColor(originalMat, grayMat, cv::COLOR_BGR2GRAY);
    cv::Canny(grayMat, edges, kCannyThreshold1, kCannyThreshold2);
    currentImage = MatToQImage(edges);
    updateImageDisplay();
}

void T_ImageViewerPage::convertToGrayscale() {
    if (originalMat.empty()) {
        return;
    }

    cv::Mat grayMat;
    cv::cvtColor(originalMat, grayMat, cv::COLOR_BGR2GRAY);
    currentImage = MatToQImage(grayMat);
    updateImageDisplay();
}

void T_ImageViewerPage::applySharpening() {
    if (originalMat.empty()) {
        return;
    }

    cv::Mat sharpenedMat;
    cv::Mat kernel = (cv::Mat_<float>(3, 3) << 0, -1, 0, -1,
                      kSharpenKernelValue, -1, 0, -1, 0);  // 锐化滤波核
    cv::filter2D(originalMat, sharpenedMat, originalMat.depth(), kernel);
    currentImage = MatToQImage(sharpenedMat);
    updateImageDisplay();
}

void T_ImageViewerPage::rotateImage(int angle) {
    if (originalMat.empty()) {
        return;
    }

    cv::Mat rotatedMat;
    cv::Point2f center(static_cast<float>(originalMat.cols) / 2.0F,
                       static_cast<float>(originalMat.rows) / 2.0F);
    cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, angle, 1.0);
    cv::warpAffine(originalMat, rotatedMat, rotationMatrix, originalMat.size());
    currentImage = MatToQImage(rotatedMat);
    updateImageDisplay();
}

void T_ImageViewerPage::saveImage() {
    QString fileName = QFileDialog::getSaveFileName(
        this, "保存图像", "", "Images (*.png *.jpg *.bmp)");
    if (!fileName.isEmpty()) {
        currentImage.save(fileName);
    }
}

void T_ImageViewerPage::updateImageDisplay() {
    imageLabel->setPixmap(QPixmap::fromImage(currentImage)
                              .scaled(imageLabel->size(), Qt::KeepAspectRatio));
}

auto T_ImageViewerPage::QImageToMat(const QImage &image) -> cv::Mat {
    cv::Mat mat;
    switch (image.format()) {
        case QImage::Format_RGB32:
            mat = cv::Mat(image.height(), image.width(), CV_8UC4,
                          const_cast<uchar *>(image.constBits()),
                          static_cast<int>(image.bytesPerLine()));
            break;
        case QImage::Format_RGB888:
            mat = cv::Mat(image.height(), image.width(), CV_8UC3,
                          const_cast<uchar *>(image.constBits()),
                          static_cast<int>(image.bytesPerLine()));
            cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
            break;
        case QImage::Format_Grayscale8:
            mat = cv::Mat(image.height(), image.width(), CV_8UC1,
                          const_cast<uchar *>(image.constBits()),
                          static_cast<int>(image.bytesPerLine()));
            break;
        default:
            break;
    }
    return mat.clone();  // 返回深拷贝
}

auto T_ImageViewerPage::MatToQImage(const cv::Mat &mat) -> QImage {
    if (mat.type() == CV_8UC4) {
        return QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step),
                      QImage::Format_RGB32);
    }
    if (mat.type() == CV_8UC3) {
        cv::Mat rgbMat;
        cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB);
        return QImage(rgbMat.data, rgbMat.cols, rgbMat.rows,
                      static_cast<int>(rgbMat.step), QImage::Format_RGB888);
    }
    if (mat.type() == CV_8UC1) {
        return QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step),
                      QImage::Format_Grayscale8);
    }
    return {};
}