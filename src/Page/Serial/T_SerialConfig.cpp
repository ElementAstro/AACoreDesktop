#include "T_SerialConfig.h"
#include "Connection/SerialScanner.h"

#include <QFile>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QPixmap>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>

#include "ElaCheckBox.h"
#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "ElaText.h"

void GetSerialPortTask::run() {
    SerialPortScanner scanner;
    auto result = scanner.scanPorts();
    QStringList names;
    for (const auto &port : result) {
        names.append(port.portName);
    }
    emit scanFinished(names);
}

T_SerialConfig::T_SerialConfig(QWidget *parent) : T_BasePage(parent) {
    setupUi();
    setupConnections();

    loadConfig();
}

T_SerialConfig::~T_SerialConfig() = default;

void T_SerialConfig::setupUi() {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);

    // ComPort section
    auto *comLabel = new ElaText("Com端口", this);
    comLabel->setTextPixelSize(15);
    comComboBox = new ElaComboBox(this);
    mainLayout->addWidget(comLabel);
    mainLayout->addWidget(comComboBox);

    // DC channels group
    auto *dcGroupBox = new QGroupBox("DC Channels", this);
    auto *dcLayout = new QFormLayout();
    dcLabels = {"DC1", "DC2", "DC3", "DC4", "DC5", "DC6", "DC7"};
    QStringList dcDescriptions = {"主镜供电",   "主相机供电", "赤道仪供电",
                                  "滤镜轮供电", "电调供电",   "N/C",
                                  "N/C"};

    for (int i = 0; i < dcLabels.size(); ++i) {
        auto *lineEdit = new ElaLineEdit(this);
        auto *checkBox = new ElaCheckBox(dcDescriptions[i], this);
        dcLineEditList.append(lineEdit);
        dcCheckBoxList.append(checkBox);
        auto *widget = new QWidget(this);
        auto *hLayout = new QHBoxLayout(widget);
        hLayout->addWidget(lineEdit);
        hLayout->addWidget(checkBox);
        hLayout->setContentsMargins(0, 0, 0, 0);
        auto *title = new ElaText(dcLabels[i], this);
        title->setTextPointSize(12);
        dcLayout->addRow(title, widget);
    }
    dcGroupBox->setLayout(dcLayout);
    mainLayout->addWidget(dcGroupBox);

    // PWM channels group
    auto *pwmGroupBox = new QGroupBox("PWM Channels", this);
    auto *pwmLayout = new QFormLayout();
    pwmLabels = {"PWM1", "PWM2", "PWM3"};
    QStringList pwmDescriptions = {"主镜自动应加热", "平场板亮度",
                                   "主机自动应加热"};

    for (int i = 0; i < pwmLabels.size(); ++i) {
        auto *lineEdit = new ElaLineEdit(this);
        auto *checkBox = new ElaCheckBox(pwmDescriptions[i], this);
        pwmLineEditList.append(lineEdit);
        pwmCheckBoxList.append(checkBox);
        auto *widget = new QWidget(this);
        auto *hLayout = new QHBoxLayout(widget);
        hLayout->addWidget(lineEdit);
        hLayout->addWidget(checkBox);
        hLayout->setContentsMargins(0, 0, 0, 0);
        auto *title = new ElaText(pwmLabels[i], this);
        title->setTextPointSize(12);
        pwmLayout->addRow(title, widget);
    }
    pwmGroupBox->setLayout(pwmLayout);
    mainLayout->addWidget(pwmGroupBox);

    // Control buttons and image
    auto *buttonLayout = new QHBoxLayout();
    auto *traceCheckBox = new ElaCheckBox("Trace on", this);
    auto *okButton = new ElaPushButton("OK", this);
    auto *cancelButton = new ElaPushButton("Cancel", this);

    buttonLayout->addWidget(traceCheckBox);
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    // Final layout assembly
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("AACore配置");
    auto *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

void T_SerialConfig::setupConnections() {
    connect(comComboBox, QOverload<int>::of(&ElaComboBox::currentIndexChanged),
            this, &T_SerialConfig::onComPortChanged);

    auto *task = new GetSerialPortTask();
    auto *thread = new QThread();
    task->moveToThread(thread);
    connect(thread, &QThread::started, task, &GetSerialPortTask::run);
    connect(task, &GetSerialPortTask::scanFinished, this,
            &T_SerialConfig::updateComPorts);
    connect(task, &GetSerialPortTask::scanFinished, thread, &QThread::quit);
    connect(task, &GetSerialPortTask::scanFinished, task,
            &GetSerialPortTask::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

void T_SerialConfig::onOkClicked() { saveConfig(); }

void T_SerialConfig::onCancelClicked() {
    QMessageBox::information(this, "Cancel",
                             "Configuration has been canceled.");
    close();
}

void T_SerialConfig::onComPortChanged(int index) {}

void T_SerialConfig::updateComPorts(const QStringList &ports) {
    comComboBox->clear();
    comComboBox->addItems(ports);
}

void T_SerialConfig::saveConfig() {
    QJsonObject config;
    config["comPort"] = comComboBox->currentText();
    QJsonArray dcArray;
    for (int i = 0; i < dcLineEditList.size(); ++i) {
        QJsonObject dcItem;
        dcItem["label"] = dcLabels[i];
        dcItem["value"] = dcLineEditList[i]->text();
        dcItem["enabled"] = dcCheckBoxList[i]->isChecked();
        dcArray.append(dcItem);
    }
    config["dcChannels"] = dcArray;
    QJsonArray pwmArray;
    for (int i = 0; i < pwmLineEditList.size(); ++i) {
        QJsonObject pwmItem;
        pwmItem["label"] = pwmLabels[i];
        pwmItem["value"] = pwmLineEditList[i]->text();
        pwmItem["enabled"] = pwmCheckBoxList[i]->isChecked();
        pwmArray.append(pwmItem);
    }
    config["pwmChannels"] = pwmArray;
    QFile file("config.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(config).toJson());
        file.close();
        QMessageBox::information(this, "Save", "Configuration has been saved.");
    } else {
        QMessageBox::warning(this, "Save Error",
                             "Failed to save configuration.");
    }
}

void T_SerialConfig::loadConfig() {
    QFile file("config.json");
    if (!file.exists())
        return;
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject config = doc.object();

        // 加载串口信息
        QString comPort = config["comPort"].toString();
        int comIndex = comComboBox->findText(comPort);
        if (comIndex != -1) {
            comComboBox->setCurrentIndex(comIndex);
        }

        // 加载 DC 通道配置信息
        QJsonArray dcArray = config["dcChannels"].toArray();
        for (int i = 0; i < dcArray.size() && i < dcLineEditList.size(); ++i) {
            QJsonObject dcItem = dcArray[i].toObject();
            dcLineEditList[i]->setText(dcItem["value"].toString());
            dcCheckBoxList[i]->setChecked(dcItem["enabled"].toBool());
        }

        // 加载 PWM 通道配置信息
        QJsonArray pwmArray = config["pwmChannels"].toArray();
        for (int i = 0; i < pwmArray.size() && i < pwmLineEditList.size();
             ++i) {
            QJsonObject pwmItem = pwmArray[i].toObject();
            pwmLineEditList[i]->setText(pwmItem["value"].toString());
            pwmCheckBoxList[i]->setChecked(pwmItem["enabled"].toBool());
        }
    } else {
        QMessageBox::warning(this, "Load Error",
                             "Failed to load configuration.");
    }
}
