#include "T_Guider_Setting.h"
#include "ElaCheckBox.h"
#include "ElaLineEdit.h"
#include "ElaMessageButton.h"
#include "ElaScrollPageArea.h"
#include "ElaSpinBox.h"
#include "ElaText.h"
#include "ElaWidget.h"
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QVBoxLayout>

T_PHD2SetupDialog::T_PHD2SetupDialog(QWidget *parent) : ElaWidget(parent) {
  setupUi();
  setWindowTitle("PHD2 Setup");
  this->setIsFixedSize(true);
  setWindowButtonFlags(ElaAppBarType::CloseButtonHint);
}

void T_PHD2SetupDialog::setupUi() {
  auto mainLayout = new QVBoxLayout(this);

  // Add a top layout with a description
  auto topLayout = createTopLayout("Configure PHD2 settings for guiding");
  mainLayout->addLayout(topLayout);

  auto scrollArea = new ElaScrollPageArea(this);
  auto formLayout = new QFormLayout(scrollArea);

  // Replace QLineEdit with ElaLineEdit
  phd2PathEdit = new ElaLineEdit(this);
  phd2PathEdit->setText("C:/Program Files (x86)/PHDGuiding2/phd2.exe");
  formLayout->addRow(new ElaText("PHD2 path:", this), phd2PathEdit);

  serverUrlEdit = new ElaLineEdit(this);
  serverUrlEdit->setText("localhost");
  formLayout->addRow(new ElaText("PHD2 server URL:", this), serverUrlEdit);

  // Replace QSpinBox with ElaSpinBox
  serverPortSpin = new ElaSpinBox(this);
  serverPortSpin->setRange(1, 65535);
  serverPortSpin->setValue(4400);
  formLayout->addRow(new ElaText("PHD2 server port:", this), serverPortSpin);

  instanceNumberSpin = new ElaSpinBox(this);
  instanceNumberSpin->setRange(1, 10);
  instanceNumberSpin->setValue(1);
  formLayout->addRow(new ElaText("PHD2 instance number:", this),
                     instanceNumberSpin);

  ditherPixelsSpin = new ElaSpinBox(this);
  ditherPixelsSpin->setRange(0, 100);
  ditherPixelsSpin->setValue(5);
  ditherPixelsSpin->setSuffix(" px");
  formLayout->addRow(new ElaText("Dither pixels:", this), ditherPixelsSpin);

  // Replace QCheckBox with ElaCheckBox
  ditherRaOnlyCheck = new ElaCheckBox("Dither in RA only", this);
  formLayout->addRow("", ditherRaOnlyCheck);

  settlePixelToleranceSpin = new ElaSpinBox(this);
  settlePixelToleranceSpin->setRange(0.1, 10.0);
  settlePixelToleranceSpin->setValue(1.5);
  settlePixelToleranceSpin->setSuffix(" px");
  formLayout->addRow(new ElaText("Settle pixel tolerance:", this),
                     settlePixelToleranceSpin);

  minSettleTimeSpin = new ElaSpinBox(this);
  minSettleTimeSpin->setRange(1, 100);
  minSettleTimeSpin->setValue(10);
  minSettleTimeSpin->setSuffix(" s");
  formLayout->addRow(new ElaText("Minimum settle time:", this),
                     minSettleTimeSpin);

  settleTimeoutSpin = new ElaSpinBox(this);
  settleTimeoutSpin->setRange(1, 300);
  settleTimeoutSpin->setValue(40);
  settleTimeoutSpin->setSuffix(" s");
  formLayout->addRow(new ElaText("Settle timeout:", this), settleTimeoutSpin);

  guidingStartRetryCheck = new ElaCheckBox("Guiding start retry", this);
  formLayout->addRow("", guidingStartRetryCheck);

  guidingStartTimeoutSpin = new ElaSpinBox(this);
  guidingStartTimeoutSpin->setRange(1, 1000);
  guidingStartTimeoutSpin->setValue(300);
  guidingStartTimeoutSpin->setSuffix(" s");
  formLayout->addRow(new ElaText("Guiding start timeout:", this),
                     guidingStartTimeoutSpin);

  roiPercentageSpin = new ElaSpinBox(this);
  roiPercentageSpin->setRange(1, 100);
  roiPercentageSpin->setValue(100);
  roiPercentageSpin->setSuffix(" %");
  formLayout->addRow(new ElaText("ROI percentage to find guide star:", this),
                     roiPercentageSpin);

  mainLayout->addWidget(scrollArea);

  // Replace QPushButton with ElaMessageButton
  okButton = new ElaMessageButton("OK", this);
  okButton->setDefault(true);
  cancelButton = new ElaMessageButton("Cancel", this);
  okButton->setAutoDefault(true);
  cancelButton->setAutoDefault(true);
  auto buttonBox = new QDialogButtonBox(this);
  buttonBox->addButton(okButton, QDialogButtonBox::AcceptRole);
  buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);
  mainLayout->addWidget(buttonBox);

  connect(okButton, &ElaMessageButton::clicked, this,
          &T_PHD2SetupDialog::accept);
  connect(cancelButton, &ElaMessageButton::clicked, this,
          &T_PHD2SetupDialog::reject);
}

QVBoxLayout *T_PHD2SetupDialog::createTopLayout(const QString &description) {
  QVBoxLayout *topLayout = new QVBoxLayout();
  ElaText *descriptionText = new ElaText(description, this);
  descriptionText->setTextPixelSize(15);
  topLayout->addWidget(descriptionText);
  topLayout->addSpacing(10);
  topLayout->setContentsMargins(0, 25, 0, 0);
  return topLayout;
}

void T_PHD2SetupDialog::accept() {
  // saveSettings();
  this->hide();
}

void T_PHD2SetupDialog::reject() {
  // Optionally, you can add any cleanup or reset logic here
  this->hide();
}