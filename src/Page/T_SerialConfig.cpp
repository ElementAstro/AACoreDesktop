#include "T_SerialConfig.h"
#include "Connection/SerialScanner.h"

#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
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
    // 执行一些任务
    SerialPortScanner scanner;
    auto result = scanner.scanPorts();
    QList<QString> names;
    for (auto &port : result) {
        names.append(port.portName);
    }
    emit scanFinished(names);
}

T_SerialConfig::T_SerialConfig(QWidget *parent) : T_BasePage(parent) {
    auto *comLabel = new ElaText("Com端口", this);
    comLabel->setTextPixelSize(15);
    comComboBox = new ElaComboBox(this);

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

    connect(comComboBox, QOverload<int>::of(&ElaComboBox::currentIndexChanged),
            this, &T_SerialConfig::onComPortChanged);

    auto *dcGroupBox = new QGroupBox("DC Channels", this);
    auto *dcLayout = new QFormLayout();

    QStringList dcLabels = {"DC1", "DC2", "DC3", "DC4", "DC5", "DC6", "DC7"};
    QStringList dcDescriptions = {"主镜供电",   "主相机供电", "赤道仪供电",
                                  "滤镜轮供电", "电调供电",   "N/C",
                                  "N/C"};

    for (int i = 0; i < dcLabels.size(); ++i) {
        auto *lineEdit = new ElaLineEdit(this);
        auto *checkBox = new ElaCheckBox(dcDescriptions[i], this);
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

    auto *pwmGroupBox = new QGroupBox("PWM Channels", this);
    auto *pwmLayout = new QFormLayout();

    QStringList pwmLabels = {"PWM1", "PWM2", "PWM3"};
    QStringList pwmDescriptions = {"主镜自动应加热", "平场板亮度",
                                   "主机自动应加热"};

    for (int i = 0; i < pwmLabels.size(); ++i) {
        auto *lineEdit = new ElaLineEdit(this);
        auto *checkBox = new ElaCheckBox(pwmDescriptions[i], this);
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

    auto *imageLabel = new ElaText(this);
    QPixmap pixmap(":/images/ascom_logo.png");
    imageLabel->setPixmap(pixmap.scaled(50, 50, Qt::KeepAspectRatio));

    auto *okButton = new ElaPushButton("OK", this);
    connect(okButton, &ElaPushButton::clicked, this,
            &T_SerialConfig::onOkClicked);

    auto *cancelButton = new ElaPushButton("Cancel", this);
    connect(cancelButton, &ElaPushButton::clicked, this,
            &T_SerialConfig::onCancelClicked);

    auto *traceCheckBox = new ElaCheckBox("Trace on", this);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(traceCheckBox);
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(comLabel);
    mainLayout->addWidget(comComboBox);
    mainLayout->addWidget(dcGroupBox);
    mainLayout->addWidget(pwmGroupBox);
    mainLayout->addWidget(imageLabel);
    mainLayout->addLayout(buttonLayout);

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("AACore配置");
    auto *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

T_SerialConfig::~T_SerialConfig() {}

void T_SerialConfig::onOkClicked() {
    QMessageBox::information(this, "OK", "Configuration has been saved.");
}

void T_SerialConfig::onCancelClicked() {
    QMessageBox::warning(this, "Cancel", "Configuration has been canceled.");
    close();
}

void T_SerialConfig::onComPortChanged(int index) {}

void T_SerialConfig::updateComPorts(const QStringList &ports) {
    comComboBox->clear();
    comComboBox->addItems(ports);
}