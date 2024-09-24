#ifndef PERFORMANCEPAGE_H
#define PERFORMANCEPAGE_H

#include "T_BasePage.h"

#include <QTimer>
#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

class ElaText;
class ElaTabWidget;

class T_SystemInfoPage : public QWidget {
    Q_OBJECT
public:
    explicit T_SystemInfoPage(QWidget *parent = nullptr);
    ~T_SystemInfoPage();

private:
    ElaTabWidget *tabWidget;

    // 各个图表相关成员
    QChartView *cpuChartView;
    QChartView *memoryChartView;
    QChartView *networkChartView;

    QLineSeries *cpuSeries;
    QLineSeries *memorySeries;
    QLineSeries *networkUpSeries;
    QLineSeries *networkDownSeries;

    ElaText *cpuUsageLabel;
    ElaText *memoryUsageLabel;
    ElaText *networkUpLabel;
    ElaText *networkDownLabel;

    QTimer *timer;
    int timeIndex;

    // 初始化UI
    void setupUI();
    void setupConnections();

    // 更新各个性能指标
    void updateCpuUsage();
    void updateMemoryUsage();
    void updateNetworkUsage();
};

#endif  // PERFORMANCEPAGE_H
