#include "T_SystemInfo.h"

#include <QProcess>
#include <QRegularExpression>
#include <QVBoxLayout>
#include <QtCharts/QValueAxis>
#include <QtGlobal>
#include <random>


#include "ElaTabWidget.h"
#include "ElaText.h"

namespace {
constexpr int kMaxTimeRange = 50;
constexpr int kMaxCpuUsage = 100;
constexpr int kMaxMemoryUsage = 100;
constexpr int kMaxNetworkSpeed = 1000;
constexpr int kUpdateIntervalMs = 1000;
constexpr int kLabelPixelSize = 15;
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

    tabWidget = new ElaTabWidget(this);

    setupCpuTab();
    setupMemoryTab();
    setupNetworkTab();

    mainLayout->addWidget(tabWidget);
}

void T_SystemInfoPage::setupCpuTab() {
    auto *cpuTab = new QWidget();
    auto *cpuLayout = new QVBoxLayout(cpuTab);

    cpuSeries = new QLineSeries();
    auto *cpuChart = new QChart();
    cpuChart->addSeries(cpuSeries);
    cpuChart->setTitle("CPU 使用率");

    auto *cpuAxisX = new QValueAxis;
    cpuAxisX->setRange(0, kMaxTimeRange);
    cpuAxisX->setLabelFormat("%i");
    cpuAxisX->setTitleText("时间");

    auto *cpuAxisY = new QValueAxis;
    cpuAxisY->setRange(0, kMaxCpuUsage);
    cpuAxisY->setTitleText("使用率 (%)");

    cpuChart->addAxis(cpuAxisX, Qt::AlignBottom);
    cpuChart->addAxis(cpuAxisY, Qt::AlignLeft);

    cpuSeries->attachAxis(cpuAxisX);
    cpuSeries->attachAxis(cpuAxisY);

    cpuChartView = new QChartView(cpuChart);
    cpuChartView->setRenderHint(QPainter::Antialiasing);

    cpuUsageLabel = new ElaText("当前 CPU 使用率: 0%", this);

    cpuLayout->addWidget(cpuChartView);
    cpuLayout->addWidget(cpuUsageLabel);
    tabWidget->addTab(cpuTab, "CPU");
}

void T_SystemInfoPage::setupMemoryTab() {
    auto *memoryTab = new QWidget();
    auto *memoryLayout = new QVBoxLayout(memoryTab);

    memorySeries = new QLineSeries();
    auto *memoryChart = new QChart();
    memoryChart->addSeries(memorySeries);
    memoryChart->setTitle("内存 使用率");

    auto *memoryAxisX = new QValueAxis;
    memoryAxisX->setRange(0, kMaxTimeRange);
    memoryAxisX->setLabelFormat("%i");
    memoryAxisX->setTitleText("时间");

    auto *memoryAxisY = new QValueAxis;
    memoryAxisY->setRange(0, kMaxMemoryUsage);
    memoryAxisY->setTitleText("使用率 (%)");

    memoryChart->addAxis(memoryAxisX, Qt::AlignBottom);
    memoryChart->addAxis(memoryAxisY, Qt::AlignLeft);

    memorySeries->attachAxis(memoryAxisX);
    memorySeries->attachAxis(memoryAxisY);

    memoryChartView = new QChartView(memoryChart);
    memoryChartView->setRenderHint(QPainter::Antialiasing);

    memoryUsageLabel = new ElaText("当前内存使用率: 0%", this);

    memoryLayout->addWidget(memoryChartView);
    memoryLayout->addWidget(memoryUsageLabel);
    tabWidget->addTab(memoryTab, "内存");
}

void T_SystemInfoPage::setupNetworkTab() {
    auto *networkTab = new QWidget();
    auto *networkLayout = new QVBoxLayout(networkTab);

    networkUpSeries = new QLineSeries();
    networkDownSeries = new QLineSeries();
    auto *networkChart = new QChart();
    networkChart->addSeries(networkUpSeries);
    networkChart->addSeries(networkDownSeries);
    networkChart->setTitle("网络流量 (上传 / 下载)");

    auto *networkAxisX = new QValueAxis;
    networkAxisX->setRange(0, kMaxTimeRange);
    networkAxisX->setLabelFormat("%i");
    networkAxisX->setTitleText("时间");

    auto *networkAxisY = new QValueAxis;
    networkAxisY->setRange(0, kMaxNetworkSpeed);
    networkAxisY->setTitleText("流量 (KB/s)");

    networkChart->addAxis(networkAxisX, Qt::AlignBottom);
    networkChart->addAxis(networkAxisY, Qt::AlignLeft);

    networkUpSeries->attachAxis(networkAxisX);
    networkUpSeries->attachAxis(networkAxisY);
    networkDownSeries->attachAxis(networkAxisX);
    networkDownSeries->attachAxis(networkAxisY);

    networkChartView = new QChartView(networkChart);
    networkChartView->setRenderHint(QPainter::Antialiasing);

    networkUpLabel = new ElaText("当前上传速度: 0 KB/s", this);
    networkDownLabel = new ElaText("当前下载速度: 0 KB/s", this);

    networkLayout->addWidget(networkChartView);
    networkLayout->addWidget(networkUpLabel);
    networkLayout->addWidget(networkDownLabel);
    tabWidget->addTab(networkTab, "网络");
}

void T_SystemInfoPage::setupConnections() {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &T_SystemInfoPage::updateCpuUsage);
    connect(timer, &QTimer::timeout, this,
            &T_SystemInfoPage::updateMemoryUsage);
    connect(timer, &QTimer::timeout, this,
            &T_SystemInfoPage::updateNetworkUsage);
    timer->start(kUpdateIntervalMs);
}

void T_SystemInfoPage::updateCpuUsage() {
    QProcess process;
#ifdef Q_OS_WIN
    process.start("wmic cpu get loadpercentage");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    QRegularExpression regexCpuUsage("(\\d+)");
    QRegularExpressionMatch match = regexCpuUsage.match(output);
    qreal cpuUsage = match.hasMatch() ? match.captured(1).toDouble() : 0.0;
#else
    process.start("top -bn1 | grep 'Cpu(s)'");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    QRegularExpression regexCpuUsage("Cpu\\(s\\):\\s+(\\d+\\.\\d+)%us");
    QRegularExpressionMatch match = regexCpuUsage.match(output);
    qreal cpuUsage = match.hasMatch() ? match.captured(1).toDouble() : 0.0;
#endif

    cpuSeries->append(timeIndex, cpuUsage);
    cpuUsageLabel->setText(QString("当前 CPU 使用率: %1%").arg(cpuUsage));
    cpuUsageLabel->setTextPixelSize(kLabelPixelSize);

    if (cpuSeries->count() > kMaxTimeRange) {
        cpuSeries->remove(0);
    }
    cpuChartView->chart()
        ->axes(Qt::Horizontal)
        .first()
        ->setRange(timeIndex - kMaxTimeRange, timeIndex);
}

void T_SystemInfoPage::updateMemoryUsage() {
    QProcess process;
#ifdef Q_OS_WIN
    process.start(
        "wmic OS get FreePhysicalMemory,TotalVisibleMemorySize /Format:List");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    QRegularExpression regexMemoryUsage(
        "FreePhysicalMemory=(\\d+).*TotalVisibleMemorySize=(\\d+)",
        QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch match = regexMemoryUsage.match(output);
    qreal freeMemory = match.hasMatch() ? match.captured(1).toDouble() : 0.0;
    qreal totalMemory = match.hasMatch() ? match.captured(2).toDouble() : 0.0;
    qreal memoryUsage =
        totalMemory > 0 ? (1 - freeMemory / totalMemory) * 100 : 0.0;
#else
    process.start("free | grep Mem");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    QRegularExpression regexMemoryUsage("Mem:\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)");
    QRegularExpressionMatch match = regexMemoryUsage.match(output);
    qreal totalMemory = match.hasMatch() ? match.captured(1).toDouble() : 0.0;
    qreal usedMemory = match.hasMatch() ? match.captured(2).toDouble() : 0.0;
    qreal memoryUsage =
        totalMemory > 0 ? (usedMemory / totalMemory) * 100 : 0.0;
#endif

    memorySeries->append(timeIndex, memoryUsage);
    memoryUsageLabel->setText(QString("当前内存使用率: %1%").arg(memoryUsage));
    memoryUsageLabel->setTextPixelSize(kLabelPixelSize);

    if (memorySeries->count() > kMaxTimeRange) {
        memorySeries->remove(0);
    }
    memoryChartView->chart()
        ->axes(Qt::Horizontal)
        .first()
        ->setRange(timeIndex - kMaxTimeRange, timeIndex);
}

void T_SystemInfoPage::updateNetworkUsage() {
    static std::random_device rd;
    static std::default_random_engine generator(rd());
    static std::uniform_real_distribution<qreal> distribution(0.0,
                                                              kMaxNetworkSpeed);

    qreal uploadSpeed = distribution(generator);
    qreal downloadSpeed = distribution(generator);

    networkUpSeries->append(timeIndex, uploadSpeed);
    networkDownSeries->append(timeIndex, downloadSpeed);

    networkUpLabel->setText(QString("当前上传速度: %1 KB/s").arg(uploadSpeed));
    networkUpLabel->setTextPixelSize(kLabelPixelSize);
    networkDownLabel->setText(
        QString("当前下载速度: %1 KB/s").arg(downloadSpeed));
    networkDownLabel->setTextPixelSize(kLabelPixelSize);

    if (networkUpSeries->count() > kMaxTimeRange) {
        networkUpSeries->remove(0);
        networkDownSeries->remove(0);
    }

    networkChartView->chart()
        ->axes(Qt::Horizontal)
        .first()
        ->setRange(timeIndex - kMaxTimeRange, timeIndex);
    timeIndex++;
}