#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <QProcess>
#include <QString>
#include <QStringList>

#ifdef _WIN32
#include <windows.h>
#endif

class ProcessManager {
public:
    ProcessManager();
    ~ProcessManager();

    // 获取当前进程列表
    QStringList listProcesses();

    // 获取指定进程ID的信息
    QString getProcessInfo(int processID);

    // 启动新进程
    bool startProcess(const QString &program,
                      const QStringList &arguments = QStringList());

    // 终止进程
    bool killProcess(int processID);

    // 设置进程优先级
    bool setProcessPriority(int processID, int priority);

    // 暂停进程
    bool pauseProcess(int processID);

    // 恢复进程
    bool resumeProcess(int processID);

    // 获取进程 CPU 使用率
    double getProcessCpuUsage(int processID);

    // 获取进程内存使用率
    double getProcessMemoryUsage(int processID);

private:
#ifdef _WIN32
    QString getWindowsProcessInfo(DWORD processID);
#else
    QString getLinuxProcessInfo(int pid);
#endif
};

#endif  // PROCESSMANAGER_H