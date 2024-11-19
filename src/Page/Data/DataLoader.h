#ifndef DATALOADER_H
#define DATALOADER_H

#include <QCache>
#include <QDateTime>
#include <QLoggingCategory>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVector>
#include <optional>


// 定义日志类别
Q_DECLARE_LOGGING_CATEGORY(dataLoader)

class DataLoaderError : public std::runtime_error {
public:
    explicit DataLoaderError(const QString& message)
        : std::runtime_error(message.toStdString()) {}
};

struct TemperatureData {
    QDateTime timestamp;
    double temperature;
    QString type;
    QString location;  // 新增位置字段
    bool isValid;      // 数据有效性标志

    bool isValidTemperature() const {
        return temperature >= -273.15 && temperature <= 1000.0;
    }
};

class DataLoader {
public:
    explicit DataLoader(const QString& dbPath);
    ~DataLoader();

    // 禁用拷贝
    DataLoader(const DataLoader&) = delete;
    DataLoader& operator=(const DataLoader&) = delete;

    // 主要查询接口
    QVector<TemperatureData> loadTemperatureData(
        const QString& type, const QDateTime& startTime,
        const QDateTime& endTime, const QString& location = QString());

    // 辅助查询接口
    QStringList getAvailableTypes() const;
    QStringList getAvailableLocations() const;
    std::optional<TemperatureData> getLatestReading(const QString& type);
    QPair<double, double> getTemperatureRange(const QString& type);

    // 统计接口
    double getAverageTemperature(const QString& type,
                                 const QDateTime& startTime,
                                 const QDateTime& endTime);

    // 缓存控制
    void clearCache();
    void setCacheSize(int size);

private:
    bool connectToDatabase();
    void logDatabaseError(const QString& operation, const QSqlError& error);
    bool validateData(TemperatureData& data);
    QString generateCacheKey(const QString& type, const QDateTime& startTime,
                             const QDateTime& endTime);

private:
    QString m_dbPath;
    QSqlDatabase m_db;
    mutable QCache<QString, QVector<TemperatureData>> m_cache;
    static constexpr int DEFAULT_CACHE_SIZE = 50;
    static constexpr double MIN_VALID_TEMP = -273.15;
    static constexpr double MAX_VALID_TEMP = 1000.0;
};

#endif  // DATALOADER_H