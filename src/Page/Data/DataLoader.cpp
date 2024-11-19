#include "DataLoader.h"
#include <QDebug>
#include <QSqlRecord>
#include <QFileInfo>

Q_LOGGING_CATEGORY(dataLoader, "app.dataloader")

DataLoader::DataLoader(const QString& dbPath)
    : m_dbPath(dbPath)
    , m_cache(DEFAULT_CACHE_SIZE)
{
    if (!connectToDatabase()) {
        throw DataLoaderError("Failed to initialize database connection");
    }
}

DataLoader::~DataLoader() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DataLoader::connectToDatabase() {
    if (!QFileInfo::exists(m_dbPath)) {
        qCCritical(dataLoader) << "Database file does not exist:" << m_dbPath;
        return false;
    }

    if (!m_db.isValid()) {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName(m_dbPath);
    }

    if (!m_db.open()) {
        logDatabaseError("Opening database", m_db.lastError());
        return false;
    }

    qCInfo(dataLoader) << "Successfully connected to database:" << m_dbPath;
    return true;
}

QVector<TemperatureData> DataLoader::loadTemperatureData(
    const QString& type,
    const QDateTime& startTime,
    const QDateTime& endTime,
    const QString& location)
{
    const QString cacheKey = generateCacheKey(type, startTime, endTime);
    if (auto cachedData = m_cache.object(cacheKey)) {
        qCDebug(dataLoader) << "Cache hit for:" << cacheKey;
        return *cachedData;
    }

    QVector<TemperatureData> data;
    QSqlQuery query(m_db);
    QString queryStr = "SELECT timestamp, temperature, type, location "
                      "FROM temperatures WHERE type = :type "
                      "AND timestamp BETWEEN :start AND :end";
    
    if (!location.isEmpty()) {
        queryStr += " AND location = :location";
    }
    
    query.prepare(queryStr);
    query.bindValue(":type", type);
    query.bindValue(":start", startTime.toString(Qt::ISODate));
    query.bindValue(":end", endTime.toString(Qt::ISODate));
    
    if (!location.isEmpty()) {
        query.bindValue(":location", location);
    }

    if (!query.exec()) {
        logDatabaseError("Executing query", query.lastError());
        throw DataLoaderError("Query execution failed");
    }

    while (query.next()) {
        TemperatureData entry;
        entry.timestamp = QDateTime::fromString(
            query.value("timestamp").toString(), Qt::ISODate);
        entry.temperature = query.value("temperature").toDouble();
        entry.type = query.value("type").toString();
        entry.location = query.value("location").toString();
        
        if (validateData(entry)) {
            data.append(entry);
        } else {
            qCWarning(dataLoader) << "Invalid data record found:" 
                                << entry.timestamp.toString()
                                << entry.temperature;
        }
    }

    m_cache.insert(cacheKey, new QVector<TemperatureData>(data));
    qCDebug(dataLoader) << "Loaded" << data.size() << "records for type:" << type;
    return data;
}

bool DataLoader::validateData(TemperatureData& data) {
    if (!data.timestamp.isValid()) {
        qCWarning(dataLoader) << "Invalid timestamp detected";
        return false;
    }

    if (!data.isValidTemperature()) {
        qCWarning(dataLoader) << "Temperature out of valid range:" 
                            << data.temperature;
        return false;
    }

    return true;
}

void DataLoader::logDatabaseError(const QString& operation, 
                                const QSqlError& error) 
{
    qCCritical(dataLoader) << "Database error during" << operation
                          << "Type:" << error.type()
                          << "Error:" << error.text();
}

QString DataLoader::generateCacheKey(const QString& type,
                                   const QDateTime& startTime,
                                   const QDateTime& endTime)
{
    return QString("%1_%2_%3")
        .arg(type)
        .arg(startTime.toString(Qt::ISODate))
        .arg(endTime.toString(Qt::ISODate));
}

// 其他方法实现...