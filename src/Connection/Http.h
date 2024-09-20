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
#include <functional>


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
  void
  setRequestInterceptor(std::function<void(QNetworkRequest &)> interceptor);
  void setResponseInterceptor(std::function<void(QNetworkReply *)> interceptor);

signals:
  void requestFinished(HttpRequest *request, int statusCode,
                       const QByteArray &response);
  void requestError(HttpRequest *request, const QString &errorString);

private slots:
  void onRequestFinished(QNetworkReply *reply);

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

  void setDefaultHeaders(QNetworkRequest &request);
  void processNextRequest();
  void enqueueRequest(HttpRequest *request);
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

  friend class HttpRequestCenter;
};

#endif // HTTPREQUESTCENTER_H