#ifndef HTTPREQUESTCENTER_H
#define HTTPREQUESTCENTER_H

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QQueue>
#include <QTimer>
#include <QUrlQuery>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <functional>
#include <QMutex>
#include <QSemaphore>
#include <QCache>

class HttpRequest;

class HttpRequestCenter : public QObject {
    Q_OBJECT

public:
    explicit HttpRequestCenter(QObject *parent = nullptr);
    ~HttpRequestCenter();

    // HTTP methods
    HttpRequest *get(const QString &url, const QUrlQuery &query = QUrlQuery());
    HttpRequest *post(const QString &url, const QJsonObject &data);
    HttpRequest *put(const QString &url, const QJsonObject &data);
    HttpRequest *deleteResource(const QString &url);
    HttpRequest *uploadFile(const QString &url, const QString &filePath,
                            const QString &fileParamName);

    // Set default headers for all requests
    void setDefaultHeader(const QString &key, const QString &value);

    // Set global timeout for all requests
    void setGlobalTimeout(int msecs);

    // Set maximum number of retries for failed requests
    void setMaxRetries(int retries);

    // Set interceptors
    void setRequestInterceptor(
        std::function<void(QNetworkRequest &)> interceptor);
    void setResponseInterceptor(
        std::function<void(QNetworkReply *)> interceptor);

    // Authentication
    void setBasicAuth(const QString &username, const QString &password);
    void setOAuthToken(const QString &token);

    // Caching
    void enableCaching(bool enable);
    void setCacheDuration(int seconds);

    // Concurrency control
    void setMaxConcurrentRequests(int max);

    // Logging
    void enableLogging(bool enable);
    void setLogFunction(std::function<void(const QString &)> logFunc);

signals:
    void requestFinished(HttpRequest *request, int statusCode,
                         const QByteArray &response);
    void requestError(HttpRequest *request, const QString &errorString);
    void requestProgress(HttpRequest *request, qint64 bytesSent, qint64 bytesTotal);
    void downloadProgress(HttpRequest *request, qint64 bytesReceived, qint64 bytesTotal);

private slots:
    void onRequestFinished(QNetworkReply *reply);
    void onReadyRead();

private:
    QNetworkAccessManager *m_networkManager;
    QHash<QString, QString> m_defaultHeaders;
    int m_globalTimeout;
    int m_maxRetries;
    std::function<void(QNetworkRequest &)> m_requestInterceptor;
    std::function<void(QNetworkReply *)> m_responseInterceptor;

    QQueue<HttpRequest *> m_highPriorityQueue;
    QQueue<HttpRequest *> m_normalPriorityQueue;
    QQueue<HttpRequest *> m_lowPriorityQueue;

    // Authentication
    QString m_basicAuthHeader;
    QString m_oauthToken;

    // Caching
    bool m_cachingEnabled;
    int m_cacheDuration;
    QCache<QString, QByteArray> m_cache;

    // Concurrency control
    QSemaphore *m_semaphore;
    int m_maxConcurrentRequests;

    // Logging
    bool m_loggingEnabled;
    std::function<void(const QString &)> m_logFunction;

    QMutex m_mutex;

    void setDefaultHeaders(QNetworkRequest &request);
    void processNextRequest();
    void enqueueRequest(HttpRequest *request);
    QString generateCacheKey(const QString &url, const QString &method, const QByteArray &data);
    void log(const QString &message);
};

class HttpRequest : public QObject {
    Q_OBJECT

public:
    enum Priority { Low, Normal, High };

    HttpRequest(HttpRequestCenter *center, const QString &url,
                const QString &method, const QJsonObject &data = QJsonObject(),
                Priority priority = Normal);

    void setRetryCount(int count);
    void setTimeout(int msecs);
    void setPriority(Priority priority);
    void cancel();

    QString url() const { return m_url; }
    QString method() const { return m_method; }
    QJsonObject data() const { return m_data; }
    Priority priority() const { return m_priority; }

private:
    HttpRequestCenter *m_center;
    QString m_url;
    QString m_method;
    QJsonObject m_data;
    int m_retryCount;
    int m_timeout;
    Priority m_priority;

    QNetworkReply *m_reply;
    bool m_cancelled;

    friend class HttpRequestCenter;
};

#endif  // HTTPREQUESTCENTER_H