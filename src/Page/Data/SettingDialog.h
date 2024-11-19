#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QColorDialog>
#include <QDateTimeEdit>
#include <QLabel>
#include <QLoggingCategory>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>


class ElaSpinBox;
class ElaPushButton;

Q_DECLARE_LOGGING_CATEGORY(settingsLog)

class SettingsWidget : public QWidget {
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget* parent = nullptr);

private slots:
    void onColorButtonClicked();
    void onApplyButtonClicked();

private:
    QDateTimeEdit* m_startTimeEdit;
    QDateTimeEdit* m_endTimeEdit;
    QSpinBox* m_lowerThresholdSpin;
    QSpinBox* m_upperThresholdSpin;
    QPushButton* m_colorButton;
    QPushButton* m_applyButton;

    void initializeUI();
    void connectSignals();
};

#endif  // SETTINGSDIALOG_H