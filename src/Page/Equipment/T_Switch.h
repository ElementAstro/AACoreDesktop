#ifndef T_SWITCHPAGE_H
#define T_SWITCHPAGE_H

#include <QTimer>
#include <QWidget>
#include <QHBoxLayout>

#include "Components/C_InfoCard.h"
#include "T_SwitchConfig.h"
#include "T_BasePage.h"

class ElaPushButton;
class ElaSlider;
class ElaText;
class ElaTabWidget;
class C_INDIPanel;
class ElaIconButton;
class ElaToggleSwitch;
class QGroupBox;

class T_SwitchPage : public T_BasePage {
    Q_OBJECT

public:
    explicit T_SwitchPage(QWidget *parent = nullptr);
    ~T_SwitchPage();

signals:
    void power1Changed(bool state);
    void power2Changed(bool state);
    void lightBoxValueChanged(int value);
    void flatPanelValueChanged(int value);

private slots:
    void onSettingsButtonClicked();
    void onPowerButtonClicked();
    void onRefreshButtonClicked();
    void updateStatus();
    void onLightboxSliderChanged(int value);
    void onFlatPanelSliderChanged(int value);

private:
    void createLayout();
    QHBoxLayout* createTopLayout();
    QWidget* createInfoTab();
    QWidget* createControlTab();
    QWidget* createSettingsTab();
    QWidget* createChartTab();
    QGroupBox* createInfoGroup(const QString &title);
    void addSliderControl(QVBoxLayout *layout, const QString &label, 
                         ElaSlider *&slider, int min, int max, int default_value);

    ElaTabWidget *tabWidget;
    
    // Power controls
    ElaPushButton *power1Button;
    ElaPushButton *power2Button;
    
    // Sliders
    ElaSlider *lightboxSlider;
    ElaSlider *flatPanelSlider;

    // Top toolbar buttons
    ElaPushButton *settingsButton;
    ElaIconButton *_powerButton;
    ElaIconButton *_refreshButton;

    // Toggle switches
    ElaToggleSwitch *lightboxSwitch;
    ElaToggleSwitch *flatPanelSwitch;

    // Info cards
    InfoCard *deviceCard;
    InfoCard *statusCard;
    InfoCard *lightboxCard;
    InfoCard *flatPanelCard;
    InfoCard *power1Card;
    InfoCard *power2Card;

    // Timers
    QTimer *updateTimer;

    // Config dialog
    T_SwitchConfig *serialConfigDialog;

    // Status tracking
    bool isPowered;
    bool isLightboxOn;
    bool isFlatPanelOn;
};

#endif  // T_SWITCHPAGE_H