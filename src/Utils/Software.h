#ifndef SOFTWAREMANAGER_H
#define SOFTWAREMANAGER_H

#include <QList>
#include <QMap>
#include <QString>

class SoftwareManager {
public:
    struct SoftwareInfo {
        QString displayName;
        QString displayVersion;
        QString installLocation;
        QString publisher;
        QString installDate;
    };

    SoftwareManager();
    bool isSoftwareInstalled(const QString &softwareName) const;
    SoftwareInfo getSoftwareInfo(const QString &softwareName) const;
    QList<SoftwareInfo> getAllInstalledSoftware() const;

private:
    void loadInstalledSoftware();
    void loadInstalledSoftwareWindows();
    void loadInstalledSoftwareLinux();
    void loadInstalledSoftwareMac();

    QMap<QString, SoftwareInfo> installedSoftware;
};

#endif  // SOFTWAREMANAGER_H
