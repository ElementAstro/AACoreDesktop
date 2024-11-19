#include <QHBoxLayout>
#include <QLabel>
#include <QSerialPortInfo>
#include <QSettings>
#include <QVBoxLayout>
#include "T_SwitchConfig.h"

#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaPushButton.h"

T_SwitchConfig::T_SwitchConfig(QWidget *parent)
    : QDialog(parent), serialPortTask(new GetSerialPortTask) {
    setupUi();
    setupConnections();
    loadConfig();

    // 移动串口扫描任务到独立线程
    //serialPortTask->moveToThread(&serialScanThread);
   // connect(&serialScanThread, &QThread::started, serialPortTask,
   //         &GetSerialPortTask::run);
   // connect(serialPortTask, &GetSerialPortTask::scanFinished, this,
   //         &T_SwitchConfig::updateComPorts);
   // serialScanThread.start();
}

T_SwitchConfig::~T_SwitchConfig() {
    serialScanThread.quit();
    serialScanThread.wait();
    delete serialPortTask;
}

void T_SwitchConfig::setupUi() {
    setWindowTitle(tr("串口配置"));
    setModal(true);
    resize(400, 300);

    auto *mainLayout = new QVBoxLayout(this);

    // COM端口选择
    auto *portLayout = new QHBoxLayout;
    auto *portLabel = new QLabel(tr("COM端口:"), this);
    comComboBox = new ElaComboBox(this);
    portLayout->addWidget(portLabel);
    portLayout->addWidget(comComboBox);
    mainLayout->addLayout(portLayout);

    // 波特率选择
    auto *baudRateLayout = new QHBoxLayout;
    auto *baudRateLabel = new QLabel(tr("波特率:"), this);
    baudRateComboBox = new ElaComboBox(this);
    baudRateComboBox->addItems({"9600", "19200", "38400", "57600", "115200"});
    baudRateLayout->addWidget(baudRateLabel);
    baudRateLayout->addWidget(baudRateComboBox);
    mainLayout->addLayout(baudRateLayout);

    // 数据位选择
    auto *dataBitsLayout = new QHBoxLayout;
    auto *dataBitsLabel = new QLabel(tr("数据位:"), this);
    dataBitsComboBox = new ElaComboBox(this);
    dataBitsComboBox->addItems({"5", "6", "7", "8"});
    dataBitsLayout->addWidget(dataBitsLabel);
    dataBitsLayout->addWidget(dataBitsComboBox);
    mainLayout->addLayout(dataBitsLayout);

    // 停止位选择
    auto *stopBitsLayout = new QHBoxLayout;
    auto *stopBitsLabel = new QLabel(tr("停止位:"), this);
    stopBitsComboBox = new ElaComboBox(this);
    stopBitsComboBox->addItems({"1", "1.5", "2"});
    stopBitsLayout->addWidget(stopBitsLabel);
    stopBitsLayout->addWidget(stopBitsComboBox);
    mainLayout->addLayout(stopBitsLayout);

    // 校验位选择
    auto *parityLayout = new QHBoxLayout;
    auto *parityLabel = new QLabel(tr("校验位:"), this);
    parityComboBox = new ElaComboBox(this);
    parityComboBox->addItems({"None", "Odd", "Even", "Mark", "Space"});
    parityLayout->addWidget(parityLabel);
    parityLayout->addWidget(parityComboBox);
    mainLayout->addLayout(parityLayout);

    // 确定取消按钮
    auto *buttonLayout = new QHBoxLayout;
    okButton = new ElaPushButton(tr("确定"), this);
    cancelButton = new ElaPushButton(tr("取消"), this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);
}

void T_SwitchConfig::setupConnections() {
    connect(okButton, &ElaPushButton::clicked, this,
            &T_SwitchConfig::onOkClicked);
    connect(cancelButton, &ElaPushButton::clicked, this,
            &T_SwitchConfig::onCancelClicked);
    connect(comComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &T_SwitchConfig::onComPortChanged);
}

void T_SwitchConfig::onOkClicked() {
    saveConfig();
    accept();
}

void T_SwitchConfig::onCancelClicked() { reject(); }

void T_SwitchConfig::onComPortChanged(int index) {
    // 处理COM端口改变
    Q_UNUSED(index)
}

void T_SwitchConfig::updateComPorts(const QStringList &ports) {
    comComboBox->clear();
    comComboBox->addItems(ports);
}

void T_SwitchConfig::saveConfig() {
    QSettings settings;
    settings.setValue("SwitchConfig/ComPort", comComboBox->currentText());
    settings.setValue("SwitchConfig/BaudRate", baudRateComboBox->currentText());
    settings.setValue("SwitchConfig/DataBits", dataBitsComboBox->currentText());
    settings.setValue("SwitchConfig/StopBits", stopBitsComboBox->currentText());
    settings.setValue("SwitchConfig/Parity", parityComboBox->currentText());
}

void T_SwitchConfig::loadConfig() {
    QSettings settings;
    QString savedPort = settings.value("SwitchConfig/ComPort").toString();
    int index = comComboBox->findText(savedPort);
    if (index >= 0) {
        comComboBox->setCurrentIndex(index);
    }

    QString savedBaudRate = settings.value("SwitchConfig/BaudRate").toString();
    index = baudRateComboBox->findText(savedBaudRate);
    if (index >= 0) {
        baudRateComboBox->setCurrentIndex(index);
    }

    QString savedDataBits = settings.value("SwitchConfig/DataBits").toString();
    index = dataBitsComboBox->findText(savedDataBits);
    if (index >= 0) {
        dataBitsComboBox->setCurrentIndex(index);
    }

    QString savedStopBits = settings.value("SwitchConfig/StopBits").toString();
    index = stopBitsComboBox->findText(savedStopBits);
    if (index >= 0) {
        stopBitsComboBox->setCurrentIndex(index);
    }

    QString savedParity = settings.value("SwitchConfig/Parity").toString();
    index = parityComboBox->findText(savedParity);
    if (index >= 0) {
        parityComboBox->setCurrentIndex(index);
    }
}

void T_SwitchConfig::onEditButtonClicked() {
    // 处理编辑按钮点击
}

void T_SwitchConfig::onLineEditReturnPressed() {
    // 处理行编辑器回车按下
}