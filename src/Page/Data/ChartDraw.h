#ifndef CHARTDRAW_H
#define CHARTDRAW_H

#include <QChart>
#include <QLineSeries>
#include <QValueAxis>
#include <QChartView>
#include <QString>
#include <QVector>
#include <QDateTime>

namespace ChartDrawing {

struct TemperatureData {
    QString type;
    QDateTime timestamp;
    double temperature;
};

struct ChartSettings {
    QString title = "Temperature Over Time";
    QString xAxisTitle = "Time";
    QString yAxisTitle = "Temperature (°C)";
    QString timeFormat = "%H:%M:%S";
    double yAxisMin = 0.0;
    double yAxisMax = 100.0;
    QColor gridColor = Qt::lightGray;
    bool showGrid = true;
    bool showLegend = true;
};

class TemperatureChartDrawer {
public:
    TemperatureChartDrawer();
    ~TemperatureChartDrawer();

    // 创建温度图表
    QChart* createMultiTemperatureChart(
        const QVector<QVector<TemperatureData>>& allData,
        const QVector<QString>& selectedTypes,
        double lowerThreshold,
        double upperThreshold,
        const ChartSettings& settings = ChartSettings());

    // 错误处理
    bool hasError() const;
    QString getLastError() const;

private:
    void initialize();
    bool validateData(const QVector<QVector<TemperatureData>>& allData);
    void setupAxis(QChart* chart, QLineSeries* series, 
                  const QVector<TemperatureData>& data,
                  double lowerThreshold, double upperThreshold,
                  const ChartSettings& settings);

    QString m_lastError;
};

} // namespace ChartDrawing

#endif // CHARTDRAW_H