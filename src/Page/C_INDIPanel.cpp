#include "C_INDIPanel.h"
#include <QDateTime>
#include <QFile>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStyledItemDelegate>
#include <QVBoxLayout>

#include "ElaCheckBox.h"
#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaPlainTextEdit.h"
#include "ElaPushButton.h"
#include "ElaSlider.h"
#include "ElaTabWidget.h"
#include "ElaText.h"

C_INDIPanel::C_INDIPanel(const QString &configFile, QWidget *parent)
    : QWidget(parent) {
    setupUI();
    loadConfiguration(configFile);
    applyStyles();
}

void C_INDIPanel::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    tabWidget = new ElaTabWidget(this);  // 使用ElaTabWidget替换QTabWidget
    mainLayout->addWidget(tabWidget);

    logTextEdit = new ElaPlainTextEdit(this);
    logTextEdit->setReadOnly(true);
    logTextEdit->setMaximumHeight(150);
    mainLayout->addWidget(logTextEdit);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    ElaPushButton *clearButton = new ElaPushButton("Clear", this);
    ElaPushButton *closeButton = new ElaPushButton("Close", this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(clearButton);
    buttonLayout->addWidget(closeButton);
    mainLayout->addLayout(buttonLayout);

    connect(clearButton, &ElaPushButton::clicked, logTextEdit,
            &QPlainTextEdit::clear);
    connect(closeButton, &ElaPushButton::clicked, this, &QWidget::close);
}

void C_INDIPanel::loadConfiguration(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        log("Failed to open configuration file");
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        log("Failed to parse configuration file");
        return;
    }

    createTabs(doc.object());
}

void C_INDIPanel::createTabs(const QJsonObject &config) {
    QJsonArray tabs = config["tabs"].toArray();
    for (const QJsonValue &tabValue : tabs) {
        QJsonObject tabObject = tabValue.toObject();
        QString tabName = tabObject["name"].toString();
        QWidget *tabWidget = new QWidget();
        QVBoxLayout *tabLayout = new QVBoxLayout(tabWidget);

        QJsonArray groups = tabObject["groups"].toArray();
        for (const QJsonValue &groupValue : groups) {
            QJsonObject groupObject = groupValue.toObject();
            QString groupName = groupObject["name"].toString();
            QGroupBox *groupBox = new QGroupBox(groupName);
            QVBoxLayout *groupLayout = new QVBoxLayout(groupBox);

            QJsonArray controls = groupObject["controls"].toArray();
            for (const QJsonValue &controlValue : controls) {
                QWidget *controlWidget =
                    createControlWidget(controlValue.toObject());
                if (controlWidget) {
                    groupLayout->addWidget(controlWidget);
                }
            }

            tabLayout->addWidget(groupBox);
        }

        tabLayout->addStretch();
        this->tabWidget->addTab(tabWidget, tabName);
    }
}

QWidget *C_INDIPanel::createControlWidget(const QJsonObject &control) {
    QString type = control["type"].toString();
    QString name = control["name"].toString();

    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(widget);

    ElaText *label = new ElaText(name);
    label->setTextPixelSize(15);

    layout->addWidget(label);

    if (type == "button") {
        ElaPushButton *button = new ElaPushButton(control["text"].toString());
        layout->addWidget(button);
        connect(button, &ElaPushButton::clicked, this,
                &C_INDIPanel::onControlValueChanged);
    } else if (type == "input") {
        ElaLineEdit *lineEdit = new ElaLineEdit();
        layout->addWidget(lineEdit);
        connect(lineEdit, &ElaLineEdit::textChanged, this,
                &C_INDIPanel::onControlValueChanged);
    } else if (type == "slider") {
        ElaSlider *slider = new ElaSlider(Qt::Horizontal);
        ElaLineEdit *valueEdit = new ElaLineEdit();
        valueEdit->setMaximumWidth(50);
        layout->addWidget(slider);
        layout->addWidget(valueEdit);
        connect(slider, &ElaSlider::valueChanged, this,
                &C_INDIPanel::onControlValueChanged);
        connect(valueEdit, &ElaLineEdit::textChanged, this,
                &C_INDIPanel::onControlValueChanged);
    } else if (type == "checkbox") {
        ElaCheckBox *checkBox = new ElaCheckBox();
        layout->addWidget(checkBox);
        connect(checkBox, &ElaCheckBox::stateChanged, this,
                &C_INDIPanel::onControlValueChanged);
    }

    // Add status indicator
    ElaText *statusIndicator = new ElaText();
    statusIndicator->setFixedSize(10, 10);
    statusIndicator->setStyleSheet(
        "background-color: #4CAF50; border-radius: 5px;");
    layout->insertWidget(0, statusIndicator);

    controlWidgets[name] = widget;
    return widget;
}

void C_INDIPanel::applyStyles() {
    setStyleSheet(R"(
       
    )");
}

void C_INDIPanel::log(const QString &message) {
    QString timestamp =
        QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    logTextEdit->appendPlainText(QString("[%1] %2").arg(timestamp, message));
}

void C_INDIPanel::onControlValueChanged() {
    // Handle control value changes here
    // You can emit signals or update the device state
    log("Control value changed");
}