#include "T_SerialDebug.h"

#include <QFile>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QList>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QString>
#include <QTextStream>
#include <QTimer>
#include <QVBoxLayout>

#include "ElaCheckBox.h"
#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaPlainTextEdit.h"
#include "ElaPushButton.h"
#include "ElaText.h"

constexpr int RECONNECT_DELAY_MS = 1000;

T_SerialDebugPage::T_SerialDebugPage(QWidget *parent)
    : T_BasePage(parent),
      serial(new QSerialPort(this)),
      openSerialButton(nullptr),
      sendButton(nullptr),
      clearScreenButton(nullptr),
      saveLogButton(nullptr),
      refreshSerialButton(nullptr),
      autoReconnectCheckBox(nullptr),
      newlineCheckBox(nullptr),
      serialPortComboBox(nullptr),
      baudRateComboBox(nullptr),
      dataBitsComboBox(nullptr),
      stopBitsComboBox(nullptr),
      parityComboBox(nullptr),
      flowControlComboBox(nullptr),
      sendLineEdit(nullptr),
      receiveTextEdit(nullptr),
      rxLabel(nullptr),
      txLabel(nullptr) {
    setupUI();          // 创建界面
    loadSendHistory();  // 加载发送历史

    // 自动检测串口
    updateSerialPorts();

    connect(serial, &QSerialPort::readyRead, this,
            &T_SerialDebugPage::readSerialData);
    connect(refreshSerialButton, &ElaPushButton::clicked, this,
            &T_SerialDebugPage::updateSerialPorts);

    connect(serial, &QSerialPort::errorOccurred, this, [this]() {
        if (autoReconnectCheckBox->isChecked() && !serial->isOpen()) {
            QTimer::singleShot(RECONNECT_DELAY_MS, this,
                               &T_SerialDebugPage::handleReconnect);
        }
    });
}

T_SerialDebugPage::~T_SerialDebugPage() {
    saveSendHistory();  // 保存发送历史
}

void T_SerialDebugPage::setupUI() {
    auto *mainLayout = new QVBoxLayout(this);

    // 串口设置区域
    auto *settingsGroup = new QGroupBox("串口设置");
    auto *settingsLayout = new QHBoxLayout(settingsGroup);

    serialPortComboBox = new ElaComboBox();
    refreshSerialButton = new ElaPushButton("刷新串口");
    settingsLayout->addWidget(serialPortComboBox);
    settingsLayout->addWidget(refreshSerialButton);

    baudRateComboBox = new ElaComboBox();
    baudRateComboBox->addItem("9600", QSerialPort::Baud9600);
    baudRateComboBox->addItem("115200", QSerialPort::Baud115200);
    settingsLayout->addWidget(baudRateComboBox);

    dataBitsComboBox = new ElaComboBox();
    dataBitsComboBox->addItem("8", QSerialPort::Data8);
    settingsLayout->addWidget(dataBitsComboBox);

    parityComboBox = new ElaComboBox();
    parityComboBox->addItem("None", QSerialPort::NoParity);
    settingsLayout->addWidget(parityComboBox);

    mainLayout->addWidget(settingsGroup);

    // 打开/关闭串口
    openSerialButton = new ElaPushButton("打开串口");
    mainLayout->addWidget(openSerialButton);
    connect(openSerialButton, &ElaPushButton::clicked, this,
            &T_SerialDebugPage::on_openSerialButton_clicked);

    // 数据接收区
    receiveTextEdit = new ElaPlainTextEdit();
    receiveTextEdit->setReadOnly(true);
    mainLayout->addWidget(receiveTextEdit);

    // 发送区
    auto *sendLayout = new QHBoxLayout();
    sendLineEdit = new ElaLineEdit();
    sendButton = new ElaPushButton("发送");
    newlineCheckBox = new ElaCheckBox("换行发送");
    sendLayout->addWidget(sendLineEdit);
    sendLayout->addWidget(sendButton);
    sendLayout->addWidget(newlineCheckBox);
    mainLayout->addLayout(sendLayout);

    connect(sendButton, &ElaPushButton::clicked, this,
            &T_SerialDebugPage::on_sendButton_clicked);

    // 时间戳选择和重连选项
    autoReconnectCheckBox = new ElaCheckBox("自动重连");
    mainLayout->addWidget(autoReconnectCheckBox);

    // 状态显示
    auto *statusLayout = new QHBoxLayout();
    rxLabel = new ElaText("RX: 0");
    txLabel = new ElaText("TX: 0");
    statusLayout->addWidget(rxLabel);
    statusLayout->addWidget(txLabel);
    mainLayout->addLayout(statusLayout);

    // 清屏和保存日志按钮
    auto *actionLayout = new QHBoxLayout();
    clearScreenButton = new ElaPushButton("清屏");
    saveLogButton = new ElaPushButton("保存日志");
    actionLayout->addWidget(clearScreenButton);
    actionLayout->addWidget(saveLogButton);
    mainLayout->addLayout(actionLayout);

    connect(clearScreenButton, &ElaPushButton::clicked, this,
            &T_SerialDebugPage::on_clearScreenButton_clicked);
    connect(saveLogButton, &ElaPushButton::clicked, this,
            &T_SerialDebugPage::on_saveLogButton_clicked);

    setLayout(mainLayout);

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("AACore调试终端");
    auto *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

void T_SerialDebugPage::updateSerialPorts() {
    serialPortComboBox->clear();
    const auto AVAILABLE_PORTS = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : AVAILABLE_PORTS) {
        serialPortComboBox->addItem(info.portName());
    }
}

void T_SerialDebugPage::on_openSerialButton_clicked() {
    if (serial->isOpen()) {
        serial->close();
        openSerialButton->setText("打开串口");
    } else {
        serial->setPortName(serialPortComboBox->currentText());
        serial->setBaudRate(static_cast<QSerialPort::BaudRate>(
            baudRateComboBox->currentData().toInt()));
        if (serial->open(QIODevice::ReadWrite)) {
            openSerialButton->setText("关闭串口");
        } else {
            QMessageBox::critical(this, "错误", "无法打开串口");
        }
    }
}

void T_SerialDebugPage::on_sendButton_clicked() {
    if (serial->isOpen()) {
        QString data = sendLineEdit->text();
        if (newlineCheckBox->isChecked()) {
            data += "\r\n";
        }
        serial->write(data.toUtf8());
        sentBytes += data.size();
        txLabel->setText(QString("TX: %1").arg(sentBytes));
    }
}

void T_SerialDebugPage::readSerialData() {
    QByteArray data = serial->readAll();
    receivedBytes += data.size();
    rxLabel->setText(QString("RX: %1").arg(receivedBytes));
    receiveTextEdit->appendPlainText(QString::fromUtf8(data));
}

void T_SerialDebugPage::on_clearScreenButton_clicked() {
    receiveTextEdit->clear();
}

void T_SerialDebugPage::on_saveLogButton_clicked() {
    QString fileName = QFileDialog::getSaveFileName(
        this, "保存日志", "", "文本文件 (*.txt);;CSV文件 (*.csv)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << receiveTextEdit->toPlainText();
        file.close();
    } else {
        QMessageBox::critical(this, "错误", "无法保存文件");
    }
}

void T_SerialDebugPage::handleReconnect() {
    if (!serial->isOpen()) {
        on_openSerialButton_clicked();  // 尝试重新打开串口
    }
}

void T_SerialDebugPage::loadSendHistory() {
    QFile historyFile("send_history.txt");
    if (historyFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream inputStream(&historyFile);
        while (!inputStream.atEnd()) {
            sendHistory.append(inputStream.readLine());
        }
        historyFile.close();
    }
}

void T_SerialDebugPage::saveSendHistory() {
    QFile historyFile("send_history.txt");
    if (historyFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream outputStream(&historyFile);
        for (const QString &entry : std::as_const(sendHistory)) {
            outputStream << entry << "\n";
        }
        historyFile.close();
    }
}

void T_SerialDebugPage::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Up) {
        if (currentHistoryIndex > 0) {
            currentHistoryIndex--;
            sendLineEdit->setText(sendHistory.at(currentHistoryIndex));
        }
    } else if (event->key() == Qt::Key_Down) {
        if (currentHistoryIndex < sendHistory.size() - 1) {
            currentHistoryIndex++;
            sendLineEdit->setText(sendHistory.at(currentHistoryIndex));
        } else {
            sendLineEdit->clear();
        }
    }
}