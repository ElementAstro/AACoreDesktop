#ifndef IMAGEGRAPHICSVIEW_H
#define IMAGEGRAPHICSVIEW_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QStack>

class ImageGraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    ImageGraphicsView(QWidget* parent = nullptr);

    // 加载图像
    void loadImage(const QImage& image);

    // 保存场景为图片
    void saveSceneToImage(const QString& filePath);

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
    void drawRectangle();
    void drawEllipse();
    void clearShapes();
    void undo();
    void redo();
    void setShapeColor();

private:
    QGraphicsScene* scene;
    QGraphicsLineItem* crossHairX;
    QGraphicsLineItem* crossHairY;

    // 当前绘制的图形
    enum class DrawMode { None, Rectangle, Ellipse };
    DrawMode currentMode;

    QGraphicsRectItem* tempRect;
    QGraphicsEllipseItem* tempEllipse;

    QPointF startPoint;

    // 图形操作栈
    QStack<QGraphicsItem*> undoStack;
    QStack<QGraphicsItem*> redoStack;

    // 图形选择和颜色改变
    QGraphicsItem* selectedShape;
    void updateSelection(QGraphicsItem* item);
};

#endif  // IMAGEGRAPHICSVIEW_H
