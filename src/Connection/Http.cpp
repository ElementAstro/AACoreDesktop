#include "Http.h"

#include <QAuthenticator>
#include <QFileInfo>
#include <QJsonValueRef>
#include <QtNetwork/QHttpMultiPart>
#include <QtNetwork/QHttpPart>
#include <QtNetwork/QNetworkReply>


HttpRequestCenter::HttpRequestCenter(QObject *parent)
    : QObject(parent),
      m_networkManager(new QNetworkAccessManager(this)),
      m_globalTimeout(30000),  // 30 seconds default
      m_maxRetries(3),
      m_cachingEnabled(false),
      m_cacheDuration(60),  // 1 minute default
      m_maxConcurrentRequests(5),
      m_loggingEnabled(false),
      m_logFunction(nullptr) {
    connect(m_networkManager, &QNetworkAccessManager::finished, this,
            &HttpRequestCenter::onRequestFinished);
    m_semaphore = new QSemaphore(m_maxConcurrentRequests);
}

HttpRequestCenter::~HttpRequestCenter() {
    // QNetworkAccessManager is deleted automatically as it's a child of this
    // object
    delete m_semaphore;
}

HttpRequest *HttpRequestCenter::get(const QString &url,
                                    const QUrlQuery &query) {
    QUrl fullUrl(url);
    fullUrl.setQuery(query);
    return new HttpRequest(this, fullUrl.toString(), "GET");
}

HttpRequest *HttpRequestCenter::post(const QString &url,
                                     const QJsonObject &data) {
    return new HttpRequest(this, url, "POST", data);
}

HttpRequest *HttpRequestCenter::put(const QString &url,
                                    const QJsonObject &data) {
    return new HttpRequest(this, url, "PUT", data);
}

HttpRequest *HttpRequestCenter::deleteResource(const QString &url) {
    return new HttpRequest(this, url, "DELETE");
}

HttpRequest *HttpRequestCenter::uploadFile(const QString &url,
                                           const QString &filePath,
                                           const QString &fileParamName) {
    HttpRequest *request = new HttpRequest(this, url, "POST");
    QFile *file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly)) {
        delete request;
        delete file;
        return nullptr;
    }

    QHttpMultiPart *multiPart =
        new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader,
                       QVariant("application/octet-stream"));
    filePart.setHeader(
        QNetworkRequest::ContentDispositionHeader,
        QVariant("form-data; name=\"" + fileParamName + "\"; filename=\"" +
                 QFileInfo(filePath).fileName() + "\""));
    filePart.setBodyDevice(file);
    file->setParent(
        multiPart);  // File will be deleted when multiPart is deleted
    multiPart->append(filePart);

    request->m_data = QJsonObject();  // Clear any existing data
    request->m_data["multiPart"] =
        QJsonValue::fromVariant(QVariant::fromValue(multiPart));

    return request;
}

void HttpRequestCenter::setDefaultHeader(const QString &key,
                                         const QString &value) {
    m_defaultHeaders[key] = value;
}

void HttpRequestCenter::setGlobalTimeout(int msecs) { m_globalTimeout = msecs; }

void HttpRequestCenter::setMaxRetries(int retries) { m_maxRetries = retries; }

void HttpRequestCenter::setRequestInterceptor(
    std::function<void(QNetworkRequest &)> interceptor) {
    m_requestInterceptor = interceptor;
}

void HttpRequestCenter::setResponseInterceptor(
    std::function<void(QNetworkReply *)> interceptor) {
    m_responseInterceptor = interceptor;
}

void HttpRequestCenter::setBasicAuth(const QString &username,
                                     const QString &password) {
    QByteArray credentials =
        QString("%1:%2").arg(username, password).toLocal8Bit();
    m_basicAuthHeader = "Basic " + credentials.toBase64();
}

void HttpRequestCenter::setOAuthToken(const QString &token) {
    m_oauthToken = "Bearer " + token;
}

void HttpRequestCenter::enableCaching(bool enable) {
    m_cachingEnabled = enable;
}

void HttpRequestCenter::setCacheDuration(int seconds) {
    m_cacheDuration = seconds;
}

void HttpRequestCenter::setMaxConcurrentRequests(int max) {
    QMutexLocker locker(&m_mutex);
    m_maxConcurrentRequests = max;
    m_semaphore->release(m_maxConcurrentRequests - m_semaphore->available());
}

void HttpRequestCenter::enableLogging(bool enable) {
    m_loggingEnabled = enable;
}

void HttpRequestCenter::setLogFunction(
    std::function<void(const QString &)> logFunc) {
    m_logFunction = logFunc;
}

QString HttpRequestCenter::generateCacheKey(const QString &url,
                                            const QString &method,
                                            const QByteArray &data) {
    return QString("%1_%2_%3")
        .arg(url, method, QString::fromUtf8(data.toHex()));
}

void HttpRequestCenter::log(const QString &message) {
    if (m_loggingEnabled && m_logFunction) {
        m_logFunction(message);
    }
}

void HttpRequestCenter::setDefaultHeaders(QNetworkRequest &request) {
    for (auto it = m_defaultHeaders.constBegin();
         it != m_defaultHeaders.constEnd(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    if (!m_basicAuthHeader.isEmpty()) {
        request.setRawHeader("Authorization", m_basicAuthHeader.toUtf8());
    }

    if (!m_oauthToken.isEmpty()) {
        request.setRawHeader("Authorization", m_oauthToken.toUtf8());
    }
}

void HttpRequestCenter::enqueueRequest(HttpRequest *request) {
    QMutexLocker locker(&m_mutex);
    switch (request->m_priority) {
        case HttpRequest::High:
            m_highPriorityQueue.enqueue(request);
            break;
        case HttpRequest::Normal:
            m_normalPriorityQueue.enqueue(request);
            break;
        case HttpRequest::Low:
            m_lowPriorityQueue.enqueue(request);
            break;
    }
    QTimer::singleShot(0, this, &HttpRequestCenter::processNextRequest);
}

void HttpRequestCenter::processNextRequest() {
    if (!m_semaphore->tryAcquire(1)) {
        return;  // Max concurrent requests reached
    }

    HttpRequest *request = nullptr;
    {
        QMutexLocker locker(&m_mutex);
        if (!m_highPriorityQueue.isEmpty()) {
            request = m_highPriorityQueue.dequeue();
        } else if (!m_normalPriorityQueue.isEmpty()) {
            request = m_normalPriorityQueue.dequeue();
        } else if (!m_lowPriorityQueue.isEmpty()) {
            request = m_lowPriorityQueue.dequeue();
        }
    }

    if (!request) {
        m_semaphore->release();
        return;
    }

    // Check cache for GET requests
    if (m_cachingEnabled && request->m_method == "GET") {
        QString cacheKey =
            generateCacheKey(request->m_url, request->m_method, QByteArray());
        if (m_cache.contains(cacheKey)) {
            QByteArray cachedResponse = *m_cache.object(cacheKey);
            emit requestFinished(request, 200, cachedResponse);
            delete request;
            m_semaphore->release();
            return;
        }
    }

    QNetworkRequest networkRequest(request->m_url);
    setDefaultHeaders(networkRequest);

    if (m_requestInterceptor) {
        m_requestInterceptor(networkRequest);
    }

    QNetworkReply *reply = nullptr;
    if (request->m_method == "GET") {
        reply = m_networkManager->get(networkRequest);
    } else if (request->m_method == "POST") {
        if (request->m_data.contains("multiPart")) {
            QJsonValueRef jsonValueRef = request->m_data["multiPart"];
            QHttpMultiPart *multiPart =
                jsonValueRef.toVariant().value<QHttpMultiPart *>();

            if (multiPart) {
                reply = m_networkManager->post(networkRequest, multiPart);
                multiPart->setParent(
                    reply);  // multiPart will be deleted when reply is deleted
            }
        } else {
            QJsonDocument jsonDoc(request->m_data);
            reply = m_networkManager->post(networkRequest, jsonDoc.toJson());
        }
    } else if (request->m_method == "PUT") {
        QJsonDocument jsonDoc(request->m_data);
        reply = m_networkManager->put(networkRequest, jsonDoc.toJson());
    } else if (request->m_method == "DELETE") {
        reply = m_networkManager->deleteResource(networkRequest);
    }

    if (reply) {
        reply->setProperty("request", QVariant::fromValue(
                                          static_cast<HttpRequest *>(request)));
        connect(reply, &QNetworkReply::downloadProgress, this,
                [this, request](qint64 bytesReceived, qint64 bytesTotal) {
                    emit downloadProgress(request, bytesReceived, bytesTotal);
                });
        connect(reply, &QNetworkReply::uploadProgress, this,
                [this, request](qint64 bytesSent, qint64 bytesTotal) {
                    emit requestProgress(request, bytesSent, bytesTotal);
                });

        QTimer::singleShot(request->m_timeout, reply, &QNetworkReply::abort);
    }
}

void HttpRequestCenter::onRequestFinished(QNetworkReply *reply) {
    HttpRequest *request = reply->property("request").value<HttpRequest *>();

    if (m_responseInterceptor) {
        m_responseInterceptor(reply);
    }

    if (reply->error() == QNetworkReply::NoError) {
        int statusCode =
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QByteArray response = reply->readAll();
        if (m_cachingEnabled && request->m_method == "GET" &&
            reply->error() == QNetworkReply::NoError) {
            QString cacheKey = generateCacheKey(
                request->m_url, request->m_method, QByteArray());
            m_cache.insert(cacheKey, new QByteArray(response), m_cacheDuration);
        }
        emit requestFinished(request, statusCode, response);
    } else {
        if (request->m_retryCount < m_maxRetries && !request->m_cancelled) {
            request->m_retryCount++;
            enqueueRequest(request);
        } else {
            emit requestError(request, reply->errorString());
            delete request;
        }
    }

    reply->deleteLater();
    m_semaphore->release();
    processNextRequest();
}

void HttpRequestCenter::onReadyRead() {
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply) {
        HttpRequest *request =
            reply->property("request").value<HttpRequest *>();
        if (request) {
            emit requestProgress(
                request, reply->bytesAvailable(),
                reply->header(QNetworkRequest::ContentLengthHeader)
                    .toLongLong());
        }
    }
}

HttpRequest::HttpRequest(HttpRequestCenter *center, const QString &url,
                         const QString &method, const QJsonObject &data,
                         Priority priority)
    : m_center(center),
      m_url(url),
      m_method(method),
      m_data(data),
      m_retryCount(0),
      m_timeout(10000),  // 10 seconds default
      m_priority(priority),
      m_reply(nullptr),
      m_cancelled(false) {}

void HttpRequest::setRetryCount(int count) { m_retryCount = count; }

void HttpRequest::setTimeout(int msecs) { m_timeout = msecs; }

void HttpRequest::setPriority(Priority priority) { m_priority = priority; }

void HttpRequest::cancel() {
    m_cancelled = true;
    if (m_reply) {
        m_reply->abort();
    }
}