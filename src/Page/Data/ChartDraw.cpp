#include "ChartDraw.h"
#include <QDebug>

namespace ChartDrawing {

TemperatureChartDrawer::TemperatureChartDrawer() {
    initialize();
}

TemperatureChartDrawer::~TemperatureChartDrawer() {
    // 清理资源
}

void TemperatureChartDrawer::initialize() {
    m_lastError.clear();
}

bool TemperatureChartDrawer::validateData(
    const QVector<QVector<TemperatureData>>& allData) {
    if (allData.isEmpty()) {
        m_lastError = "Empty data provided";
        return false;
    }

    for (const auto& dataset : allData) {
        if (dataset.isEmpty()) {
            m_lastError = "Empty dataset found";
            return false;
        }
    }
    return true;
}

QChart* TemperatureChartDrawer::createMultiTemperatureChart(
    const QVector<QVector<TemperatureData>>& allData,
    const QVector<QString>& selectedTypes,
    double lowerThreshold,
    double upperThreshold,
    const ChartSettings& settings) {
    
    try {
        if (!validateData(allData)) {
            return nullptr;
        }

        QChart* chart = new QChart();
        chart->setTitle(settings.title);

        if (settings.showLegend) {
            chart->legend()->show();
        } else {
            chart->legend()->hide();
        }

        for (int i = 0; i < allData.size(); ++i) {
            if (!selectedTypes.contains(allData[i].first().type)) {
                continue;
            }

            QLineSeries* series = new QLineSeries(chart);
            series->setName(allData[i].first().type);

            // 添加数据点
            for (const auto& entry : allData[i]) {
                series->append(entry.timestamp.toMSecsSinceEpoch(), 
                             entry.temperature);
            }

            chart->addSeries(series);
            setupAxis(chart, series, allData[i], lowerThreshold, 
                     upperThreshold, settings);
        }

        // 设置网格
        if (settings.showGrid) {
            chart->axes(Qt::Horizontal).first()->setGridLineVisible(true);
            chart->axes(Qt::Vertical).first()->setGridLineVisible(true);
            chart->axes(Qt::Horizontal).first()->setGridLineColor(
                settings.gridColor);
            chart->axes(Qt::Vertical).first()->setGridLineColor(
                settings.gridColor);
        }

        return chart;

    } catch (const std::exception& e) {
        m_lastError = QString("Exception occurred: %1").arg(e.what());
        return nullptr;
    }
}

void TemperatureChartDrawer::setupAxis(
    QChart* chart, QLineSeries* series,
    const QVector<TemperatureData>& data,
    double lowerThreshold, double upperThreshold,
    const ChartSettings& settings) {
    
    QValueAxis* axisX = new QValueAxis(chart);
    axisX->setLabelFormat(settings.timeFormat);
    axisX->setTitleText(settings.xAxisTitle);
    axisX->setRange(data.first().timestamp.toMSecsSinceEpoch(),
                    data.last().timestamp.toMSecsSinceEpoch());
    
    QValueAxis* axisY = new QValueAxis(chart);
    axisY->setTitleText(settings.yAxisTitle);
    axisY->setRange(lowerThreshold, upperThreshold);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
}

bool TemperatureChartDrawer::hasError() const {
    return !m_lastError.isEmpty();
}

QString TemperatureChartDrawer::getLastError() const {
    return m_lastError;
}

} // namespace ChartDrawing