#include "T_HttpClient.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QJsonObject>
#include <QMessageBox>


#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaPlainTextEdit.h"
#include "ElaPushButton.h"
#include "ElaText.h"


T_HttpClientPage::T_HttpClientPage(QWidget *parent)
    : QWidget(parent), m_requestCenter(new HttpRequestCenter(this)) {
    initLayout();

    connect(m_requestCenter, &HttpRequestCenter::requestFinished, this,
            &T_HttpClientPage::handleRequestFinished);
    connect(m_requestCenter, &HttpRequestCenter::requestError, this,
            &T_HttpClientPage::handleRequestError);
}

T_HttpClientPage::~T_HttpClientPage() {}

void T_HttpClientPage::initLayout() {
    // Initialize UI elements
    urlLineEdit = new ElaLineEdit(this);
    methodComboBox = new ElaComboBox(this);
    dataLineEdit = new ElaLineEdit(this);
    responseTextEdit = new ElaPlainTextEdit(this);
    requestHeaderLineEdit = new ElaLineEdit(this);
    timeoutLineEdit = new ElaLineEdit(this);
    sendButton = new ElaPushButton("Send", this);
    clearResponseButton = new ElaPushButton("Clear Response", this);

    // Set up method selection
    methodComboBox->addItems({"GET", "POST", "PUT", "DELETE"});

    // Create a layout for the request group
    QGroupBox *requestGroup = new QGroupBox("HTTP Request", this);
    QGridLayout *requestLayout = new QGridLayout(requestGroup);
    requestLayout->addWidget(new QLabel("URL:"), 0, 0);
    requestLayout->addWidget(urlLineEdit, 0, 1);
    requestLayout->addWidget(new QLabel("Method:"), 1, 0);
    requestLayout->addWidget(methodComboBox, 1, 1);
    requestLayout->addWidget(new QLabel("Data:"), 2, 0);
    requestLayout->addWidget(dataLineEdit, 2, 1);
    requestLayout->addWidget(new QLabel("Request Header:"), 3, 0);
    requestLayout->addWidget(requestHeaderLineEdit, 3, 1);
    requestLayout->addWidget(new QLabel("Timeout (ms):"), 4, 0);
    requestLayout->addWidget(timeoutLineEdit, 4, 1);
    requestLayout->addWidget(sendButton, 5, 0);
    requestLayout->addWidget(clearResponseButton, 5, 1);

    // Create a layout for the response group
    QGroupBox *responseGroup = new QGroupBox("HTTP Response", this);
    QVBoxLayout *responseLayout = new QVBoxLayout(responseGroup);
    responseLayout->addWidget(responseTextEdit);

    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(requestGroup);
    mainLayout->addWidget(responseGroup);
    setLayout(mainLayout);

    // Connect button actions to slots
    connect(sendButton, &ElaPushButton::clicked, this,
            &T_HttpClientPage::on_sendButton_clicked);
    connect(clearResponseButton, &ElaPushButton::clicked, this,
            &T_HttpClientPage::on_clearResponseButton_clicked);
}

void T_HttpClientPage::on_sendButton_clicked() {
    QString url = urlLineEdit->text();
    QString method = methodComboBox->currentText();
    QJsonObject data;

    if (method != "GET") {
        data["exampleKey"] = dataLineEdit->text();
    }

    // Optionally configure request headers
    QString requestHeader = requestHeaderLineEdit->text();
    if (!requestHeader.isEmpty()) {
        // Assuming the header is in "Key: Value" format
        QStringList headerParts = requestHeader.split(":");
        if (headerParts.size() == 2) {
            QString key = headerParts[0].trimmed();
            QString value = headerParts[1].trimmed();
            m_requestCenter->setDefaultHeader(key, value);
        }
    }

    // Set timeout if provided
    bool timeoutOk;
    int timeout = timeoutLineEdit->text().toInt(&timeoutOk);
    if (timeoutOk) {
        m_requestCenter->setGlobalTimeout(timeout);
    }

    HttpRequest *request = nullptr;

    if (method == "GET") {
        request = m_requestCenter->get(url);
    } else if (method == "POST") {
        request = m_requestCenter->post(url, data);
    } else if (method == "PUT") {
        request = m_requestCenter->put(url, data);
    } else if (method == "DELETE") {
        request = m_requestCenter->deleteResource(url);
    }

    if (request == nullptr) {
        QMessageBox::warning(this, "Error", "Failed to create request");
    } else {
        clearInputFields();
    }
}

void T_HttpClientPage::on_clearResponseButton_clicked() {
    responseTextEdit->clear();
}

void T_HttpClientPage::handleRequestFinished(HttpRequest *request,
                                             int statusCode,
                                             const QByteArray &response) {
    responseTextEdit->setPlainText(QString("Status Code: %1\nResponse: %2")
                                       .arg(statusCode)
                                       .arg(QString(response)));
}

void T_HttpClientPage::handleRequestError(HttpRequest *request,
                                          const QString &errorString) {
    QMessageBox::warning(this, "Request Error", errorString);
}

void T_HttpClientPage::clearInputFields() {
    urlLineEdit->clear();
    dataLineEdit->clear();
    requestHeaderLineEdit->clear();
    timeoutLineEdit->clear();
}
