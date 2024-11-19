#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QColorDialog>
#include <QLabel>
#include <QLoggingCategory>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>
#include <QDate>
#include <QDialog>

#include "ElaCalendar.h"

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
    void onStartDateButtonClicked();
    void onEndDateButtonClicked();
    void onStartDateSelected(const QDate& date);
    void onEndDateSelected(const QDate& date);

private:
    QDate m_startDate;
    QDate m_endDate;
    QSpinBox* m_lowerThresholdSpin;
    QSpinBox* m_upperThresholdSpin;
    QPushButton* m_colorButton;
    QPushButton* m_applyButton;
    QPushButton* m_startDateButton;
    QPushButton* m_endDateButton;

    void initializeUI();
    void connectSignals();
};

class DateDialog : public QDialog {
    Q_OBJECT

public:
    explicit DateDialog(QWidget* parent = nullptr);

    QDate selectedDate() const;

private slots:
    void onDateSelected();

private:
    ElaCalendar* m_calendar;
    QDate m_selectedDate;
};

#endif  // SETTINGSDIALOG_H