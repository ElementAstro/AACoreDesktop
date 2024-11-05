// T_SimpleSequencerPage.h
#ifndef T_SIMPLERESEQUENCERPAGE_H
#define T_SIMPLERESEQUENCERPAGE_H

#include <QChartView>
#include <QDateTimeEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QWidget>

#include "T_BasePage.h"

class ElaLineEdit;
class ElaTableView;
class ElaComboBox;
class ElaPushButton;
class ElaToggleSwitch;
class ElaSpinBox;
class ElaText;

class T_SimpleSequencerPage : public T_BasePage {
    Q_OBJECT

public:
    explicit T_SimpleSequencerPage(QWidget *parent = nullptr);

private slots:
    void onBackButtonClicked();
    void onAddButtonClicked();
    void onDeleteButtonClicked();
    void onResetButtonClicked();
    void onMoveUpButtonClicked();
    void onMoveDownButtonClicked();
    void onStartButtonClicked();
    void onSequenceModeChanged(const QString &mode);
    void updateEstimatedTimes();

private:
    void setupUI();
    void createTopSection();
    void createMiddleSection();
    void createBottomSection();
    void createChart();
    void createControlButtons();
    void applyStyles();
    void loadSequenceFromFile(const QString &filePath);
    void saveSequenceToFile(const QString &filePath);
    void populateChart();

    QVBoxLayout *mainLayout;
    QWidget *topWidget;
    QWidget *middleWidget;
    QWidget *bottomWidget;

    QStandardItemModel *model;

    ElaToggleSwitch *coolCameraSwitch;
    ElaToggleSwitch *unparkMountSwitch;
    ElaToggleSwitch *meridianFlipSwitch;
    ElaToggleSwitch *warmCameraSwitch;
    ElaToggleSwitch *parkMountSwitch;

    ElaSpinBox *delayStartSpinBox;
    ElaComboBox *sequenceModeCombo;
    ElaLineEdit *estimatedDownloadTimeEdit;
    QDateTimeEdit *estimatedFinishTimeEdit;
    QDateTimeEdit *estFinishTimeThisTargetEdit;

    ElaTableView *targetTable;
    QChartView *chartView;

    ElaPushButton *backButton;
    ElaPushButton *addButton;
    ElaPushButton *deleteButton;
    ElaPushButton *resetButton;
    ElaPushButton *moveUpButton;
    ElaPushButton *moveDownButton;
    ElaPushButton *startButton;
};

#endif  // T_SIMPLERESEQUENCERPAGE_H