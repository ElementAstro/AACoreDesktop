#include "SettingDialog.h"

#include <QDebug>
#include <QFormLayout>
#include <QGroupBox>
#include <QVBoxLayout>

#include "ElaPushButton.h"
#include "ElaColorDialog.h"
#include "ElaSpinBox.h"
#include "ElaText.h"

Q_LOGGING_CATEGORY(settingsLog, "SettingsWidget")

SettingsWidget::SettingsWidget(QWidget* parent)
    : QWidget(parent),
      m_lowerThresholdSpin(new ElaSpinBox(this)),
      m_upperThresholdSpin(new ElaSpinBox(this)),
      m_colorButton(new ElaPushButton("Set Exceed Color", this)),
      m_applyButton(new ElaPushButton("Apply", this)),
      m_startDateButton(new ElaPushButton("Set Start Date", this)),
      m_endDateButton(new ElaPushButton("Set End Date", this)) {
    qCDebug(settingsLog) << "Initializing SettingsWidget";
    initializeUI();
    connectSignals();
}

void SettingsWidget::initializeUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 创建时间设置组
    QGroupBox* timeGroupBox = new QGroupBox("Time Settings");
    QFormLayout* timeLayout = new QFormLayout();
    timeLayout->addRow(new ElaText("Start Date:"), m_startDateButton);
    timeLayout->addRow(new ElaText("End Date:"), m_endDateButton);
    timeGroupBox->setLayout(timeLayout);

    // 创建阈值设置组
    QGroupBox* thresholdGroupBox = new QGroupBox("Threshold Settings");
    QFormLayout* thresholdLayout = new QFormLayout();
    m_lowerThresholdSpin->setRange(-50, 50);
    m_lowerThresholdSpin->setValue(0);
    m_upperThresholdSpin->setRange(-50, 50);
    m_upperThresholdSpin->setValue(50);
    thresholdLayout->addRow(new ElaText("Lower Threshold:"), m_lowerThresholdSpin);
    thresholdLayout->addRow(new ElaText("Upper Threshold:"), m_upperThresholdSpin);
    thresholdGroupBox->setLayout(thresholdLayout);

    // 添加组到主布局
    mainLayout->addWidget(timeGroupBox);
    mainLayout->addWidget(thresholdGroupBox);
    mainLayout->addWidget(m_colorButton);
    mainLayout->addWidget(m_applyButton);

    setLayout(mainLayout);
}

void SettingsWidget::connectSignals() {
    connect(m_colorButton, &ElaPushButton::clicked, this, &SettingsWidget::onColorButtonClicked);
    connect(m_applyButton, &ElaPushButton::clicked, this, &SettingsWidget::onApplyButtonClicked);
    connect(m_startDateButton, &ElaPushButton::clicked, this, &SettingsWidget::onStartDateButtonClicked);
    connect(m_endDateButton, &ElaPushButton::clicked, this, &SettingsWidget::onEndDateButtonClicked);
}

void SettingsWidget::onColorButtonClicked() {
    qCDebug(settingsLog) << "Color button clicked";
    auto colorDialog = new ElaColorDialog(this);
    connect(colorDialog, &ElaColorDialog::colorSelected, this, [](const QColor& color) {
        qCDebug(settingsLog) << "Selected color:" << color;
    });
    colorDialog->exec();
}

void SettingsWidget::onApplyButtonClicked() {
    qCDebug(settingsLog) << "Apply button clicked";
    try {
        // 获取并验证输入值
        int lowerThreshold = m_lowerThresholdSpin->value();
        int upperThreshold = m_upperThresholdSpin->value();

        if (m_startDate >= m_endDate) {
            throw std::invalid_argument("Start date must be before end date");
        }

        if (lowerThreshold >= upperThreshold) {
            throw std::invalid_argument("Lower threshold must be less than upper threshold");
        }

        qCInfo(settingsLog)
            << "Settings applied successfully:"
            << "Start Date:" << m_startDate << "End Date:" << m_endDate
            << "Lower Threshold:" << lowerThreshold
            << "Upper Threshold:" << upperThreshold;
    } catch (const std::exception& e) {
        qCCritical(settingsLog) << "Failed to apply settings:" << e.what();
    }
}

void SettingsWidget::onStartDateButtonClicked() {
    DateDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        m_startDate = dialog.selectedDate();
        qCDebug(settingsLog) << "Start date selected:" << m_startDate;
    }
}

void SettingsWidget::onEndDateButtonClicked() {
    DateDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        m_endDate = dialog.selectedDate();
        qCDebug(settingsLog) << "End date selected:" << m_endDate;
    }
}

DateDialog::DateDialog(QWidget* parent)
    : QDialog(parent), m_calendar(new ElaCalendar(this)) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_calendar);

    QPushButton* okButton = new QPushButton("OK", this);
    connect(okButton, &QPushButton::clicked, this, &DateDialog::onDateSelected);
    layout->addWidget(okButton);

    setLayout(layout);
}

QDate DateDialog::selectedDate() const {
    return m_selectedDate;
}

void DateDialog::onDateSelected() {
    m_selectedDate = m_calendar->getSelectedDate();
    accept();
}

void SettingsWidget::onStartDateSelected(const QDate& date) {
    m_startDate = date;
    qCDebug(settingsLog) << "Start date selected:" << m_startDate;
}

void SettingsWidget::onEndDateSelected(const QDate& date) {
    m_endDate = date;
    qCDebug(settingsLog) << "End date selected:" << m_endDate;
}