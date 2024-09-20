// T_SimpleSequencerPage.h
#ifndef TARGETSETWIDGET_H
#define TARGETSETWIDGET_H

#include <QBarSeries>
#include <QBarSet>
#include <QChart>
#include <QChartView>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QValueAxis>
#include <QWidget>


#include "ElaComboBox.h"
#include "ElaPushButton.h"
#include "ElaSlider.h"
#include "ElaSpinBox.h"
#include "ElaText.h"
#include "ElaToggleSwitch.h"
#include "T_BasePage.h"


class T_SimpleSequencerPage : public T_BasePage {
  Q_OBJECT

public:
  explicit T_SimpleSequencerPage(QWidget *parent = nullptr);

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

  // Top section widgets
  ElaToggleSwitch *coolCameraSwitch;
  ElaToggleSwitch *unparkMountSwitch;
  ElaToggleSwitch *meridianFlipSwitch;
  ElaToggleSwitch *warmCameraSwitch;
  ElaToggleSwitch *parkMountSwitch;

  // Middle section widgets
  ElaSpinBox *delayStartSpinBox;
  ElaComboBox *sequenceModeCombo;
  QLineEdit *estimatedDownloadTimeEdit;
  QDateTimeEdit *estimatedFinishTimeEdit;
  QDateTimeEdit *estFinishTimeThisTargetEdit;

  // Bottom section widgets
  QTableWidget *targetTable;
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

#endif // TARGETSETWIDGET_H