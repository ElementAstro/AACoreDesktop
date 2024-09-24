#ifndef HTTPCLIENTWIDGET_H
#define HTTPCLIENTWIDGET_H

#include <QWidget>

class ElaLineEdit;
class ElaComboBox;
class ElaPlainTextEdit;
class ElaPushButton;

#include "Connection/Http.h"

class T_HttpClientPage : public QWidget {
    Q_OBJECT

public:
    explicit T_HttpClientPage(QWidget *parent = nullptr);
    ~T_HttpClientPage();

private slots:
    void on_sendButton_clicked();
    void on_clearResponseButton_clicked();
    void handleRequestFinished(HttpRequest *request, int statusCode,
                               const QByteArray &response);
    void handleRequestError(HttpRequest *request, const QString &errorString);

private:
    ElaLineEdit *urlLineEdit;
    ElaComboBox *methodComboBox;
    ElaLineEdit *dataLineEdit;
    ElaPlainTextEdit *responseTextEdit;
    ElaLineEdit *requestHeaderLineEdit;
    ElaLineEdit *timeoutLineEdit;
    ElaPushButton *sendButton;
    ElaPushButton *clearResponseButton;

    HttpRequestCenter *m_requestCenter;

    void initLayout();
    void clearInputFields();
};

#endif  // HTTPCLIENTWIDGET_H
