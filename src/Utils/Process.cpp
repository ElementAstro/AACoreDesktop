#include "Process.h"

#include <QDebug>

#ifdef _WIN32
#include <psapi.h>
#include <tlhelp32.h>

#else
#include <signal.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <unistd.h>
#include <QDir>
#include <QFile>
#include <QTextStream>


#endif

ProcessManager::ProcessManager() {}

ProcessManager::~ProcessManager() {}

// 获取当前进程列表
QStringList ProcessManager::listProcesses() {
    QStringList processList;

#ifdef _WIN32
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        qDebug() << "Failed to take process snapshot";
        return processList;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        qDebug() << "Failed to retrieve process information";
        return processList;
    }

    do {
        processList << QString::fromWCharArray(pe32.szExeFile);
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
#else
    QDir procDir("/proc");
    QStringList procDirs = procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    foreach (QString procEntry, procDirs) {
        bool isNumber;
        int pid = procEntry.toInt(&isNumber);
        if (isNumber) {
            QFile statusFile("/proc/" + procEntry + "/status");
            if (statusFile.open(QIODevice::ReadOnly)) {
                QTextStream in(&statusFile);
                QString statusInfo = in.readLine();
                processList << statusInfo;
                statusFile.close();
            }
        }
    }
#endif

    return processList;
}

// 获取特定进程ID的信息
QString ProcessManager::getProcessInfo(int processID) {
#ifdef _WIN32
    return getWindowsProcessInfo(processID);
#else
    return getLinuxProcessInfo(processID);
#endif
}

#ifdef _WIN32
QString ProcessManager::getWindowsProcessInfo(DWORD processID) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                  FALSE, processID);
    if (hProcess) {
        HMODULE hMod;
        DWORD cbNeeded;
        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
            char processName[MAX_PATH] = "<unknown>";
            GetModuleBaseNameA(hProcess, hMod, processName,
                               sizeof(processName) / sizeof(char));
            CloseHandle(hProcess);
            return QString(processName);
        }
    }
    return "Unknown Process";
}
#else
QString ProcessManager::getLinuxProcessInfo(int pid) {
    QFile statusFile("/proc/" + QString::number(pid) + "/status");
    if (statusFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&statusFile);
        QString statusInfo = in.readAll();
        statusFile.close();
        return statusInfo;
    } else {
        return "Unknown Process";
    }
}
#endif

// 启动新进程
bool ProcessManager::startProcess(const QString &program,
                                  const QStringList &arguments) {
    QProcess process;
    process.start(program, arguments);
    return process.waitForStarted();
}

// 终止进程
bool ProcessManager::killProcess(int processID) {
#ifdef _WIN32
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processID);
    if (hProcess) {
        bool result = TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
        return result;
    }
    return false;
#else
    return kill(processID, SIGKILL) == 0;
#endif
}

// 设置进程优先级
bool ProcessManager::setProcessPriority(int processID, int priority) {
#ifdef _WIN32
    HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, FALSE, processID);
    if (hProcess) {
        bool result = SetPriorityClass(hProcess, priority);
        CloseHandle(hProcess);
        return result;
    }
    return false;
#else
    return setpriority(PRIO_PROCESS, processID, priority) == 0;
#endif
}

// 暂停进程
bool ProcessManager::pauseProcess(int processID) {
#ifdef _WIN32
    HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, processID);
    if (hProcess) {
        typedef LONG(NTAPI * NtSuspendProcess)(IN HANDLE ProcessHandle);
        NtSuspendProcess pfnNtSuspendProcess = (NtSuspendProcess)GetProcAddress(
            GetModuleHandleA("ntdll.dll"), "NtSuspendProcess");
        if (pfnNtSuspendProcess) {
            pfnNtSuspendProcess(hProcess);
            CloseHandle(hProcess);
            return true;
        }
    }
    return false;
#else
    return kill(processID, SIGSTOP) == 0;
#endif
}

// 恢复进程
bool ProcessManager::resumeProcess(int processID) {
#ifdef _WIN32
    HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, processID);
    if (hProcess) {
        typedef LONG(NTAPI * NtResumeProcess)(IN HANDLE ProcessHandle);
        NtResumeProcess pfnNtResumeProcess = (NtResumeProcess)GetProcAddress(
            GetModuleHandleA("ntdll.dll"), "NtResumeProcess");
        if (pfnNtResumeProcess) {
            pfnNtResumeProcess(hProcess);
            CloseHandle(hProcess);
            return true;
        }
    }
    return false;
#else
    return kill(processID, SIGCONT) == 0;
#endif
}

// 获取进程 CPU 使用率
double ProcessManager::getProcessCpuUsage(int processID) {
#ifdef _WIN32
    FILETIME creationTime, exitTime, kernelTime, userTime;
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                  FALSE, processID);
    if (hProcess) {
        if (GetProcessTimes(hProcess, &creationTime, &exitTime, &kernelTime,
                            &userTime)) {
            ULARGE_INTEGER kTime, uTime;
            kTime.LowPart = kernelTime.dwLowDateTime;
            kTime.HighPart = kernelTime.dwHighDateTime;
            uTime.LowPart = userTime.dwLowDateTime;
            uTime.HighPart = userTime.dwHighDateTime;
            CloseHandle(hProcess);
            return (kTime.QuadPart + uTime.QuadPart) / 10000.0;
        }
        CloseHandle(hProcess);
    }
    return 0.0;
#else
    struct tms timeSample;
    clock_t now;
    now = times(&timeSample);
    return (timeSample.tms_utime + timeSample.tms_stime) /
           (double)sysconf(_SC_CLK_TCK);
#endif
}

// 获取进程内存使用率
double ProcessManager::getProcessMemoryUsage(int processID) {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                  FALSE, processID);
    if (hProcess) {
        if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
            CloseHandle(hProcess);
            return pmc.WorkingSetSize / 1024.0 / 1024.0;  // 转换为 MB
        }
        CloseHandle(hProcess);
    }
    return 0.0;
#else
    QFile statusFile("/proc/" + QString::number(processID) + "/status");
    if (statusFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&statusFile);
        QString line;
        while (in.readLineInto(&line)) {
            if (line.startsWith("VmRSS:")) {
                QStringList parts = line.split(QRegExp("\\s+"));
                if (parts.size() >= 2) {
                    return parts[1].toDouble() / 1024.0;  // 转换为 MB
                }
            }
        }
        statusFile.close();
    }
    return 0.0;
#endif
}