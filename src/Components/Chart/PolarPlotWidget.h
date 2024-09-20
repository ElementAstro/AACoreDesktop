#ifndef POLARPLOTWIDGET_H
#define POLARPLOTWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QPolarChart>


class PolarPlotWidget : public QWidget {
    Q_OBJECT
public:
    PolarPlotWidget(QWidget* parent = nullptr);

    // 设置极坐标图数据
    void setPolarData(const QVector<QPointF>& points);

private:
    QPolarChart* polarChart;
    QChartView* chartView;
};

#endif  // POLARPLOTWIDGET_H
