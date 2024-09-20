#include "ImageGraphicsView.h"
#include <QColorDialog>
#include <QContextMenuEvent>
#include <QFileDialog>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QMenu>
#include <QMouseEvent>
#include <QPen>
#include <QWheelEvent>

ImageGraphicsView::ImageGraphicsView(QWidget* parent)
    : QGraphicsView(parent),
      crossHairX(nullptr),
      crossHairY(nullptr),
      tempRect(nullptr),
      tempEllipse(nullptr),
      currentMode(DrawMode::None),
      selectedShape(nullptr) {
    scene = new QGraphicsScene(this);
    setScene(scene);
    setMouseTracking(true);  // 启用鼠标跟踪
}

void ImageGraphicsView::loadImage(const QImage& image) {
    scene->clear();  // 清除场景中的所有项目
    auto* item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
    scene->addItem(item);  // 添加图像到场景
    crossHairX = scene->addLine(QLineF(), QPen(Qt::green));  // 添加十字准线
    crossHairY = scene->addLine(QLineF(), QPen(Qt::green));
}

void ImageGraphicsView::saveSceneToImage(const QString& filePath) {
    QImage image(scene->sceneRect().size().toSize(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    scene->render(&painter);
    image.save(filePath);
}

void ImageGraphicsView::mouseMoveEvent(QMouseEvent* event) {
    QPointF scenePos = mapToScene(event->pos());  // 将视图坐标映射到场景坐标
    if ((crossHairX != nullptr) && (crossHairY != nullptr)) {
        crossHairX->setLine(scenePos.x(), 0, scenePos.x(),
                            scene->height());  // 更新十字准线X轴
        crossHairY->setLine(0, scenePos.y(), scene->width(),
                            scenePos.y());  // 更新十字准线Y轴
    }

    // 更新正在绘制的矩形或椭圆
    if (currentMode == DrawMode::Rectangle && (tempRect != nullptr)) {
        QRectF rect(startPoint, scenePos);
        tempRect->setRect(rect.normalized());
    } else if (currentMode == DrawMode::Ellipse && (tempEllipse != nullptr)) {
        QRectF rect(startPoint, scenePos);
        tempEllipse->setRect(rect.normalized());
    }

    QGraphicsView::mouseMoveEvent(event);
}

void ImageGraphicsView::mousePressEvent(QMouseEvent* event) {
    QPointF scenePos = mapToScene(event->pos());

    if (event->button() == Qt::LeftButton) {
        // 处理图形绘制
        startPoint = scenePos;  // 记录起点
        if (currentMode == DrawMode::Rectangle) {
            tempRect =
                scene->addRect(QRectF(startPoint, startPoint), QPen(Qt::red));
            undoStack.push(tempRect);
        } else if (currentMode == DrawMode::Ellipse) {
            tempEllipse = scene->addEllipse(QRectF(startPoint, startPoint),
                                            QPen(Qt::blue));
            undoStack.push(tempEllipse);
        } else {
            // 选择已经绘制的图形
            QGraphicsItem* item = scene->itemAt(scenePos, QTransform());
            if ((item != nullptr) && item != crossHairX && item != crossHairY) {
                updateSelection(item);
            }
        }
    }

    QGraphicsView::mousePressEvent(event);
}

void ImageGraphicsView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        tempRect = nullptr;
        tempEllipse = nullptr;
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void ImageGraphicsView::wheelEvent(QWheelEvent* event) {
    const double SCALE_FACTOR = 1.15;
    if (event->angleDelta().y() > 0) {
        scale(SCALE_FACTOR, SCALE_FACTOR);  // 放大
    } else {
        scale(1.0 / SCALE_FACTOR, 1.0 / SCALE_FACTOR);  // 缩小
    }
    QGraphicsView::wheelEvent(event);
}

void ImageGraphicsView::contextMenuEvent(QContextMenuEvent* event) {
    QMenu contextMenu;
    contextMenu.addAction("Draw Rectangle", this, SLOT(drawRectangle()));
    contextMenu.addAction("Draw Ellipse", this, SLOT(drawEllipse()));
    contextMenu.addAction("Clear Shapes", this, SLOT(clearShapes()));
    contextMenu.addAction("Undo", this, SLOT(undo()));
    contextMenu.addAction("Redo", this, SLOT(redo()));
    if (selectedShape != nullptr) {
        contextMenu.addAction("Change Color", this, SLOT(setShapeColor()));
    }
    contextMenu.exec(event->globalPos());  // 在鼠标位置显示上下文菜单
}

void ImageGraphicsView::drawRectangle() {
    currentMode = DrawMode::Rectangle;  // 设置为绘制矩形模式
}

void ImageGraphicsView::drawEllipse() {
    currentMode = DrawMode::Ellipse;  // 设置为绘制椭圆模式
}

void ImageGraphicsView::clearShapes() {
    scene->clear();  // 清除场景中的所有图形
    crossHairX = scene->addLine(QLineF(), QPen(Qt::green));  // 重新添加十字准线
    crossHairY = scene->addLine(QLineF(), QPen(Qt::green));
}

void ImageGraphicsView::undo() {
    if (!undoStack.isEmpty()) {
        QGraphicsItem* item = undoStack.pop();
        scene->removeItem(item);
        redoStack.push(item);
    }
}

void ImageGraphicsView::redo() {
    if (!redoStack.isEmpty()) {
        QGraphicsItem* item = redoStack.pop();
        scene->addItem(item);
        undoStack.push(item);
    }
}

void ImageGraphicsView::setShapeColor() {
    if (selectedShape != nullptr) {
        QColor color = QColorDialog::getColor(Qt::white, this);
        if (color.isValid()) {
            // 尝试将 selectedShape 转换为 QGraphicsRectItem
            if (auto* rectItem =
                    qgraphicsitem_cast<QGraphicsRectItem*>(selectedShape)) {
                QPen pen = rectItem->pen();
                pen.setColor(color);
                rectItem->setPen(pen);
            }
            // 尝试将 selectedShape 转换为 QGraphicsEllipseItem
            else if (auto* ellipseItem =
                         qgraphicsitem_cast<QGraphicsEllipseItem*>(
                             selectedShape)) {
                QPen pen = ellipseItem->pen();
                pen.setColor(color);
                ellipseItem->setPen(pen);
            }
            // 如果有其他可支持的 QGraphicsItem 子类，类似处理
        }
    }
}

void ImageGraphicsView::updateSelection(QGraphicsItem* item) {
    // 取消之前选中的图形的高亮效果
    if (selectedShape != nullptr) {
        if (auto* rectItem =
                qgraphicsitem_cast<QGraphicsRectItem*>(selectedShape)) {
            rectItem->setPen(QPen(Qt::black));  // 重置之前的形状颜色
        } else if (auto* ellipseItem =
                       qgraphicsitem_cast<QGraphicsEllipseItem*>(
                           selectedShape)) {
            ellipseItem->setPen(QPen(Qt::black));
        }
    }

    // 设置新选中的图形
    selectedShape = item;
    if (auto* rectItem =
            qgraphicsitem_cast<QGraphicsRectItem*>(selectedShape)) {
        rectItem->setPen(QPen(Qt::yellow));  // 设置高亮效果
    } else if (auto* ellipseItem =
                   qgraphicsitem_cast<QGraphicsEllipseItem*>(selectedShape)) {
        ellipseItem->setPen(QPen(Qt::yellow));
    }
}
