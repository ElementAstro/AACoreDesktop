#include <QCoreApplication>
#include <QProcess>
#include <QStringList>
#include <QDebug>
#ifdef _WIN32
    #include <windows.h>
#elif __linux__
    #include <fstream>
#endif

// 获取CPU核心数
int getCpuCores() {
#ifdef _WIN32
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwNumberOfProcessors;
#elif __linux__
    std::ifstream cpuInfo("/proc/cpuinfo");
    std::string line;
    int coreCount = 0;
    while (std::getline(cpuInfo, line)) {
        if (line.find("processor") != std::string::npos) {
            coreCount++;
        }
    }
    return coreCount;
#else
    return -1;  // 不支持的平台
#endif
}

// 获取CPU信息(通过Qt调用系统命令)
QString getCpuInfo() {
    QProcess process;
#ifdef _WIN32
    process.start("wmic cpu get Name, MaxClockSpeed");
#elif __linux__
    process.start("lscpu");
#endif
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    return output;
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // 打印CPU核心数
    int cores = getCpuCores();
    qDebug() << "CPU Cores:" << cores;

    // 打印CPU详细信息
    QString cpuInfo = getCpuInfo();
    qDebug() << "CPU Info:\n" << cpuInfo;

    return 0;
}
