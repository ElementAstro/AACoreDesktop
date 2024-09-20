#include "Software.h"

#include <QDebug>
#include <QSettings>

SoftwareManager::SoftwareManager() { loadInstalledSoftware(); }

void SoftwareManager::loadInstalledSoftware() {
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
