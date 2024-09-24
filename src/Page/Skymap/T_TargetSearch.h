#ifndef T_TargetSearchPage_H
#define T_TargetSearchPage_H

#include "T_BasePage.h"

#include <QDate>
#include <QWidget>
#include "ElaComboBox.h"
#include "ElaPlainTextEdit.h"
#include "ElaProgressBar.h"
#include "ElaPushButton.h"
#include "ElaSlider.h"
#include "ElaSpinBox.h"
#include "ElaTabWidget.h"
#include "ElaToggleSwitch.h"

class QVBoxLayout;
class QHBoxLayout;
class ElaText;
class QChart;
class QChartView;
class QListWidget;

class T_TargetSearchPage : public T_BasePage {
    Q_OBJECT

public:
    explicit T_TargetSearchPage(QWidget *parent = nullptr);
    ~T_TargetSearchPage();

private slots:
    void onSearchClicked();
    void onObjectSelected(int index);

private:
    void setupUI();
    void createLeftPanel();
    void createRightPanel();
    void createObjectList();
    void createObjectDetails();
    void createVisibilityChart();
    void createMoonPhaseInfo();
    void updateObjectDetails(int index);
    void updateMoonPhaseInfo();
    void styleLeftPanel();
    void styleObjectDetails();

    QVBoxLayout *mainLayout;
    QHBoxLayout *contentLayout;

    // Left panel widgets
    QWidget *leftPanel;
    ElaComboBox *observationCombo;
    ElaPlainTextEdit *dateEdit;
    ElaComboBox *altitudeCombo;
    ElaComboBox *apparentSizeCombo;
    ElaComboBox *fromCombo;
    ElaComboBox *throughCombo;
    ElaComboBox *moonCombo;
    ElaSpinBox *minDistanceSpinBox;
    ElaComboBox *objectTypeCombo;
    ElaComboBox *constellationCombo;
    ElaComboBox *coordinatesCombo;
    ElaComboBox *surfaceBrightnessCombo;
    ElaComboBox *apparentMagnitudeCombo;
    ElaComboBox *orderByCombo;
    ElaComboBox *descendingCombo;
    ElaSpinBox *itemsPerPageSpinBox;
    ElaPushButton *searchButton;

    // Right panel widgets
    QWidget *rightPanel;
    ElaTabWidget *objectTabWidget;

    // Object list widgets
    QListWidget *objectListWidget;

    // Object details widgets
    QWidget *objectDetailsWidget;
    ElaText *objectNameLabel;
    ElaText *raLabel;
    ElaText *decLabel;
    ElaText *typeLabel;
    ElaText *constellationLabel;
    ElaSlider *altitudeSlider;
    ElaProgressBar *illuminationProgress;
    QChartView *visibilityChartView;
    QWidget *moonPhaseWidget;

    // Moon phase widgets
    ElaText *moonPhaseLabel;
    ElaText *moonriseLabel;
    ElaText *moonsetLabel;
    ElaText *illuminationLabel;

    // Utility functions
    QChart *createVisibilityQChart();
    void createMoonPhaseWidget();
};

#endif  // T_TargetSearchPage_H