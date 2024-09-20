#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QObject>
#include <QVariant>
#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>


class GlobalConfig : public QObject {
    Q_OBJECT

public:
    static GlobalConfig &getInstance();

    template <typename T>
    void defineStore(const QString &storeName, const std::function<T()> &setup);

    template <typename T>
    std::optional<T> useStore(QStringView storeName) const;

    template <typename T>
    void setState(QStringView storeName, QStringView key, const T &value);

    std::optional<QVariant> getState(QStringView storeName,
                                     QStringView key) const;

    void resetStore(QStringView storeName) noexcept;

    // 新增功能
    bool hasStore(QStringView storeName) const noexcept;
    QStringList getStoreNames() const noexcept;
    void clearAllStores() noexcept;

    using Callback = std::function<void(const QString &)>;
    void subscribeToStore(QStringView storeName,
                          const std::shared_ptr<Callback> &callback);
    void unsubscribeFromStore(QStringView storeName, void *subscriber) noexcept;
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

    void loadConfig() noexcept;
    void saveConfig() const noexcept;

    QJsonObject m_stores;
    std::unordered_map<QString,
                       std::unordered_map<void *, std::shared_ptr<Callback>>>
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
std::optional<T> GlobalConfig::useStore(QStringView storeName) const {
    if (!m_stores.contains(storeName.toString())) {
        qWarning() << "Store" << storeName << "does not exist!";
        return std::nullopt;
    }

    QJsonObject storeObject = m_stores[storeName.toString()].toObject();
    QVariantMap variantMap = storeObject.toVariantMap();
    T result;
    for (auto it = variantMap.begin(); it != variantMap.end(); ++it) {
        result[it.key()] = it.value().value<typename T::mapped_type>();
    }
    return result;
}

template <typename T>
void GlobalConfig::setState(QStringView storeName, QStringView key,
                            const T &value) {
    if (!m_stores.contains(storeName.toString())) {
        qWarning() << "Store" << storeName << "does not exist!";
        return;
    }

    QJsonObject storeObject = m_stores[storeName.toString()].toObject();
    storeObject[key.toString()] =
        QJsonValue::fromVariant(QVariant::fromValue(value));
    m_stores[storeName.toString()] = storeObject;
    saveConfig();
    emit stateChanged(storeName.toString(), key.toString());

    // 通知订阅者
    if (m_subscribers.count(storeName.toString()) > 0) {
        for (const auto &callback : m_subscribers[storeName.toString()]) {
            (*callback.second)(key.toString());
        }
    }
}

#endif  // GLOBALCONFIG_H
