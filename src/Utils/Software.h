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

    // 检查指定软件是否安装
    bool isSoftwareInstalled(const QString &softwareName) const;

    // 获取指定软件的信息
    SoftwareInfo getSoftwareInfo(const QString &softwareName) const;

    // 获取所有已安装软件的列表
    QList<SoftwareInfo> getAllInstalledSoftware() const;

private:
    QMap<QString, SoftwareInfo> installedSoftware;

    // 从注册表中加载已安装的软件信息
    void loadInstalledSoftware();
};

#endif  // SOFTWAREMANAGER_H
