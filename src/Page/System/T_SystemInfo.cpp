#include "T_SystemInfo.h"

#include <QVBoxLayout>
#include <QtCharts/QValueAxis>
#include <QtGlobal>
#include <random>

#include "ElaTabWidget.h"
#include "ElaText.h"

namespace {
constexpr int MaxTimeRange = 50;
constexpr int MaxCpuUsage = 100;
constexpr int MaxMemoryUsage = 100;
constexpr int MaxNetworkSpeed = 1000;
constexpr int UpdateIntervalMs = 1000;
}  // namespace

T_SystemInfoPage::T_SystemInfoPage(QWidget *parent)
    : QWidget(parent),
      tabWidget(nullptr),
      cpuChartView(nullptr),
      memoryChartView(nullptr),
      networkChartView(nullptr),
      cpuSeries(nullptr),
      memorySeries(nullptr),
      networkUpSeries(nullptr),
      networkDownSeries(nullptr),
      cpuUsageLabel(nullptr),
      memoryUsageLabel(nullptr),
      networkUpLabel(nullptr),
      networkDownLabel(nullptr),
      timer(nullptr),
      timeIndex(0) {
    setupUI();
    setupConnections();
}

T_SystemInfoPage::~T_SystemInfoPage() = default;

void T_SystemInfoPage::setupUI() {
    auto *mainLayout = new QVBoxLayout(this);

    // 创建 QTabWidget 用于分离不同的图表
    tabWidget = new ElaTabWidget(this);

    // CPU 使用率 Tab
    auto *cpuTab = new QWidget();
    auto *cpuLayout = new QVBoxLayout(cpuTab);

    cpuSeries = new QLineSeries();
    auto *cpuChart = new QChart();
    cpuChart->addSeries(cpuSeries);
    cpuChart->setTitle("CPU 使用率");

    auto *cpuAxisX = new QValueAxis;
    cpuAxisX->setRange(0, MaxTimeRange);
    cpuAxisX->setLabelFormat("%i");
    cpuAxisX->setTitleText("时间");

    auto *cpuAxisY = new QValueAxis;
    cpuAxisY->setRange(0, MaxCpuUsage);
    cpuAxisY->setTitleText("使用率 (%)");

    cpuChart->addAxis(cpuAxisX, Qt::AlignBottom);
    cpuChart->addAxis(cpuAxisY, Qt::AlignLeft);

    cpuSeries->attachAxis(cpuAxisX);
    cpuSeries->attachAxis(cpuAxisY);

    cpuChartView = new QChartView(cpuChart);
    cpuChartView->setRenderHint(QPainter::Antialiasing);

    // CPU 当前使用率 Label
    cpuUsageLabel = new ElaText("当前 CPU 使用率: 0%", this);

    cpuLayout->addWidget(cpuChartView);
    cpuLayout->addWidget(cpuUsageLabel);
    tabWidget->addTab(cpuTab, "CPU");

    // 内存使用率 Tab
    auto *memoryTab = new QWidget();
    auto *memoryLayout = new QVBoxLayout(memoryTab);

    memorySeries = new QLineSeries();
    auto *memoryChart = new QChart();
    memoryChart->addSeries(memorySeries);
    memoryChart->setTitle("内存 使用率");

    auto *memoryAxisX = new QValueAxis;
    memoryAxisX->setRange(0, MaxTimeRange);
    memoryAxisX->setLabelFormat("%i");
    memoryAxisX->setTitleText("时间");

    auto *memoryAxisY = new QValueAxis;
    memoryAxisY->setRange(0, MaxMemoryUsage);
    memoryAxisY->setTitleText("使用率 (%)");

    memoryChart->addAxis(memoryAxisX, Qt::AlignBottom);
    memoryChart->addAxis(memoryAxisY, Qt::AlignLeft);

    memorySeries->attachAxis(memoryAxisX);
    memorySeries->attachAxis(memoryAxisY);

    memoryChartView = new QChartView(memoryChart);
    memoryChartView->setRenderHint(QPainter::Antialiasing);

    // 内存当前使用率 Label
    memoryUsageLabel = new ElaText("当前内存使用率: 0%", this);

    memoryLayout->addWidget(memoryChartView);
    memoryLayout->addWidget(memoryUsageLabel);
    tabWidget->addTab(memoryTab, "内存");

    // 网络流量 Tab
    auto *networkTab = new QWidget();
    auto *networkLayout = new QVBoxLayout(networkTab);

    networkUpSeries = new QLineSeries();
    networkDownSeries = new QLineSeries();
    auto *networkChart = new QChart();
    networkChart->addSeries(networkUpSeries);
    networkChart->addSeries(networkDownSeries);
    networkChart->setTitle("网络流量 (上传 / 下载)");

    auto *networkAxisX = new QValueAxis;
    networkAxisX->setRange(0, MaxTimeRange);
    networkAxisX->setLabelFormat("%i");
    networkAxisX->setTitleText("时间");

    auto *networkAxisY = new QValueAxis;
    networkAxisY->setRange(0, MaxNetworkSpeed);  // 假设最大网络带宽1000KB/s
    networkAxisY->setTitleText("流量 (KB/s)");

    networkChart->addAxis(networkAxisX, Qt::AlignBottom);
    networkChart->addAxis(networkAxisY, Qt::AlignLeft);

    networkUpSeries->attachAxis(networkAxisX);
    networkUpSeries->attachAxis(networkAxisY);
    networkDownSeries->attachAxis(networkAxisX);
    networkDownSeries->attachAxis(networkAxisY);

    networkChartView = new QChartView(networkChart);
    networkChartView->setRenderHint(QPainter::Antialiasing);

    // 上传和下载速度 Label
    networkUpLabel = new ElaText("当前上传速度: 0 KB/s", this);
    networkDownLabel = new ElaText("当前下载速度: 0 KB/s", this);

    networkLayout->addWidget(networkChartView);
    networkLayout->addWidget(networkUpLabel);
    networkLayout->addWidget(networkDownLabel);
    tabWidget->addTab(networkTab, "网络");

    // 将 TabWidget 添加到主布局
    mainLayout->addWidget(tabWidget);
}

void T_SystemInfoPage::setupConnections() {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &T_SystemInfoPage::updateCpuUsage);
    connect(timer, &QTimer::timeout, this,
            &T_SystemInfoPage::updateMemoryUsage);
    connect(timer, &QTimer::timeout, this,
            &T_SystemInfoPage::updateNetworkUsage);
    timer->start(UpdateIntervalMs);  // 每秒更新
}

void T_SystemInfoPage::updateCpuUsage() {
    static std::default_random_engine generator;
    static std::uniform_real_distribution<qreal> distribution(0.0, MaxCpuUsage);

    qreal cpuUsage = distribution(generator);  // 模拟CPU使用率
    cpuSeries->append(timeIndex, cpuUsage);
    cpuUsageLabel->setText(QString("当前 CPU 使用率: %1%").arg(cpuUsage));
    cpuUsageLabel->setTextPixelSize(15);

    if (cpuSeries->count() > MaxTimeRange) {
        cpuSeries->remove(0);
    }
    cpuChartView->chart()
        ->axes(Qt::Horizontal)
        .first()
        ->setRange(timeIndex - MaxTimeRange, timeIndex);
}

void T_SystemInfoPage::updateMemoryUsage() {
    static std::default_random_engine generator;
    static std::uniform_real_distribution<qreal> distribution(0.0,
                                                              MaxMemoryUsage);

    qreal memoryUsage = distribution(generator);  // 模拟内存使用率
    memorySeries->append(timeIndex, memoryUsage);
    memoryUsageLabel->setText(QString("当前内存使用率: %1%").arg(memoryUsage));
    memoryUsageLabel->setTextPixelSize(15);

    if (memorySeries->count() > MaxTimeRange) {
        memorySeries->remove(0);
    }
    memoryChartView->chart()
        ->axes(Qt::Horizontal)
        .first()
        ->setRange(timeIndex - MaxTimeRange, timeIndex);
}

void T_SystemInfoPage::updateNetworkUsage() {
    static std::default_random_engine generator;
    static std::uniform_real_distribution<qreal> distribution(0.0,
                                                              MaxNetworkSpeed);

    qreal uploadSpeed = distribution(generator);  // 模拟上传速度（KB/s）
    qreal downloadSpeed = distribution(generator);  // 模拟下载速度（KB/s）

    networkUpSeries->append(timeIndex, uploadSpeed);
    networkDownSeries->append(timeIndex, downloadSpeed);

    networkUpLabel->setText(QString("当前上传速度: %1 KB/s").arg(uploadSpeed));
    networkUpLabel->setTextPixelSize(15);
    networkDownLabel->setText(
        QString("当前下载速度: %1 KB/s").arg(downloadSpeed));
    networkDownLabel->setTextPixelSize(15);

    if (networkUpSeries->count() > MaxTimeRange) {
        networkUpSeries->remove(0);
        networkDownSeries->remove(0);
    }

    networkChartView->chart()
        ->axes(Qt::Horizontal)
        .first()
        ->setRange(timeIndex - MaxTimeRange, timeIndex);
    timeIndex++;
}