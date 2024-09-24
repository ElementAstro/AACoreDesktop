#include "T_Guider_Setting.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QVBoxLayout>

#include "ElaCheckBox.h"
#include "ElaLineEdit.h"
#include "ElaMessageButton.h"
#include "ElaScrollPageArea.h"
#include "ElaSpinBox.h"
#include "ElaText.h"
#include "ElaWidget.h"

namespace {
constexpr int kMaxPortNumber = 65535;
constexpr int kDefaultPortNumber = 4400;
constexpr int kMaxInstanceNumber = 10;
constexpr int kMaxDitherPixels = 100;
constexpr int kDefaultDitherPixels = 5;
constexpr double kMinSettlePixelTolerance = 0.1;
constexpr double kMaxSettlePixelTolerance = 10.0;
constexpr double kDefaultSettlePixelTolerance = 1.5;
constexpr int kMaxSettleTime = 100;
constexpr int kDefaultSettleTime = 10;
constexpr int kMaxSettleTimeout = 300;
constexpr int kDefaultSettleTimeout = 40;
constexpr int kMaxGuidingStartTimeout = 1000;
constexpr int kDefaultGuidingStartTimeout = 300;
constexpr int kMaxRoiPercentage = 100;
constexpr int kDefaultRoiPercentage = 100;
constexpr int kDescriptionTextPixelSize = 15;
constexpr int kTopLayoutSpacing = 10;
constexpr int kTopLayoutMarginTop = 25;
}  // namespace

T_PHD2SetupDialog::T_PHD2SetupDialog(QWidget *parent)
    : ElaWidget(parent),
      phd2PathEdit(nullptr),
      serverUrlEdit(nullptr),
      serverPortSpin(nullptr),
      instanceNumberSpin(nullptr),
      ditherPixelsSpin(nullptr),
      ditherRaOnlyCheck(nullptr),
      settlePixelToleranceSpin(nullptr),
      minSettleTimeSpin(nullptr),
      settleTimeoutSpin(nullptr),
      guidingStartRetryCheck(nullptr),
      guidingStartTimeoutSpin(nullptr),
      roiPercentageSpin(nullptr),
      okButton(nullptr),
      cancelButton(nullptr) {
    setupUi();
    setWindowTitle("PHD2 Setup");
    this->setIsFixedSize(true);
    setWindowButtonFlags(ElaAppBarType::CloseButtonHint);
}

void T_PHD2SetupDialog::setupUi() {
    auto *mainLayout = new QVBoxLayout(this);

    // Add a top layout with a description
    auto *topLayout = createTopLayout("Configure PHD2 settings for guiding");
    mainLayout->addLayout(topLayout);

    auto *scrollArea = new ElaScrollPageArea(this);
    auto *formLayout = new QFormLayout(scrollArea);

    // Replace QLineEdit with ElaLineEdit
    phd2PathEdit = new ElaLineEdit(this);
    phd2PathEdit->setText("C:/Program Files (x86)/PHDGuiding2/phd2.exe");
    formLayout->addRow(new ElaText("PHD2 path:", this), phd2PathEdit);

    serverUrlEdit = new ElaLineEdit(this);
    serverUrlEdit->setText("localhost");
    formLayout->addRow(new ElaText("PHD2 server URL:", this), serverUrlEdit);

    // Replace QSpinBox with ElaSpinBox
    serverPortSpin = new ElaSpinBox(this);
    serverPortSpin->setRange(1, kMaxPortNumber);
    serverPortSpin->setValue(kDefaultPortNumber);
    formLayout->addRow(new ElaText("PHD2 server port:", this), serverPortSpin);

    instanceNumberSpin = new ElaSpinBox(this);
    instanceNumberSpin->setRange(1, kMaxInstanceNumber);
    instanceNumberSpin->setValue(1);
    formLayout->addRow(new ElaText("PHD2 instance number:", this),
                       instanceNumberSpin);

    ditherPixelsSpin = new ElaSpinBox(this);
    ditherPixelsSpin->setRange(0, kMaxDitherPixels);
    ditherPixelsSpin->setValue(kDefaultDitherPixels);
    ditherPixelsSpin->setSuffix(" px");
    formLayout->addRow(new ElaText("Dither pixels:", this), ditherPixelsSpin);

    // Replace QCheckBox with ElaCheckBox
    ditherRaOnlyCheck = new ElaCheckBox("Dither in RA only", this);
    formLayout->addRow("", ditherRaOnlyCheck);

    settlePixelToleranceSpin = new ElaSpinBox(this);
    settlePixelToleranceSpin->setRange(kMinSettlePixelTolerance,
                                       kMaxSettlePixelTolerance);
    settlePixelToleranceSpin->setValue(kDefaultSettlePixelTolerance);
    settlePixelToleranceSpin->setSuffix(" px");
    formLayout->addRow(new ElaText("Settle pixel tolerance:", this),
                       settlePixelToleranceSpin);

    minSettleTimeSpin = new ElaSpinBox(this);
    minSettleTimeSpin->setRange(1, kMaxSettleTime);
    minSettleTimeSpin->setValue(kDefaultSettleTime);
    minSettleTimeSpin->setSuffix(" s");
    formLayout->addRow(new ElaText("Minimum settle time:", this),
                       minSettleTimeSpin);

    settleTimeoutSpin = new ElaSpinBox(this);
    settleTimeoutSpin->setRange(1, kMaxSettleTimeout);
    settleTimeoutSpin->setValue(kDefaultSettleTimeout);
    settleTimeoutSpin->setSuffix(" s");
    formLayout->addRow(new ElaText("Settle timeout:", this), settleTimeoutSpin);

    guidingStartRetryCheck = new ElaCheckBox("Guiding start retry", this);
    formLayout->addRow("", guidingStartRetryCheck);

    guidingStartTimeoutSpin = new ElaSpinBox(this);
    guidingStartTimeoutSpin->setRange(1, kMaxGuidingStartTimeout);
    guidingStartTimeoutSpin->setValue(kDefaultGuidingStartTimeout);
    guidingStartTimeoutSpin->setSuffix(" s");
    formLayout->addRow(new ElaText("Guiding start timeout:", this),
                       guidingStartTimeoutSpin);

    roiPercentageSpin = new ElaSpinBox(this);
    roiPercentageSpin->setRange(1, kMaxRoiPercentage);
    roiPercentageSpin->setValue(kDefaultRoiPercentage);
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
    auto *buttonBox = new QDialogButtonBox(this);
    buttonBox->addButton(okButton, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);
    mainLayout->addWidget(buttonBox);

    connect(okButton, &ElaMessageButton::clicked, this,
            &T_PHD2SetupDialog::accept);
    connect(cancelButton, &ElaMessageButton::clicked, this,
            &T_PHD2SetupDialog::reject);
}

auto T_PHD2SetupDialog::createTopLayout(const QString &description)
    -> QVBoxLayout * {
    auto *topLayout = new QVBoxLayout();
    auto *descriptionText = new ElaText(description, this);
    descriptionText->setTextPixelSize(kDescriptionTextPixelSize);
    topLayout->addWidget(descriptionText);
    topLayout->addSpacing(kTopLayoutSpacing);
    topLayout->setContentsMargins(0, kTopLayoutMarginTop, 0, 0);
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