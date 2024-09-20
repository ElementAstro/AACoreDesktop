#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include <QJsonObject>
#include <QMap>
#include <QObject>
#include <QVariant>
#include <functional>
#include <unordered_map>

class GlobalConfig : public QObject {
    Q_OBJECT

public:
    static GlobalConfig &getInstance();

    template <typename T>
    void defineStore(const QString &storeName, const std::function<T()> &setup);

    template <typename T>
    T useStore(const QString &storeName);

    template <typename T>
    void setState(const QString &storeName, const QString &key, const T &value);

    QVariant getState(const QString &storeName, const QString &key);

    void resetStore(const QString &storeName);

    // 新增功能
    bool hasStore(const QString &storeName) const;
    QStringList getStoreNames() const;
    void clearAllStores();
    void subscribeToStore(const QString &storeName,
                          std::function<void(const QString &)> callback);
    void unsubscribeFromStore(const QString &storeName, void *subscriber);
    void importConfig(const QString &filePath);
    void exportConfig(const QString &filePath) const;

signals:
    void stateChanged(const QString &storeName, const QString &key);
    void storeReset(const QString &storeName);
    void allStoresCleared();

private:
    GlobalConfig();
    ~GlobalConfig();
    GlobalConfig(const GlobalConfig &) = delete;
    GlobalConfig &operator=(const GlobalConfig &) = delete;

    void loadConfig();
    void saveConfig() const;

    QJsonObject m_stores;
    std::unordered_map<
        QString,
        std::unordered_map<void *, std::function<void(const QString &)>>>
        m_subscribers;
    QString m_configPath;
};

#define globalConfig GlobalConfig::getInstance()

// 模板函数的实现需要在头文件中
template <typename T>
void GlobalConfig::defineStore(const QString &storeName,
                               const std::function<T()> &setup) {
    if (m_stores.contains(storeName)) {
        qWarning() << "Store" << storeName << "already exists!";
        return;
    }

    T state = setup();
    QVariantMap variantMap;
    for (const auto &[key, value] : state) {
        variantMap[key] = QVariant::fromValue(value);
    }
    m_stores[storeName] = QJsonObject::fromVariantMap(variantMap);
    saveConfig();
}

template <typename T>
T GlobalConfig::useStore(const QString &storeName) {
    if (!m_stores.contains(storeName)) {
        qWarning() << "Store" << storeName << "does not exist!";
        return T();
    }

    QJsonObject storeObject = m_stores[storeName].toObject();
    QVariantMap variantMap = storeObject.toVariantMap();
    T result;
    for (auto it = variantMap.begin(); it != variantMap.end(); ++it) {
        result[it.key()] = it.value().value<typename T::mapped_type>();
    }
    return result;
}

template <typename T>
void GlobalConfig::setState(const QString &storeName, const QString &key,
                            const T &value) {
    if (!m_stores.contains(storeName)) {
        qWarning() << "Store" << storeName << "does not exist!";
        return;
    }

    QJsonObject storeObject = m_stores[storeName].toObject();
    storeObject[key] = QJsonValue::fromVariant(QVariant::fromValue(value));
    m_stores[storeName] = storeObject;
    saveConfig();
    emit stateChanged(storeName, key);

    // 通知订阅者
    if (m_subscribers.count(storeName) > 0) {
        for (const auto &callback : m_subscribers[storeName]) {
            callback.second(key);
        }
    }
}

#endif  // GLOBALCONFIG_H