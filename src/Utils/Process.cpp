#include "Process.h"

#include <QDebug>

#ifdef _WIN32
#include <psapi.h>
#include <tlhelp32.h>

#else
#include <signal.h>
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
