#include "GlobalConfig.h"

GlobalConfig &GlobalConfig::getInstance() {
    static GlobalConfig instance;
    return instance;
}

GlobalConfig::GlobalConfig() : QObject(nullptr) {
    m_configPath = QDir::homePath() + "/.myapp/config.json";
    loadConfig();
}

GlobalConfig::~GlobalConfig() { saveConfig(); }

void GlobalConfig::loadConfig() noexcept {
    QFile file(m_configPath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray saveData = file.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
        m_stores = loadDoc.object();
    }
}

void GlobalConfig::saveConfig() const noexcept {
    QDir().mkpath(QFileInfo(m_configPath).path());
    QFile file(m_configPath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument saveDoc(m_stores);
        file.write(saveDoc.toJson());
    }
}

std::optional<QMap<QString, QVariant>> GlobalConfig::useStore(QStringView storeName) const {
    if (!m_stores.contains(storeName)) {
        qWarning() << "Store" << storeName << "does not exist!";
        return std::nullopt;
    }

    QJsonObject storeObject = m_stores[storeName].toObject();
    QMap<QString, QVariant> storeMap;
    for (auto it = storeObject.begin(); it != storeObject.end(); ++it) {
        storeMap.insert(it.key(), it.value().toVariant());
    }

    return storeMap;
}

std::optional<QVariant> GlobalConfig::getState(QStringView storeName,
                                               QStringView key) const {
    if (!m_stores.contains(storeName.toString())) {
        qWarning() << "Store" << storeName << "does not exist!";
        return std::nullopt;
    }

    QJsonObject storeObject = m_stores[storeName.toString()].toObject();
    return storeObject[key.toString()].toVariant();
}

void GlobalConfig::resetStore(QStringView storeName) noexcept {
    if (!m_stores.contains(storeName.toString())) {
        qWarning() << "Store" << storeName << "does not exist!";
        return;
    }

    m_stores.remove(storeName.toString());
    saveConfig();
    emit storeReset(storeName.toString());
}

bool GlobalConfig::hasStore(QStringView storeName) const noexcept {
    return m_stores.contains(storeName.toString());
}

QStringList GlobalConfig::getStoreNames() const noexcept {
    return m_stores.keys();
}

void GlobalConfig::clearAllStores() noexcept {
    m_stores = QJsonObject();
    saveConfig();
    emit allStoresCleared();
}

void GlobalConfig::subscribeToStore(QStringView storeName,
                                    const std::shared_ptr<Callback> &callback) {
    m_subscribers[storeName.toString()][this] = callback;
}

void GlobalConfig::unsubscribeFromStore(QStringView storeName,
                                        void *subscriber) noexcept {
    if (m_subscribers.count(storeName.toString()) > 0) {
        m_subscribers[storeName.toString()].erase(subscriber);
    }
}

void GlobalConfig::importConfig(const QString &filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray saveData = file.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
        m_stores = loadDoc.object();
        saveConfig();
    } else {
        qWarning() << "Failed to import config from" << filePath;
    }
}

void GlobalConfig::exportConfig(const QString &filePath) const {
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument saveDoc(m_stores);
        file.write(saveDoc.toJson());
    } else {
        qWarning() << "Failed to export config to" << filePath;
    }
}