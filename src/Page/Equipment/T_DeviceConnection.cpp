#include "T_DeviceConnection.h"

#include <QApplication>
#include <QDesktopServices>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QRandomGenerator>
#include <QSpacerItem>
#include <QStyleFactory>
#include <QTimer>
#include <QVBoxLayout>

#include "ElaCheckBox.h"
#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "ElaRadioButton.h"
#include "ElaSpinBox.h"
#include "ElaText.h"

#include "Components/C_InfoCard.h"

namespace {
constexpr int kSpacing20 = 20;
constexpr int kMargin20 = 20;
constexpr int kTextPixelSize15 = 15;
constexpr int kMinimumWidth200 = 200;
constexpr int kMinimumWidth50 = 50;
constexpr int kPortDefault = 7624;
constexpr int kPortRangeMin = 1000;
constexpr int kPortRangeMax = 88524;
constexpr int kPortAlternative = 8624;
constexpr int kColumnStretch1 = 1;
constexpr int kColumnStretch5 = 5;
constexpr int kRow4 = 4;
constexpr int kColumnSpan6 = 6;
}  // namespace

T_DeviceConnection::T_DeviceConnection(QWidget *parent) : T_BasePage(parent) {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(kSpacing20);
    mainLayout->setContentsMargins(kMargin20, kMargin20, kMargin20, kMargin20);

    // 创建 Profile 部分
    createProfileSection(mainLayout);

    // 创建 Select Devices 部分
    createDevicesSection(mainLayout);

    // 创建底部按钮
    createBottomButtons(mainLayout);

    // Info Cards
    auto *infoCardsLayout = new QHBoxLayout();
    _connectionStatusCard = new InfoCard("连接状态", "未连接", this);
    _deviceStatusCard = new InfoCard("设备状态", "未检测", this);
    infoCardsLayout->addWidget(_connectionStatusCard);
    infoCardsLayout->addWidget(_deviceStatusCard);
    infoCardsLayout->addStretch();
    mainLayout->addLayout(infoCardsLayout);

    // 状态更新定时器
    _statusTimer = new QTimer(this);
    connect(_statusTimer, &QTimer::timeout, this,
            &T_DeviceConnection::updateConnectionStatus);
    _statusTimer->start(5000);  // 每5秒更新一次状态
}

T_DeviceConnection::~T_DeviceConnection() = default;

void T_DeviceConnection::createProfileSection(QVBoxLayout *mainLayout) {
    auto *profileGroup = new QGroupBox("Profile", this);
    profileGroup->setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            border: 1px solid #bbb;
            border-radius: 5px;
            margin-top: 10px;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 3px 0 3px;
        }
    )");
    auto *profileLayout = new QVBoxLayout(profileGroup);

    // Name 和 Checkboxes
    auto *nameLayout = new QHBoxLayout();
    auto *nameLabel = new ElaText("Name:", this);
    nameLabel->setTextPixelSize(kTextPixelSize15);
    _nameEdit = new ElaLineEdit(this);
    _nameEdit->setText("测试");
    _nameEdit->setMinimumWidth(kMinimumWidth200);
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(_nameEdit);
    nameLayout->addStretch();
    profileLayout->addLayout(nameLayout);

    auto *checkboxLayout = new QHBoxLayout();
    _autoConnectCheck = new ElaCheckBox("Auto Connect", this);
    _portSelectorCheck = new ElaCheckBox("Port Selector", this);
    _siteInfoCheck = new ElaCheckBox("Site Info", this);
    checkboxLayout->addWidget(_autoConnectCheck);
    checkboxLayout->addWidget(_portSelectorCheck);
    checkboxLayout->addWidget(_siteInfoCheck);
    checkboxLayout->addStretch();
    profileLayout->addLayout(checkboxLayout);

    // Mode 和 Connection Details
    auto *modeLayout = new QHBoxLayout();
    auto *modeLabel = new ElaText("Mode:", this);
    modeLabel->setTextPixelSize(kTextPixelSize15);
    _localRadio = new ElaRadioButton("Local", this);
    _remoteRadio = new ElaRadioButton("Remote", this);
    _localRadio->setChecked(true);
    modeLayout->addWidget(modeLabel);
    modeLayout->addWidget(_localRadio);
    modeLayout->addWidget(_remoteRadio);
    modeLayout->addStretch();
    profileLayout->addLayout(modeLayout);

    auto *connectionLayout = new QHBoxLayout();
    auto *hostLabel = new ElaText("Host:", this);
    hostLabel->setTextPixelSize(kTextPixelSize15);
    _hostEdit = new ElaLineEdit(this);
    _hostEdit->setText("127.0.0.1");
    _hostEdit->setMinimumWidth(kMinimumWidth200);

    auto *portLabel = new ElaText("Port:", this);
    portLabel->setTextPixelSize(kTextPixelSize15);
    _portSpinBox = new ElaSpinBox(this);
    _portSpinBox->setRange(kPortRangeMin, kPortRangeMax);
    _portSpinBox->setValue(kPortDefault);

    connectionLayout->addWidget(hostLabel);
    connectionLayout->addWidget(_hostEdit);
    connectionLayout->addWidget(portLabel);
    connectionLayout->addWidget(_portSpinBox);
    connectionLayout->addStretch();
    profileLayout->addLayout(connectionLayout);

    // Guiding 和 Additional Options
    auto *guidingLayout = new QHBoxLayout();
    auto *guidingLabel = new ElaText("Guiding:", this);
    guidingLabel->setTextPixelSize(kTextPixelSize15);
    _guidingCombo = new ElaComboBox(this);
    _guidingCombo->addItem("Internal");
    _guidingCombo->addItem("PHD2");
    _guidingCombo->addItem("Sodium");
    _guidingCombo->addItem("None");
    _guidingCombo->setMinimumWidth(kMinimumWidth200);
    guidingLayout->addWidget(guidingLabel);
    guidingLayout->addWidget(_guidingCombo);
    guidingLayout->addStretch();
    profileLayout->addLayout(guidingLayout);

    auto *optionsLayout = new QHBoxLayout();
    _indiWebManagerCheck = new ElaCheckBox("INDI Web Manager", this);
    _indiWebManagerHost = new ElaLineEdit(this);
    _indiWebManagerHost->setText("127.0.0.1");
    _indiWebManagerHost->setMinimumWidth(kMinimumWidth50);
    _indiWebManagerPort = new ElaSpinBox(this);
    _indiWebManagerPort->setRange(kPortRangeMin, kPortRangeMax);
    _indiWebManagerPort->setValue(kPortAlternative);

    _webManagerButton = new ElaPushButton("Web Manager", this);
    _scanButton = new ElaPushButton("Scan", this);
    _scriptsButton = new ElaPushButton("Scripts", this);

    optionsLayout->addWidget(_indiWebManagerCheck);
    optionsLayout->addWidget(_indiWebManagerHost);
    optionsLayout->addWidget(_indiWebManagerPort);
    optionsLayout->addWidget(_webManagerButton);
    optionsLayout->addWidget(_scanButton);
    optionsLayout->addWidget(_scriptsButton);
    optionsLayout->addStretch();
    profileLayout->addLayout(optionsLayout);

    mainLayout->addWidget(profileGroup);

    // 连接槽
    connect(_webManagerButton, &ElaPushButton::clicked, this,
            &T_DeviceConnection::onWebManagerButtonClicked);
    connect(_scanButton, &ElaPushButton::clicked, this,
            &T_DeviceConnection::onScanButtonClicked);
    connect(_scriptsButton, &ElaPushButton::clicked, this,
            &T_DeviceConnection::onScriptsButtonClicked);
}

void T_DeviceConnection::createDevicesSection(QVBoxLayout *mainLayout) {
    auto *devicesGroup = new QGroupBox("Select Devices", this);
    devicesGroup->setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            border: 1px solid #bbb;
            border-radius: 5px;
            margin-top: 10px;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 3px 0 3px;
        }
    )");
    auto *devicesLayout = new QGridLayout(devicesGroup);
    devicesLayout->setColumnStretch(1, kColumnStretch1);
    devicesLayout->setColumnStretch(3, kColumnStretch1);
    devicesLayout->setColumnStretch(5, kColumnStretch1);

    QStringList deviceLabels = {
        "Mount:", "Camera 1:", "Camera 2:", "Focuser:", "Filter:", "AO:",
        "Dome:",  "Weather:",  "Aux 1:",    "Aux 2:",   "Aux 3:",  "Aux 4:"};
    int row = 0;
    int col = 0;
    for (const QString &label : deviceLabels) {
        auto *deviceLabel = new ElaText(label, this);
        deviceLabel->setTextPixelSize(kTextPixelSize15);
        auto *deviceCombo = new ElaComboBox(this);
        deviceCombo->addItem("--");
        if (label == "Camera 1:" || label == "Camera 2:") {
            deviceCombo->addItem("CCD Simulator");
            deviceCombo->addItem("Camera Model X");
            deviceCombo->addItem("Camera Model Y");
        }
        if (label == "Focuser:") {
            deviceCombo->addItem("Focuser Simulator");
            deviceCombo->addItem("Focuser Model A");
            deviceCombo->addItem("Focuser Model B");
        }
        if (label == "Filter:") {
            deviceCombo->addItem("Filter Simulator");
            deviceCombo->addItem("Filter Wheel A");
            deviceCombo->addItem("Filter Wheel B");
        }
        if (label == "Mount:") {
            deviceCombo->addItem("Mount Simulator");
            deviceCombo->addItem("Mount Model 1");
            deviceCombo->addItem("Mount Model 2");
        }
        // 其他设备可以根据需要添加
        devicesLayout->addWidget(deviceLabel, row, col * 2);
        devicesLayout->addWidget(deviceCombo, row, col * 2 + 1);
        col++;
        if (col > 2) {
            col = 0;
            row++;
        }
    }

    // Remote Scripts
    auto *remoteLayout = new QHBoxLayout();
    auto *remoteLabel = new ElaText("Remote:", this);
    remoteLabel->setTextPixelSize(kTextPixelSize15);
    auto *remoteEdit = new ElaLineEdit(this);
    remoteEdit->setText(
        "driver@host:port, driver@host, @host:port, @host, driver");
    auto *scriptsButton = new ElaPushButton("Scripts", this);
    remoteLayout->addWidget(remoteLabel);
    remoteLayout->addWidget(remoteEdit, 1);
    remoteLayout->addWidget(scriptsButton);

    devicesLayout->addLayout(remoteLayout, kRow4, 0, 1, kColumnSpan6);

    mainLayout->addWidget(devicesGroup);
}

void T_DeviceConnection::createBottomButtons(QVBoxLayout *mainLayout) {
    auto *bottomLayout = new QHBoxLayout();
    _saveButton = new ElaPushButton("Save", this);
    _resetButton = new ElaPushButton("Reset", this);
    bottomLayout->addStretch();
    bottomLayout->addWidget(_saveButton);
    bottomLayout->addWidget(_resetButton);
    mainLayout->addLayout(bottomLayout);

    // 连接槽
    connect(_saveButton, &ElaPushButton::clicked, this,
            &T_DeviceConnection::onSaveButtonClicked);
    connect(_resetButton, &ElaPushButton::clicked, this,
            &T_DeviceConnection::onResetButtonClicked);
}

void T_DeviceConnection::updateConnectionStatus() {
    // 模拟连接状态更新
    // 实际应用中应从设备接口获取状态
    bool connected = QRandomGenerator::global()->bounded(0, 100) > 50;
    _connectionStatusCard->setValue(connected ? "已连接" : "未连接");
    _deviceStatusCard->setValue(connected ? "正常" : "异常");
}

void T_DeviceConnection::onSaveButtonClicked() {
    // 保存配置功能
    // 实现保存配置到文件或数据库
    // 示例: 显示保存成功信息
    _connectionStatusCard->setValue("配置已保存");
}

void T_DeviceConnection::onResetButtonClicked() {
    // 重置配置功能
    // 实现重置到默认配置
    _nameEdit->setText("测试");
    _autoConnectCheck->setChecked(false);
    _portSelectorCheck->setChecked(false);
    _siteInfoCheck->setChecked(false);
    _localRadio->setChecked(true);
    _hostEdit->setText("127.0.0.1");
    _portSpinBox->setValue(kPortDefault);
    _guidingCombo->setCurrentIndex(0);
    _indiWebManagerCheck->setChecked(false);
    _indiWebManagerHost->setText("127.0.0.1");
    _indiWebManagerPort->setValue(kPortAlternative);
    // 重置设备选择
    // 示例: 选择第一个选项
    // 具体实现根据实际设备列表
    updateConnectionStatus();
}

void T_DeviceConnection::onWebManagerButtonClicked() {
    // 打开 Web Manager 功能
    // 实现打开浏览器或内部 Web 界面
    QDesktopServices::openUrl(QUrl("http://localhost:7624"));
}

void T_DeviceConnection::onScanButtonClicked() {
    // 扫描设备功能
    // 实现自动扫描连接的设备
    // 示例: 显示扫描中
    _connectionStatusCard->setValue("正在扫描...");
    // 扫描完成后更新状态
    QTimer::singleShot(3000, this, [this]() { updateConnectionStatus(); });
}

void T_DeviceConnection::onScriptsButtonClicked() {
    // 打开 Scripts 功能
    // 实现打开脚本管理界面
    // 示例: 显示消息
    _deviceStatusCard->setValue("打开 Scripts 界面");
}

void T_DeviceConnection::onConnectionStatusUpdated(bool connected) {
    _connectionStatusCard->setValue(connected ? "已连接" : "未连接");
    _deviceStatusCard->setValue(connected ? "正常" : "异常");
}