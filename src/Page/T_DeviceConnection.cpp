#include "T_DeviceConnection.h"

#include <QApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLineEdit>
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

T_DeviceConnection::T_DeviceConnection(QWidget *parent) : T_BasePage(parent) {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(20);
  mainLayout->setContentsMargins(20, 20, 20, 20);

  // Profile section
  QGroupBox *profileGroup = new QGroupBox("Profile", this);
  QVBoxLayout *profileLayout = new QVBoxLayout(profileGroup);

  // Name and checkboxes
  QHBoxLayout *nameLayout = new QHBoxLayout();
  ElaText *nameLabel = new ElaText("Name:", this);
  nameLabel->setTextPixelSize(15);
  ElaLineEdit *nameEdit = new ElaLineEdit(this);
  nameEdit->setText("测试");
  nameEdit->setMinimumWidth(200);
  nameLayout->addWidget(nameLabel);
  nameLayout->addWidget(nameEdit);
  nameLayout->addStretch();

  QHBoxLayout *checkboxLayout = new QHBoxLayout();
  ElaCheckBox *autoConnectCheck = new ElaCheckBox("Auto Connect", this);
  ElaCheckBox *portSelectorCheck = new ElaCheckBox("Port Selector", this);
  ElaCheckBox *siteInfoCheck = new ElaCheckBox("Site Info", this);
  checkboxLayout->addWidget(autoConnectCheck);
  checkboxLayout->addWidget(portSelectorCheck);
  checkboxLayout->addWidget(siteInfoCheck);
  checkboxLayout->addStretch();

  profileLayout->addLayout(nameLayout);
  profileLayout->addLayout(checkboxLayout);

  // Mode and connection details
  QHBoxLayout *modeLayout = new QHBoxLayout();
  ElaText *modeLabel = new ElaText("Mode:", this);
  modeLabel->setTextPixelSize(15);
  ElaRadioButton *localRadio = new ElaRadioButton("Local", this);
  ElaRadioButton *remoteRadio = new ElaRadioButton("Remote", this);
  modeLayout->addWidget(modeLabel);
  modeLayout->addWidget(localRadio);
  modeLayout->addWidget(remoteRadio);
  modeLayout->addStretch();

  QHBoxLayout *connectionLayout = new QHBoxLayout();
  ElaText *hostLabel = new ElaText("Host:", this);
  hostLabel->setTextPixelSize(15);
  ElaLineEdit *hostEdit = new ElaLineEdit(this);
  hostEdit->setText("127.0.0.1");
  ElaText *portLabel = new ElaText("Port:", this);
  portLabel->setTextPixelSize(15);
  ElaSpinBox *portSpinBox = new ElaSpinBox(this);
  portSpinBox->setValue(7624);
  connectionLayout->addWidget(hostLabel);
  connectionLayout->addWidget(hostEdit);
  connectionLayout->addWidget(portLabel);
  connectionLayout->addWidget(portSpinBox);
  connectionLayout->addStretch();

  profileLayout->addLayout(modeLayout);
  profileLayout->addLayout(connectionLayout);

  // Guiding and additional options
  QHBoxLayout *guidingLayout = new QHBoxLayout();
  ElaText *guidingLabel = new ElaText("Guiding:", this);
  guidingLabel->setTextPixelSize(15);
  ElaComboBox *guidingCombo = new ElaComboBox(this);
  guidingCombo->addItem("Internal");
  guidingCombo->addItem("PHD2");
  guidingCombo->addItem("Sodium");
  guidingCombo->addItem("None");
  guidingCombo->setMinimumWidth(200);
  guidingLayout->addWidget(guidingLabel);
  guidingLayout->addWidget(guidingCombo);
  guidingLayout->addStretch();

  QHBoxLayout *optionsLayout = new QHBoxLayout();
  ElaCheckBox *indiWebManagerCheck = new ElaCheckBox("INDI Web Manager", this);
  ElaLineEdit *indiWebManagerHost = new ElaLineEdit(this);
  indiWebManagerHost->setText("127.0.0.1");
  indiWebManagerHost->setMinimumWidth(50);
  ElaSpinBox *indiWebManagerPort = new ElaSpinBox(this);
  indiWebManagerPort->setRange(1000, 88524);
  indiWebManagerPort->setValue(8624);

  ElaPushButton *webManagerButton = new ElaPushButton("Web Manager", this);
  ElaPushButton *scanButton = new ElaPushButton("Scan", this);
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
  QGroupBox *devicesGroup = new QGroupBox("Select Devices", this);
  QGridLayout *devicesLayout = new QGridLayout(devicesGroup);
  devicesLayout->setColumnStretch(1, 1);
  devicesLayout->setColumnStretch(3, 1);
  devicesLayout->setColumnStretch(5, 1);

  QStringList deviceLabels = {
      "Mount:", "Camera 1:", "Camera 2:", "Focuser:", "Filter:", "AO:",
      "Dome:",  "Weather:",  "Aux 1:",    "Aux 2:",   "Aux 3:",  "Aux 4:"};
  int row = 0, col = 0;
  for (const QString &label : deviceLabels) {
    ElaText *deviceLabel = new ElaText(label, this);
    deviceLabel->setTextPixelSize(15);
    ElaComboBox *deviceCombo = new ElaComboBox(this);
    deviceCombo->addItem("--");
    if (label == "Camera 1:")
      deviceCombo->addItem("CCD Simulator");
    if (label == "Focuser:")
      deviceCombo->addItem("Focuser Simulator");
    if (label == "Filter:")
      deviceCombo->addItem("Filter Simulator");
    devicesLayout->addWidget(deviceLabel, row, col * 2);
    devicesLayout->addWidget(deviceCombo, row, col * 2 + 1);
    col++;
    if (col > 2) {
      col = 0;
      row++;
    }
  }

  QHBoxLayout *remoteLayout = new QHBoxLayout();
  ElaText *remoteLabel = new ElaText("Remote:", this);
  remoteLabel->setTextPixelSize(15);
  ElaLineEdit *remoteEdit = new ElaLineEdit(this);
  remoteEdit->setText(
      "driver@host:port, driver@host, @host:port, @host, driver");
  ElaPushButton *scriptsButton = new ElaPushButton("Scripts", this);
  remoteLayout->addWidget(remoteLabel);
  remoteLayout->addWidget(remoteEdit, 1);
  remoteLayout->addWidget(scriptsButton);

  devicesLayout->addLayout(remoteLayout, 4, 0, 1, 6);

  mainLayout->addWidget(devicesGroup);

  // Bottom buttons
  QHBoxLayout *bottomLayout = new QHBoxLayout();
  ElaPushButton *saveButton = new ElaPushButton("Save", this);
  ElaPushButton *resetButton = new ElaPushButton("Reset", this);
  bottomLayout->addStretch();
  bottomLayout->addWidget(saveButton);
  bottomLayout->addWidget(resetButton);

  mainLayout->addLayout(bottomLayout);

  QWidget *centralWidget = new QWidget(this);
  centralWidget->setWindowTitle("设备连接");
  QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
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

T_DeviceConnection::~T_DeviceConnection() {}

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