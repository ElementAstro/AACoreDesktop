#include "T_SystemInfo.h"

#include <QVBoxLayout>
#include <QtCharts/QValueAxis>
#include <QtGlobal>

T_SystemInfoPage::T_SystemInfoPage(QWidget *parent)
    : QWidget(parent), timeIndex(0) {
    setupUI();
    setupConnections();
}

T_SystemInfoPage::~T_SystemInfoPage() {}

void T_SystemInfoPage::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 创建CPU使用率图表
    cpuSeries = new QLineSeries();
    QChart *cpuChart = new QChart();
    cpuChart->addSeries(cpuSeries);
    cpuChart->setTitle("CPU 使用率");

    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, 50);
    axisX->setLabelFormat("%i");
    axisX->setTitleText("时间");

    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 100);
    axisY->setTitleText("使用率 (%)");

    cpuChart->addAxis(axisX, Qt::AlignBottom);
    cpuChart->addAxis(axisY, Qt::AlignLeft);

    cpuSeries->attachAxis(axisX);
    cpuSeries->attachAxis(axisY);

    cpuChartView = new QChartView(cpuChart);
    cpuChartView->setRenderHint(QPainter::Antialiasing);

    // 创建内存使用率图表
    memorySeries = new QLineSeries();
    QChart *memoryChart = new QChart();
    memoryChart->addSeries(memorySeries);
    memoryChart->setTitle("内存 使用率");

    QValueAxis *memoryAxisX = new QValueAxis;
    memoryAxisX->setRange(0, 50);
    memoryAxisX->setLabelFormat("%i");
    memoryAxisX->setTitleText("时间");

    QValueAxis *memoryAxisY = new QValueAxis;
    memoryAxisY->setRange(0, 100);
    memoryAxisY->setTitleText("使用率 (%)");

    memoryChart->addAxis(memoryAxisX, Qt::AlignBottom);
    memoryChart->addAxis(memoryAxisY, Qt::AlignLeft);

    memorySeries->attachAxis(memoryAxisX);
    memorySeries->attachAxis(memoryAxisY);

    memoryChartView = new QChartView(memoryChart);
    memoryChartView->setRenderHint(QPainter::Antialiasing);

    // 创建网络流量图表（上传和下载）
    networkUpSeries = new QLineSeries();
    networkDownSeries = new QLineSeries();
    QChart *networkChart = new QChart();
    networkChart->addSeries(networkUpSeries);
    networkChart->addSeries(networkDownSeries);
    networkChart->setTitle("网络流量 (上传 / 下载)");

    QValueAxis *networkAxisX = new QValueAxis;
    networkAxisX->setRange(0, 50);
    networkAxisX->setLabelFormat("%i");
    networkAxisX->setTitleText("时间");

    QValueAxis *networkAxisY = new QValueAxis;
    networkAxisY->setRange(0, 1000);  // 假设最大网络带宽1000KB/s
    networkAxisY->setTitleText("流量 (KB/s)");

    networkChart->addAxis(networkAxisX, Qt::AlignBottom);
    networkChart->addAxis(networkAxisY, Qt::AlignLeft);

    networkUpSeries->attachAxis(networkAxisX);
    networkUpSeries->attachAxis(networkAxisY);
    networkDownSeries->attachAxis(networkAxisX);
    networkDownSeries->attachAxis(networkAxisY);

    networkChartView = new QChartView(networkChart);
    networkChartView->setRenderHint(QPainter::Antialiasing);

    // 添加到布局
    layout->addWidget(cpuChartView);
    layout->addWidget(memoryChartView);
    layout->addWidget(networkChartView);
}

void T_SystemInfoPage::setupConnections() {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &T_SystemInfoPage::updateCpuUsage);
    connect(timer, &QTimer::timeout, this,
            &T_SystemInfoPage::updateMemoryUsage);
    connect(timer, &QTimer::timeout, this,
            &T_SystemInfoPage::updateNetworkUsage);
    timer->start(1000);  // 每秒更新
}

void T_SystemInfoPage::updateCpuUsage() {
    qreal cpuUsage = rand() % 100;  // 模拟CPU使用率
    cpuSeries->append(timeIndex, cpuUsage);

    if (cpuSeries->count() > 50) {
        cpuSeries->remove(0);
    }
    cpuChartView->chart()->axisX()->setRange(timeIndex - 50, timeIndex);
}

void T_SystemInfoPage::updateMemoryUsage() {
    qreal memoryUsage = rand() % 100;  // 模拟内存使用率
    memorySeries->append(timeIndex, memoryUsage);

    if (memorySeries->count() > 50) {
        memorySeries->remove(0);
    }
    memoryChartView->chart()->axisX()->setRange(timeIndex - 50, timeIndex);
}

void T_SystemInfoPage::updateNetworkUsage() {
    qreal uploadSpeed = rand() % 1000;    // 模拟上传速度（KB/s）
    qreal downloadSpeed = rand() % 1000;  // 模拟下载速度（KB/s）

    networkUpSeries->append(timeIndex, uploadSpeed);
    networkDownSeries->append(timeIndex, downloadSpeed);

    if (networkUpSeries->count() > 50) {
        networkUpSeries->remove(0);
        networkDownSeries->remove(0);
    }

    networkChartView->chart()->axisX()->setRange(timeIndex - 50, timeIndex);
    timeIndex++;
}
