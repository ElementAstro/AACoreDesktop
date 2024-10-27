// Software.cpp
#include "Software.h"

#include <QDebug>
#include <QSettings>
#ifdef _WIN32
#include <windows.h>
#include <winreg.h>
#elif __linux__
#include <cstdlib>
#include <fstream>
#include <sstream>

#elif __APPLE__
#include <cstdlib>
#include <sstream>
#endif

SoftwareManager::SoftwareManager() { loadInstalledSoftware(); }

void SoftwareManager::loadInstalledSoftware() {
#ifdef _WIN32
    loadInstalledSoftwareWindows();
#elif __linux__
    loadInstalledSoftwareLinux();
#elif __APPLE__
    loadInstalledSoftwareMac();
#else
    qDebug() << "Unsupported OS";
#endif
}

void SoftwareManager::loadInstalledSoftwareWindows() {
    QStringList regPaths = {
        "HKEY_LOCAL_"
        "MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall",
        "HKEY_CURRENT_"
        "USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"};

    foreach (const QString &regPath, regPaths) {
        QSettings settings(regPath, QSettings::NativeFormat);
        QStringList keys = settings.childGroups();
        foreach (const QString &key, keys) {
            settings.beginGroup(key);
            SoftwareInfo info;
            info.displayName = settings.value("DisplayName").toString();
            info.displayVersion = settings.value("DisplayVersion").toString();
            info.installLocation = settings.value("InstallLocation").toString();
            info.publisher = settings.value("Publisher").toString();
            info.installDate = settings.value("InstallDate").toString();

            if (!info.displayName.isEmpty()) {
                installedSoftware[info.displayName] = info;
            }
            settings.endGroup();
        }
    }
}

void SoftwareManager::loadInstalledSoftwareLinux() {
    QStringList commands = {"dpkg-query -W -f='${Package} ${Version}\n'",
                            "rpm -qa --qf '%{NAME} %{VERSION}\n'"};
    foreach (const QString &command, commands) {
        FILE *fp = popen(command.toStdString().c_str(), "r");
        if (fp == nullptr)
            continue;

        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
            QString line = QString::fromUtf8(buffer).trimmed();
            QStringList parts = line.split(" ");
            if (parts.size() >= 2) {
                SoftwareInfo info;
                info.displayName = parts[0];
                info.displayVersion = parts[1];
                installedSoftware[info.displayName] = info;
            }
        }
        pclose(fp);
    }
}

void SoftwareManager::loadInstalledSoftwareMac() {
    FILE *fp = popen("system_profiler SPApplicationsDataType", "r");
    if (fp == nullptr)
        return;

    char buffer[1024];
    SoftwareInfo info;
    while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
        QString line = QString::fromUtf8(buffer).trimmed();
        if (line.startsWith("Location:")) {
            info.installLocation = line.section(":", 1, 1).trimmed();
        } else if (line.startsWith("Version:")) {
            info.displayVersion = line.section(":", 1, 1).trimmed();
        } else if (line.startsWith("Obtained from:")) {
            info.publisher = line.section(":", 1, 1).trimmed();
        } else if (line.startsWith("  ")) {
            if (!info.displayName.isEmpty()) {
                installedSoftware[info.displayName] = info;
            }
            info = SoftwareInfo();
            info.displayName = line.trimmed();
        }
    }
    if (!info.displayName.isEmpty()) {
        installedSoftware[info.displayName] = info;
    }
    pclose(fp);
}

bool SoftwareManager::isSoftwareInstalled(const QString &softwareName) const {
    return installedSoftware.contains(softwareName);
}

SoftwareManager::SoftwareInfo SoftwareManager::getSoftwareInfo(
    const QString &softwareName) const {
    return installedSoftware.value(softwareName, SoftwareInfo());
}

QList<SoftwareManager::SoftwareInfo> SoftwareManager::getAllInstalledSoftware()
    const {
    return installedSoftware.values();
}