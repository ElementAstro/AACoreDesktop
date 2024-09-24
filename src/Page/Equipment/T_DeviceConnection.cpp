#include "T_DeviceConnection.h"

#include <QApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QStyleFactory>
#include <QVBoxLayout>

#include "ElaCheckBox.h"
#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "ElaRadioButton.h"
#include "ElaSpinBox.h"
#include "ElaText.h"

namespace {
constexpr int kSpacing20 = 20;
constexpr int kMargin20 = 20;
constexpr int kTextPixelSize15 = 15;
constexpr int kMinimumWidth200 = 200;
constexpr int kMinimumWidth50 = 50;
constexpr int kPort7624 = 7624;
constexpr int kPortRangeMin = 1000;
constexpr int kPortRangeMax = 88524;
constexpr int kPort8624 = 8624;
constexpr int kColumnStretch1 = 1;
constexpr int kColumnStretch5 = 5;
constexpr int kRow4 = 4;
constexpr int kColumnSpan6 = 6;
}  // namespace

T_DeviceConnection::T_DeviceConnection(QWidget *parent) : T_BasePage(parent) {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(kSpacing20);
    mainLayout->setContentsMargins(kMargin20, kMargin20, kMargin20, kMargin20);

    // Profile section
    auto *profileGroup = new QGroupBox("Profile", this);
    auto *profileLayout = new QVBoxLayout(profileGroup);

    // Name and checkboxes
    auto *nameLayout = new QHBoxLayout();
    auto *nameLabel = new ElaText("Name:", this);
    nameLabel->setTextPixelSize(kTextPixelSize15);
    auto *nameEdit = new ElaLineEdit(this);
    nameEdit->setText("测试");
    nameEdit->setMinimumWidth(kMinimumWidth200);
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);
    nameLayout->addStretch();

    auto *checkboxLayout = new QHBoxLayout();
    auto *autoConnectCheck = new ElaCheckBox("Auto Connect", this);
    auto *portSelectorCheck = new ElaCheckBox("Port Selector", this);
    auto *siteInfoCheck = new ElaCheckBox("Site Info", this);
    checkboxLayout->addWidget(autoConnectCheck);
    checkboxLayout->addWidget(portSelectorCheck);
    checkboxLayout->addWidget(siteInfoCheck);
    checkboxLayout->addStretch();

    profileLayout->addLayout(nameLayout);
    profileLayout->addLayout(checkboxLayout);

    // Mode and connection details
    auto *modeLayout = new QHBoxLayout();
    auto *modeLabel = new ElaText("Mode:", this);
    modeLabel->setTextPixelSize(kTextPixelSize15);
    auto *localRadio = new ElaRadioButton("Local", this);
    auto *remoteRadio = new ElaRadioButton("Remote", this);
    modeLayout->addWidget(modeLabel);
    modeLayout->addWidget(localRadio);
    modeLayout->addWidget(remoteRadio);
    modeLayout->addStretch();

    auto *connectionLayout = new QHBoxLayout();
    auto *hostLabel = new ElaText("Host:", this);
    hostLabel->setTextPixelSize(kTextPixelSize15);
    auto *hostEdit = new ElaLineEdit(this);
    hostEdit->setText("127.0.0.1");
    auto *portLabel = new ElaText("Port:", this);
    portLabel->setTextPixelSize(kTextPixelSize15);
    auto *portSpinBox = new ElaSpinBox(this);
    portSpinBox->setValue(kPort7624);
    connectionLayout->addWidget(hostLabel);
    connectionLayout->addWidget(hostEdit);
    connectionLayout->addWidget(portLabel);
    connectionLayout->addWidget(portSpinBox);
    connectionLayout->addStretch();

    profileLayout->addLayout(modeLayout);
    profileLayout->addLayout(connectionLayout);

    // Guiding and additional options
    auto *guidingLayout = new QHBoxLayout();
    auto *guidingLabel = new ElaText("Guiding:", this);
    guidingLabel->setTextPixelSize(kTextPixelSize15);
    auto *guidingCombo = new ElaComboBox(this);
    guidingCombo->addItem("Internal");
    guidingCombo->addItem("PHD2");
    guidingCombo->addItem("Sodium");
    guidingCombo->addItem("None");
    guidingCombo->setMinimumWidth(kMinimumWidth200);
    guidingLayout->addWidget(guidingLabel);
    guidingLayout->addWidget(guidingCombo);
    guidingLayout->addStretch();

    auto *optionsLayout = new QHBoxLayout();
    auto *indiWebManagerCheck = new ElaCheckBox("INDI Web Manager", this);
    auto *indiWebManagerHost = new ElaLineEdit(this);
    indiWebManagerHost->setText("127.0.0.1");
    indiWebManagerHost->setMinimumWidth(kMinimumWidth50);
    auto *indiWebManagerPort = new ElaSpinBox(this);
    indiWebManagerPort->setRange(kPortRangeMin, kPortRangeMax);
    indiWebManagerPort->setValue(kPort8624);

    auto *webManagerButton = new ElaPushButton("Web Manager", this);
    auto *scanButton = new ElaPushButton("Scan", this);
    optionsLayout->addWidget(indiWebManagerCheck);
    optionsLayout->addWidget(indiWebManagerHost);
    optionsLayout->addWidget(indiWebManagerPort);
    optionsLayout->addWidget(webManagerButton);
    optionsLayout->addWidget(scanButton);
    optionsLayout->addStretch();

    profileLayout->addLayout(guidingLayout);
    profileLayout->addLayout(optionsLayout);

    mainLayout->addWidget(profileGroup);

    // Select Devices section
    auto *devicesGroup = new QGroupBox("Select Devices", this);
    auto *devicesLayout = new QGridLayout(devicesGroup);
    devicesLayout->setColumnStretch(1, kColumnStretch1);
    devicesLayout->setColumnStretch(3, kColumnStretch1);
    devicesLayout->setColumnStretch(kColumnStretch5, kColumnStretch1);

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
        if (label == "Camera 1:") {
            deviceCombo->addItem("CCD Simulator");
        }
        if (label == "Focuser:") {
            deviceCombo->addItem("Focuser Simulator");
        }
        if (label == "Filter:") {
            deviceCombo->addItem("Filter Simulator");
        }
        devicesLayout->addWidget(deviceLabel, row, col * 2);
        devicesLayout->addWidget(deviceCombo, row, col * 2 + 1);
        col++;
        if (col > 2) {
            col = 0;
            row++;
        }
    }

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

    // Bottom buttons
    auto *bottomLayout = new QHBoxLayout();
    auto *saveButton = new ElaPushButton("Save", this);
    auto *resetButton = new ElaPushButton("Reset", this);
    bottomLayout->addStretch();
    bottomLayout->addWidget(saveButton);
    bottomLayout->addWidget(resetButton);

    mainLayout->addLayout(bottomLayout);

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("设备连接");
    auto *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);

    connect(saveButton, &QPushButton::clicked, this,
            &T_DeviceConnection::onSaveButtonClicked);
    connect(resetButton, &QPushButton::clicked, this,
            &T_DeviceConnection::onCloseButtonClicked);
    connect(webManagerButton, &QPushButton::clicked, this,
            &T_DeviceConnection::onWebManagerButtonClicked);
    connect(scanButton, &QPushButton::clicked, this,
            &T_DeviceConnection::onScanButtonClicked);
    connect(scriptsButton, &QPushButton::clicked, this,
            &T_DeviceConnection::onScriptsButtonClicked);
}

T_DeviceConnection::~T_DeviceConnection() = default;

void T_DeviceConnection::onSaveButtonClicked() {
    // Save button functionality
    // Implement saving functionality here
}

void T_DeviceConnection::onCloseButtonClicked() {
    // Close button functionality
    // Implement closing functionality here
    close();
}

void T_DeviceConnection::onWebManagerButtonClicked() {
    // Web Manager button functionality
    // Implement Web Manager functionality here
}

void T_DeviceConnection::onScanButtonClicked() {
    // Scan button functionality
    // Implement scan functionality here
}

void T_DeviceConnection::onScriptsButtonClicked() {
    // Scripts button functionality
    // Implement scripts functionality here
}