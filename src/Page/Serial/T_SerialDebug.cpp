#include "T_SerialDebug.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTextStream>
#include <QVBoxLayout>

#include "ElaPushButton.h"
#include "ElaText.h"
#include "ElaCheckBox.h"
#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaPlainTextEdit.h"

T_SerialDebugPage::T_SerialDebugPage(QWidget *parent)
    : T_BasePage(parent), serial(new QSerialPort(this)) {
    setupUI();  // 创建界面

    // 自动检测串口
    updateSerialPorts();

    // 当串口有数据可读时，连接到处理函数
    connect(serial, &QSerialPort::readyRead, this,
            &T_SerialDebugPage::readSerialData);

    // 检测串口设备的插拔
    connect(refreshSerialButton, &ElaPushButton::clicked, this,
            &T_SerialDebugPage::updateSerialPorts);
}

T_SerialDebugPage::~T_SerialDebugPage() {}

void T_SerialDebugPage::setupUI() {
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 上部的串口设置区域
    QHBoxLayout *settingsLayout = new QHBoxLayout();

    // 串口选择
    serialPortComboBox = new ElaComboBox();
    refreshSerialButton = new ElaPushButton("刷新串口");
    settingsLayout->addWidget(serialPortComboBox);
    settingsLayout->addWidget(refreshSerialButton);

    // 波特率选择
    baudRateComboBox = new ElaComboBox();
    baudRateComboBox->addItem("9600", QSerialPort::Baud9600);
    baudRateComboBox->addItem("115200", QSerialPort::Baud115200);
    settingsLayout->addWidget(baudRateComboBox);

    // 数据位选择
    dataBitsComboBox = new ElaComboBox();
    dataBitsComboBox->addItem("8", QSerialPort::Data8);
    dataBitsComboBox->addItem("7", QSerialPort::Data7);
    settingsLayout->addWidget(dataBitsComboBox);

    // 停止位选择
    stopBitsComboBox = new ElaComboBox();
    stopBitsComboBox->addItem("1", QSerialPort::OneStop);
    stopBitsComboBox->addItem("2", QSerialPort::TwoStop);
    settingsLayout->addWidget(stopBitsComboBox);

    // 校验位选择
    parityComboBox = new ElaComboBox();
    parityComboBox->addItem("None", QSerialPort::NoParity);
    parityComboBox->addItem("Even", QSerialPort::EvenParity);
    parityComboBox->addItem("Odd", QSerialPort::OddParity);
    settingsLayout->addWidget(parityComboBox);

    // 流控制选择
    flowControlComboBox = new ElaComboBox();
    flowControlComboBox->addItem("None", QSerialPort::NoFlowControl);
    flowControlComboBox->addItem("Hardware", QSerialPort::HardwareControl);
    flowControlComboBox->addItem("Software", QSerialPort::SoftwareControl);
    settingsLayout->addWidget(flowControlComboBox);

    // 添加到主布局
    mainLayout->addLayout(settingsLayout);

    // 打开/关闭串口按钮
    openSerialButton = new ElaPushButton("打开串口");
    mainLayout->addWidget(openSerialButton);
    connect(openSerialButton, &ElaPushButton::clicked, this,
            &T_SerialDebugPage::on_openSerialButton_clicked);

    // 数据接收区
    receiveTextEdit = new ElaPlainTextEdit();
    receiveTextEdit->setReadOnly(true);
    mainLayout->addWidget(receiveTextEdit);

    // 发送区
    QHBoxLayout *sendLayout = new QHBoxLayout();
    sendLineEdit = new ElaLineEdit();
    sendButton = new ElaPushButton("发送");
    sendLayout->addWidget(sendLineEdit);
    sendLayout->addWidget(sendButton);
    mainLayout->addLayout(sendLayout);
    connect(sendButton, &ElaPushButton::clicked, this,
            &T_SerialDebugPage::on_sendButton_clicked);

    // 时间戳选择
    timestampCheckBox = new ElaCheckBox("添加时间戳");
    mainLayout->addWidget(timestampCheckBox);

    // 接收、发送字节数显示
    QHBoxLayout *statusLayout = new QHBoxLayout();
    rxLabel = new ElaText("RX: 0");
    txLabel = new ElaText("TX: 0");
    statusLayout->addWidget(rxLabel);
    statusLayout->addWidget(txLabel);
    mainLayout->addLayout(statusLayout);

    // 清屏、保存日志按钮
    QHBoxLayout *actionLayout = new QHBoxLayout();
    clearScreenButton = new ElaPushButton("清屏");
    saveLogButton = new ElaPushButton("保存日志");
    actionLayout->addWidget(clearScreenButton);
    actionLayout->addWidget(saveLogButton);
    mainLayout->addLayout(actionLayout);

    connect(clearScreenButton, &ElaPushButton::clicked, this,
            &T_SerialDebugPage::on_clearScreenButton_clicked);
    connect(saveLogButton, &ElaPushButton::clicked, this,
            &T_SerialDebugPage::on_saveLogButton_clicked);

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("AACore调试终端");
    auto *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

void T_SerialDebugPage::updateSerialPorts() {
    serialPortComboBox->clear();
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : ports) {
        serialPortComboBox->addItem(info.portName());
    }
}

void T_SerialDebugPage::on_openSerialButton_clicked() {
    if (serial->isOpen()) {
        serial->close();
        openSerialButton->setText("打开串口");
        return;
    }

    // 设置串口参数
    serial->setPortName(serialPortComboBox->currentText());
    serial->setBaudRate(static_cast<QSerialPort::BaudRate>(
        baudRateComboBox->currentData().toInt()));
    serial->setDataBits(static_cast<QSerialPort::DataBits>(
        dataBitsComboBox->currentData().toInt()));
    serial->setParity(static_cast<QSerialPort::Parity>(
        parityComboBox->currentData().toInt()));
    serial->setStopBits(static_cast<QSerialPort::StopBits>(
        stopBitsComboBox->currentData().toInt()));
    serial->setFlowControl(static_cast<QSerialPort::FlowControl>(
        flowControlComboBox->currentData().toInt()));

    if (serial->open(QIODevice::ReadWrite)) {
        openSerialButton->setText("关闭串口");
        receivedBytes = 0;
        sentBytes = 0;
        rxLabel->setText("RX: 0");
        txLabel->setText("TX: 0");
    } else {
        QMessageBox::critical(this, "错误", "无法打开串口");
    }
}

void T_SerialDebugPage::on_sendButton_clicked() {
    if (serial->isOpen()) {
        QString data = sendLineEdit->text();
        if (!data.isEmpty()) {
            serial->write(data.toUtf8());
            sentBytes += data.size();
            txLabel->setText(QString("TX: %1").arg(sentBytes));
        }
    }
}

void T_SerialDebugPage::readSerialData() {
    if (serial->isOpen()) {
        QByteArray data = serial->readAll();
        receivedBytes += data.size();
        rxLabel->setText(QString("RX: %1").arg(receivedBytes));

        QString receivedData;
        if (timestampCheckBox->isChecked()) {
            QString timestamp = QDateTime::currentDateTime().toString(
                "yyyy-MM-dd hh:mm:ss.zzz");
            receivedData.append("[" + timestamp + "] ");
        }
        receivedData.append(QString::fromUtf8(data));
        receiveTextEdit->insertPlainText(receivedData);
    }
}

void T_SerialDebugPage::on_clearScreenButton_clicked() {
    receiveTextEdit->clear();
}

void T_SerialDebugPage::on_saveLogButton_clicked() {
    QString fileName =
        QFileDialog::getSaveFileName(this, "保存日志", "", "文本文件 (*.txt)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法保存文件");
        return;
    }

    QTextStream out(&file);
    out << receiveTextEdit->toPlainText();
    file.close();
}
