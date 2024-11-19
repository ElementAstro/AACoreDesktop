#include "DataAnalysis.h"

#include <QDebug>
#include <algorithm>
#include <cmath>

namespace DataAnalysis {

constexpr double OUTLIER_THRESHOLD = 3.0;
constexpr double CONFIDENCE_INTERVAL = 0.95;
constexpr double Q1_MULTIPLIER = 0.25;
constexpr double Q3_MULTIPLIER = 0.75;
constexpr double STABILITY_THRESHOLD = 0.1;

DataAnalyzer::DataAnalyzer()
    : m_outlierThreshold(OUTLIER_THRESHOLD),
      m_confidenceInterval(CONFIDENCE_INTERVAL),
      m_logger("DataAnalysis") {
    qCDebug(m_logger) << "DataAnalyzer initialized with outlierThreshold:"
                      << m_outlierThreshold
                      << "and confidenceInterval:" << m_confidenceInterval;
}

DataAnalyzer::~DataAnalyzer() { qCDebug(m_logger) << "DataAnalyzer destroyed"; }

auto DataAnalyzer::analyzeData(const QVector<TemperatureData>& data)
    -> AnalysisResult {
    qCDebug(m_logger) << "Starting data analysis for" << data.size()
                      << "samples";
    try {
        if (!validateData(data)) {
            throw std::runtime_error(m_lastError.toStdString());
        }

        QVector<double> values;
        values.reserve(data.size());
        for (const auto& entry : data) {
            values.append(entry.temperature);
        }
        qCDebug(m_logger) << "Collected temperature values";

        // 移除异常值
        auto cleanedValues = removeOutliers(values);
        qCDebug(m_logger) << "Removed outliers, remaining values count:"
                          << cleanedValues.size();

        // 基本统计计算
        double sum =
            std::accumulate(cleanedValues.begin(), cleanedValues.end(), 0.0);
        double avg = sum / static_cast<double>(cleanedValues.size());
        double maxVal =
            *std::max_element(cleanedValues.begin(), cleanedValues.end());
        double minVal =
            *std::min_element(cleanedValues.begin(), cleanedValues.end());
        qCDebug(m_logger) << "Calculated basic statistics: avg =" << avg
                          << ", max =" << maxVal << ", min =" << minVal;

        // 计算中位数
        std::sort(cleanedValues.begin(), cleanedValues.end());
        double median = cleanedValues[cleanedValues.size() / 2];
        qCDebug(m_logger) << "Calculated median:" << median;

        // 计算标准差
        double sqSum =
            std::inner_product(cleanedValues.begin(), cleanedValues.end(),
                               cleanedValues.begin(), 0.0);
        double stdDev = std::sqrt(
            sqSum / static_cast<double>(cleanedValues.size()) - avg * avg);
        qCDebug(m_logger) << "Calculated standard deviation:" << stdDev;

        // 计算高阶统计量
        double skewness = calculateSkewness(cleanedValues, avg, stdDev);
        double kurtosis = calculateKurtosis(cleanedValues, avg, stdDev);
        qCDebug(m_logger) << "Calculated skewness:" << skewness
                          << ", kurtosis:" << kurtosis;

        // 计算线性回归
        double slope, rSquared;
        calculateLinearRegression(cleanedValues, slope, rSquared);
        qCDebug(m_logger) << "Calculated linear regression: slope =" << slope
                          << ", rSquared =" << rSquared;

        // 分析趋势
        QString trend = analyzeTrend(cleanedValues);
        qCDebug(m_logger) << "Analyzed trend:" << trend;

        qCInfo(m_logger) << "Analysis completed successfully for" << data.size()
                         << "samples";

        return AnalysisResult{avg,
                              maxVal,
                              minVal,
                              median,
                              stdDev,
                              maxVal - minVal,
                              skewness,
                              kurtosis,
                              slope,
                              rSquared,
                              trend,
                              data.first().timestamp,
                              data.last().timestamp,
                              static_cast<int>(cleanedValues.size())};

    } catch (const std::exception& e) {
        m_lastError = QString("Analysis failed: %1").arg(e.what());
        qCCritical(m_logger) << m_lastError;
        return {};
    }
}

auto DataAnalyzer::validateData(const QVector<TemperatureData>& data) -> bool {
    qCDebug(m_logger) << "Validating data with" << data.size() << "samples";
    if (data.isEmpty()) {
        m_lastError = "Empty dataset";
        qCWarning(m_logger) << m_lastError;
        return false;
    }

    // 检查时间戳的连续性和有效性
    for (int i = 1; i < data.size(); ++i) {
        if (data[i].timestamp < data[i - 1].timestamp) {
            m_lastError = "Invalid timestamp sequence";
            qCWarning(m_logger) << m_lastError;
            return false;
        }
    }

    qCDebug(m_logger) << "Data validation passed";
    return true;
}

auto DataAnalyzer::removeOutliers(const QVector<double>& values)
    -> QVector<double> {
    qCDebug(m_logger) << "Removing outliers from" << values.size() << "values";
    QVector<double> result = values;

    double q1 = result[static_cast<qsizetype>(result.size() * Q1_MULTIPLIER)];
    double q3 = result[static_cast<qsizetype>(result.size() * Q3_MULTIPLIER)];
    double iqr = q3 - q1;
    double lowerBound = q1 - m_outlierThreshold * iqr;
    double upperBound = q3 + m_outlierThreshold * iqr;

    result.erase(std::remove_if(result.begin(), result.end(),
                                [=](double value) {
                                    return value < lowerBound ||
                                           value > upperBound;
                                }),
                 result.end());

    qCDebug(m_logger) << "Outliers removed, remaining values count:"
                      << result.size();
    return result;
}

auto DataAnalyzer::analyzeTrend(const QVector<double>& values) -> QString {
    qCDebug(m_logger) << "Analyzing trend for" << values.size() << "values";
    if (values.size() < 2) {
        return "Insufficient data";
    }

    double firstQuarter =
        std::accumulate(values.begin(), values.begin() + values.size() / 4,
                        0.0) /
        static_cast<double>(values.size() / 4);
    double lastQuarter =
        std::accumulate(values.end() - values.size() / 4, values.end(), 0.0) /
        static_cast<double>(values.size() / 4);

    if (std::abs(lastQuarter - firstQuarter) < STABILITY_THRESHOLD) {
        return "Stable";
    }
    return lastQuarter > firstQuarter ? "Increasing" : "Decreasing";
}

}  // namespace DataAnalysis