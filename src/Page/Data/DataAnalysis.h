#ifndef DATA_ANALYSIS_H
#define DATA_ANALYSIS_H

#include <QVector>
#include <QString>
#include <QDateTime>
#include <QLoggingCategory>

#include "DataLoader.h"

namespace DataAnalysis {

// 扩展分析结果结构体
struct AnalysisResult {
    double average;
    double max;
    double min;
    double median;
    double stdDev;
    double volatility;
    double skewness;        // 偏度
    double kurtosis;        // 峰度
    double slope;           // 线性回归斜率
    double rSquared;        // R方值
    QString trend;          // 趋势描述
    QDateTime startTime;    // 分析开始时间
    QDateTime endTime;      // 分析结束时间
    int sampleCount;        // 样本数量
};

class DataAnalyzer {
public:
    DataAnalyzer();
    ~DataAnalyzer();

    // 主分析函数
    AnalysisResult analyzeData(const QVector<TemperatureData>& data);

    // 获取最后的错误信息
    QString getLastError() const;

    // 设置分析参数
    void setOutlierThreshold(double threshold);
    void setConfidenceInterval(double interval);

private:
    // 数据验证
    bool validateData(const QVector<TemperatureData>& data);
    
    // 计算统计指标
    double calculateSkewness(const QVector<double>& values, double mean, double stdDev);
    double calculateKurtosis(const QVector<double>& values, double mean, double stdDev);
    void calculateLinearRegression(const QVector<double>& values, double& slope, double& rSquared);
    QString analyzeTrend(const QVector<double>& values);
    
    // 异常值检测
    QVector<double> removeOutliers(const QVector<double>& values);

    double m_outlierThreshold;
    double m_confidenceInterval;
    QString m_lastError;
    QLoggingCategory m_logger;
};

} // namespace DataAnalysis

#endif // DATA_ANALYSIS_H