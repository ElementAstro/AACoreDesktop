#ifndef CPU_USAGE_H
#define CPU_USAGE_H

#include <string>

class CpuUsage {
public:
    CpuUsage();

    double getCpuUsage();
    std::string getCpuModel();
    int getCpuCores();
    int getLogicalProcessors();

private:
#ifdef _WIN32
    unsigned long long previousIdle;
    unsigned long long previousKernel;
    unsigned long long previousUser;
    bool getCpuTimes(unsigned long long &idleTime, unsigned long long &kernelTime, unsigned long long &userTime);
#elif __linux__
    long long previousTotal;
    long long previousIdle;
    std::pair<long long, long long> getCpuTimes();
#endif

    void parseCpuModel(std::string &model);
};

#endif  // CPU_USAGE_H
