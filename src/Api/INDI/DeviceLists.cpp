#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QtXml/QDomDocument>

struct Device {
    QString label;
    QString manufacturer;
    QString driverName;
    QString version;
};

struct DevGroup {
    QString group;
    std::vector<Device> devices;
};

struct DriversList {
    std::vector<DevGroup> devGroups;
};

bool loadXMLFile(const QString &filename, QDomDocument &doc) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Unable to load XML file:" << filename;
        return false;
    }

    if (!doc.setContent(&file)) {
        qWarning() << "Failed to parse XML file:" << filename;
        file.close();
        return false;
    }

    file.close();
    return true;
}

std::vector<DevGroup> parseDriversList(const QString &filename) {
    std::vector<DevGroup> devGroups;
    QDomDocument doc;

    if (!loadXMLFile(filename, doc)) {
        return devGroups;
    }

    QDomElement root = doc.documentElement();
    QDomNodeList devGroupElems = root.elementsByTagName("devGroup");

    for (int i = 0; i < devGroupElems.size(); ++i) {
        QDomElement devGroupElem = devGroupElems.at(i).toElement();
        DevGroup devGroup;
        devGroup.group = devGroupElem.attribute("group");
        devGroups.push_back(devGroup);
    }

    return devGroups;
}

std::vector<DevGroup> parseDevicesFromPath(const QString &path,
                                           std::vector<Device> &devicesFrom) {
    std::vector<DevGroup> devGroups;
    QDir dir(path);
    QFileInfoList files =
        dir.entryInfoList(QStringList() << "*.xml", QDir::Files);

    for (const QFileInfo &fileInfo : files) {
        QString filename = fileInfo.fileName();
        if (filename.endsWith("sk.xml")) {
            continue;  // 跳过以“sk.xml”结尾的文件
        }

        QDomDocument doc;
        if (!loadXMLFile(fileInfo.absoluteFilePath(), doc)) {
            qWarning() << "Unable to load XML file:"
                       << fileInfo.absoluteFilePath();
            continue;
        }

        QDomElement root = doc.documentElement();
        QDomNodeList devGroupElems = root.elementsByTagName("devGroup");

        for (int i = 0; i < devGroupElems.size(); ++i) {
            QDomElement devGroupElem = devGroupElems.at(i).toElement();
            DevGroup devGroup;
            devGroup.group = devGroupElem.attribute("group");

            QDomNodeList deviceElems = devGroupElem.elementsByTagName("device");

            for (int j = 0; j < deviceElems.size(); ++j) {
                QDomElement deviceElem = deviceElems.at(j).toElement();
                Device device;
                device.label = deviceElem.attribute("label");

                if (deviceElem.hasAttribute("manufacturer")) {
                    device.manufacturer = deviceElem.attribute("manufacturer");
                }

                QDomNodeList childElems = deviceElem.childNodes();
                for (int k = 0; k < childElems.size(); ++k) {
                    QDomElement childElem = childElems.at(k).toElement();
                    if (childElem.tagName() == "driver") {
                        device.driverName = childElem.text();
                    } else if (childElem.tagName() == "version") {
                        device.version = childElem.text();
                    }
                }
                devGroup.devices.push_back(device);
                devicesFrom.push_back(device);
            }
            devGroups.push_back(devGroup);
        }
    }

    return devGroups;
}

DriversList mergeDeviceGroups(const DriversList &driversListFrom,
                              const std::vector<DevGroup> &devGroupsFromPath) {
    DriversList mergedList = driversListFrom;

    for (const auto &devGroupXml : devGroupsFromPath) {
        for (auto &devGroupFrom : mergedList.devGroups) {
            if (devGroupXml.group == devGroupFrom.group) {
                devGroupFrom.devices.insert(devGroupFrom.devices.end(),
                                            devGroupXml.devices.begin(),
                                            devGroupXml.devices.end());
            }
        }
    }

    return mergedList;
}

std::tuple<DriversList, std::vector<DevGroup>, std::vector<Device>>
readDriversListFromFiles(const QString &filename, const QString &path) {
    DriversList driversListFrom;
    std::vector<DevGroup> devGroupsFrom;
    std::vector<Device> devicesFrom;

    QDir dir(path);
    if (!dir.exists()) {
        qWarning() << "Folder not found:" << path;
        return {driversListFrom, devGroupsFrom, devicesFrom};
    }

    driversListFrom.devGroups = parseDriversList(filename);
    devGroupsFrom = parseDevicesFromPath(path, devicesFrom);
    driversListFrom = mergeDeviceGroups(driversListFrom, devGroupsFrom);

    return {driversListFrom, devGroupsFrom, devicesFrom};
}
