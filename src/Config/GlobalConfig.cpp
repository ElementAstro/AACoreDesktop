#include "GlobalConfig.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>


GlobalConfig &GlobalConfig::getInstance() {
  static GlobalConfig instance;
  return instance;
}

GlobalConfig::GlobalConfig() : QObject(nullptr) {
  m_configPath = QDir::homePath() + "/.myapp/config.json";
  loadConfig();
}

GlobalConfig::~GlobalConfig() { saveConfig(); }

void GlobalConfig::loadConfig() {
  QFile file(m_configPath);
  if (file.open(QIODevice::ReadOnly)) {
    QByteArray saveData = file.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    m_stores = loadDoc.object();
  }
}

void GlobalConfig::saveConfig() const {
  QDir().mkpath(QFileInfo(m_configPath).path());
  QFile file(m_configPath);
  if (file.open(QIODevice::WriteOnly)) {
    QJsonDocument saveDoc(m_stores);
    file.write(saveDoc.toJson());
  }
}

QVariant GlobalConfig::getState(const QString &storeName, const QString &key) {
  if (!m_stores.contains(storeName)) {
    qWarning() << "Store" << storeName << "does not exist!";
    return QVariant();
  }

  QJsonObject storeObject = m_stores[storeName].toObject();
  return storeObject[key].toVariant();
}

void GlobalConfig::resetStore(const QString &storeName) {
  if (!m_stores.contains(storeName)) {
    qWarning() << "Store" << storeName << "does not exist!";
    return;
  }

  m_stores.remove(storeName);
  saveConfig();
  emit storeReset(storeName);
}

bool GlobalConfig::hasStore(const QString &storeName) const {
  return m_stores.contains(storeName);
}

QStringList GlobalConfig::getStoreNames() const { return m_stores.keys(); }

void GlobalConfig::clearAllStores() {
  m_stores = QJsonObject();
  saveConfig();
  emit allStoresCleared();
}

void GlobalConfig::subscribeToStore(
    const QString &storeName, std::function<void(const QString &)> callback) {
  m_subscribers[storeName][this] = callback;
}

void GlobalConfig::unsubscribeFromStore(const QString &storeName,
                                        void *subscriber) {
  if (m_subscribers.count(storeName) > 0) {
    m_subscribers[storeName].erase(subscriber);
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