#include "Cpu.h"

#ifdef _WIN32
#include <windows.h>
#include <intrin.h>
#include <vector>
#include <thread>
#elif __linux__
#include <sys/sysinfo.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

#endif

CpuUsage::CpuUsage() {
#ifdef _WIN32
    previousIdle = 0;
    previousKernel = 0;
    previousUser = 0;
#elif __linux__
    previousTotal = 0;
    previousIdle = 0;
#endif
}

double CpuUsage::getCpuUsage() {
#ifdef _WIN32
    unsigned long long idleTime, kernelTime, userTime;
    if (getCpuTimes(idleTime, kernelTime, userTime)) {
        unsigned long long idleDiff = idleTime - previousIdle;
        unsigned long long kernelDiff = kernelTime - previousKernel;
        unsigned long long userDiff = userTime - previousUser;
        unsigned long long totalDiff = kernelDiff + userDiff;

        double cpuUsage = (1.0 - (double)idleDiff / totalDiff) * 100.0;

        previousIdle = idleTime;
        previousKernel = kernelTime;
        previousUser = userTime;

        return cpuUsage;
    }
    return -1.0;  // 错误处理
#elif __linux__
    auto [total, idle] = getCpuTimes();

    long long totalDiff = total - previousTotal;
    long long idleDiff = idle - previousIdle;

    double cpuUsage = (1.0 - (double)idleDiff / totalDiff) * 100.0;

    previousTotal = total;
    previousIdle = idle;

    return cpuUsage;
#endif
}

#ifdef _WIN32
bool CpuUsage::getCpuTimes(unsigned long long &idleTime,
                           unsigned long long &kernelTime,
                           unsigned long long &userTime) {
    FILETIME idle, kernel, user;
    if (GetSystemTimes(&idle, &kernel, &user)) {
        idleTime = ((unsigned long long)idle.dwHighDateTime << 32) |
                   idle.dwLowDateTime;
        kernelTime = ((unsigned long long)kernel.dwHighDateTime << 32) |
                     kernel.dwLowDateTime;
        userTime = ((unsigned long long)user.dwHighDateTime << 32) |
                   user.dwLowDateTime;
        return true;
    }
    return false;
}

std::string CpuUsage::getCpuModel() {
    int cpuInfo[4];
    __cpuid(cpuInfo, 0x80000002);
    char model[48];
    memcpy(model, cpuInfo, sizeof(cpuInfo));
    __cpuid(cpuInfo, 0x80000003);
    memcpy(model + 16, cpuInfo, sizeof(cpuInfo));
    __cpuid(cpuInfo, 0x80000004);
    memcpy(model + 32, cpuInfo, sizeof(cpuInfo));
    model[47] = '\0';  // 结束字符串
    return std::string(model);
}

int CpuUsage::getCpuCores() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwNumberOfProcessors;
}

int CpuUsage::getLogicalProcessors() {
    return std::thread::hardware_concurrency();
}

#elif __linux__
std::pair<long long, long long> CpuUsage::getCpuTimes() {
    std::ifstream file("/proc/stat");
    std::string line;
    std::getline(file, line);
    std::istringstream ss(line);

    std::string cpu;
    long long user, nice, system, idle, iowait, irq, softirq, steal, guest,
        guest_nice;
    ss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >>
        steal >> guest >> guest_nice;

    long long idleTime = idle + iowait;
    long long totalTime =
        user + nice + system + idle + iowait + irq + softirq + steal;

    return {totalTime, idleTime};
}

std::string CpuUsage::getCpuModel() {
    std::ifstream cpuInfoFile("/proc/cpuinfo");
    std::string line, model;
    while (std::getline(cpuInfoFile, line)) {
        if (line.find("model name") != std::string::npos) {
            model = line.substr(line.find(':') + 2);  // 获取':'后面的部分
            break;
        }
    }
    return model;
}

int CpuUsage::getCpuCores() {
    return sysconf(_SC_NPROCESSORS_CONF);  // 获取可用的物理核心数
}

int CpuUsage::getLogicalProcessors() {
    return sysconf(_SC_NPROCESSORS_ONLN);  // 获取当前在线的逻辑处理器数量
}
#endif
