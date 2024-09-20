#include "Http.h"

#include <QFileInfo>
#include <QJsonValueRef>
#include <QtNetwork/QHttpMultiPart>
#include <QtNetwork/QHttpPart>
#include <QtNetwork/QRestReply>

HttpRequestCenter::HttpRequestCenter(QObject *parent)
    : QObject(parent),
      m_networkManager(new QNetworkAccessManager(this)),
      m_globalTimeout(30000)  // 30 seconds default
      ,
      m_maxRetries(3) {
    connect(m_networkManager, &QNetworkAccessManager::finished, this,
            &HttpRequestCenter::onRequestFinished);
}

HttpRequestCenter::~HttpRequestCenter() {
    // QNetworkAccessManager is deleted automatically as it's a child of this
    // object
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

void HttpRequestCenter::setDefaultHeaders(QNetworkRequest &request) {
    for (auto it = m_defaultHeaders.constBegin();
         it != m_defaultHeaders.constEnd(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }
}

void HttpRequestCenter::processNextRequest() {
    HttpRequest *request = nullptr;
    if (!m_highPriorityQueue.isEmpty()) {
        request = m_highPriorityQueue.dequeue();
    } else if (!m_normalPriorityQueue.isEmpty()) {
        request = m_normalPriorityQueue.dequeue();
    } else if (!m_lowPriorityQueue.isEmpty()) {
        request = m_lowPriorityQueue.dequeue();
    }

    if (request) {
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
                    multiPart->setParent(reply);  // multiPart will be deleted
                                                  // when reply is deleted
                } else {
                    // 处理 multiPart 为 nullptr 的情况
                }
            } else {
                QJsonDocument jsonDoc(request->m_data);
                reply =
                    m_networkManager->post(networkRequest, jsonDoc.toJson());
            }
        } else if (request->m_method == "PUT") {
            QJsonDocument jsonDoc(request->m_data);
            reply = m_networkManager->put(networkRequest, jsonDoc.toJson());
        } else if (request->m_method == "DELETE") {
            reply = m_networkManager->deleteResource(networkRequest);
        }

        if (reply) {
            QTimer::singleShot(request->m_timeout, reply,
                               &QNetworkReply::abort);
            connect(reply, &QNetworkReply::errorOccurred,
                    [this, request, reply](QNetworkReply::NetworkError) {
                        if (request->m_retryCount < m_maxRetries) {
                            request->m_retryCount++;
                            enqueueRequest(request);
                        } else {
                            emit requestError(request, reply->errorString());
                        }
                    });
        }
    }
}

void HttpRequestCenter::enqueueRequest(HttpRequest *request) {
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

void HttpRequestCenter::onRequestFinished(QNetworkReply *reply) {
    HttpRequest *request = reply->property("request").value<HttpRequest *>();

    if (m_responseInterceptor) {
        m_responseInterceptor(reply);
    }

    if (reply->error() == QNetworkReply::NoError) {
        int statusCode =
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QByteArray response = reply->readAll();
        emit requestFinished(request, statusCode, response);
    } else {
        emit requestError(request, reply->errorString());
    }

    reply->deleteLater();
    delete request;

    processNextRequest();
}

HttpRequest::HttpRequest(HttpRequestCenter *center, const QString &url,
                         const QString &method, const QJsonObject &data,
                         Priority priority)
    : m_center(center),
      m_url(url),
      m_method(method),
      m_data(data),
      m_retryCount(0),
      m_timeout(10),
      m_priority(priority) {}

void HttpRequest::setRetryCount(int count) { m_retryCount = count; }

void HttpRequest::setTimeout(int msecs) { m_timeout = msecs; }

void HttpRequest::setPriority(Priority priority) { m_priority = priority; }