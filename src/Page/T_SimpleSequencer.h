// T_SimpleSequencerPage.h
#ifndef TARGETSETWIDGET_H
#define TARGETSETWIDGET_H

#include <qstandarditemmodel.h>
#include <QBarSeries>
#include <QBarSet>
#include <QChart>
#include <QChartView>
#include <QDateTimeEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QValueAxis>
#include <QWidget>

#include "T_BasePage.h"

class ElaLineEdit;
class ElaTableView;
class ElaComboBox;
class ElaPushButton;
class ElaToggleSwitch;
class ElaSpinBox;

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

private:
    void setupUI();
    void createTopSection();
    void createMiddleSection();
    void createBottomSection();
    void createChart();
    void createControlButtons();
    void applyStyles();

    QVBoxLayout *mainLayout;
    QWidget *topWidget;
    QWidget *middleWidget;
    QWidget *bottomWidget;

    QStandardItemModel *model;

    // Top section widgets
    ElaToggleSwitch *coolCameraSwitch;
    ElaToggleSwitch *unparkMountSwitch;
    ElaToggleSwitch *meridianFlipSwitch;
    ElaToggleSwitch *warmCameraSwitch;
    ElaToggleSwitch *parkMountSwitch;

    // Middle section widgets
    ElaSpinBox *delayStartSpinBox;
    ElaComboBox *sequenceModeCombo;
    ElaLineEdit *estimatedDownloadTimeEdit;
    QDateTimeEdit *estimatedFinishTimeEdit;
    QDateTimeEdit *estFinishTimeThisTargetEdit;

    // Bottom section widgets
    ElaTableView *targetTable;
    QChartView *chartView;

    // Control buttons
    ElaPushButton *backButton;
    ElaPushButton *addButton;
    ElaPushButton *deleteButton;
    ElaPushButton *resetButton;
    ElaPushButton *moveUpButton;
    ElaPushButton *moveDownButton;
    ElaPushButton *startButton;
};

#endif  // TARGETSETWIDGET_H