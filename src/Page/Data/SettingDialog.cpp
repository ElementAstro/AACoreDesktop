#include "SettingDialog.h"
#include <QDebug>

Q_LOGGING_CATEGORY(settingsLog, "SettingsWidget")

SettingsWidget::SettingsWidget(QWidget* parent)
    : QWidget(parent),
      m_startTimeEdit(new QDateTimeEdit(this)),
      m_endTimeEdit(new QDateTimeEdit(this)),
      m_lowerThresholdSpin(new QSpinBox(this)),
      m_upperThresholdSpin(new QSpinBox(this)),
      m_colorButton(new QPushButton("Set Exceed Color", this)),
      m_applyButton(new QPushButton("Apply", this)) {
    qCDebug(settingsLog) << "Initializing SettingsWidget";
    initializeUI();
    connectSignals();
}

void SettingsWidget::initializeUI() {
    QVBoxLayout* layout = new QVBoxLayout(this);

    m_startTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    layout->addWidget(new QLabel("Start Time:"));
    layout->addWidget(m_startTimeEdit);

    m_endTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    layout->addWidget(new QLabel("End Time:"));
    layout->addWidget(m_endTimeEdit);

    m_lowerThresholdSpin->setRange(-50, 50);
    m_lowerThresholdSpin->setValue(0);
    layout->addWidget(new QLabel("Lower Threshold:"));
    layout->addWidget(m_lowerThresholdSpin);

    m_upperThresholdSpin->setRange(-50, 50);
    m_upperThresholdSpin->setValue(50);
    layout->addWidget(new QLabel("Upper Threshold:"));
    layout->addWidget(m_upperThresholdSpin);

    layout->addWidget(m_colorButton);
    layout->addWidget(m_applyButton);
}

void SettingsWidget::connectSignals() {
    connect(m_colorButton, &QPushButton::clicked, this, &SettingsWidget::onColorButtonClicked);
    connect(m_applyButton, &QPushButton::clicked, this, &SettingsWidget::onApplyButtonClicked);
}

void SettingsWidget::onColorButtonClicked() {
    qCDebug(settingsLog) << "Color button clicked";
    QColor color = QColorDialog::getColor(Qt::red, this, "Choose Color");
    if (color.isValid()) {
        qCDebug(settingsLog) << "Selected color:" << color;
    } else {
        qCWarning(settingsLog) << "No color selected";
    }
}

void SettingsWidget::onApplyButtonClicked() {
    qCDebug(settingsLog) << "Apply button clicked";
    try {
        // 获取并验证输入值
        QDateTime startTime = m_startTimeEdit->dateTime();
        QDateTime endTime = m_endTimeEdit->dateTime();
        int lowerThreshold = m_lowerThresholdSpin->value();
        int upperThreshold = m_upperThresholdSpin->value();

        if (startTime >= endTime) {
            throw std::invalid_argument("Start time must be before end time");
        }

        if (lowerThreshold >= upperThreshold) {
            throw std::invalid_argument("Lower threshold must be less than upper threshold");
        }

        qCInfo(settingsLog) << "Settings applied successfully:"
                            << "Start Time:" << startTime
                            << "End Time:" << endTime
                            << "Lower Threshold:" << lowerThreshold
                            << "Upper Threshold:" << upperThreshold;
    } catch (const std::exception& e) {
        qCCritical(settingsLog) << "Failed to apply settings:" << e.what();
    }
}