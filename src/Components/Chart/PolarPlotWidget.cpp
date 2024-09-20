#include "PolarPlotWidget.h"
#include <QVBoxLayout>
#include <QtCharts/QValueAxis>
#include <QtMath>  // for trigonometric functions


PolarPlotWidget::PolarPlotWidget(QWidget* parent) : QWidget(parent) {
    // 创建极坐标图
    polarChart = new QPolarChart();
    chartView =
        new QChartView(polarChart, this);  // 使用 QChartView 来展示 QPolarChart
    chartView->setRenderHint(QPainter::Antialiasing);  // 开启抗锯齿

    // 使用垂直布局，将图表视图添加到窗口中
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(chartView);

    // 初始化极坐标轴
    QValueAxis* angularAxis = new QValueAxis();  // 角度轴
    angularAxis->setTickCount(9);
    angularAxis->setLabelFormat("%.1f");
    angularAxis->setRange(0, 360);
    polarChart->setAxisX(angularAxis);

    QValueAxis* radialAxis = new QValueAxis();  // 半径轴
    radialAxis->setRange(0, 1);
    polarChart->setAxisY(radialAxis);
}

void PolarPlotWidget::setPolarData(const QVector<QPointF>& points) {
    QLineSeries* series = new QLineSeries();

    for (const QPointF& point : points) {
        series->append(point);  // 添加极坐标点
    }

    polarChart->addSeries(series);
    series->attachAxis(polarChart->axisX());  // 关联角度轴
    series->attachAxis(polarChart->axisY());  // 关联半径轴

    polarChart->legend()->hide();  // 隐藏图例
}
