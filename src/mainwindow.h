#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ElaWindow.h"

class T_Home;
class T_Icon;
class T_DeviceConnection;
class T_CameraPage;
class T_TelescopePage;
class T_FocuserPage;
class T_FilterWheelPage;
class T_GuiderPage;
class T_ConfigPanel;

class T_SimpleSequencerPage;

class T_TargetSearchPage;

class T_SerialConfig;
class T_SerialDebugPage;

class T_SoftwarePage;
class T_ProcessPage;
class T_SystemInfoPage;

class T_LogPanelPage;

class T_Navigation;
class T_Popup;
class T_Card;
class T_View;
class MainWindow : public ElaWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Q_SLOT void onCloseButtonClicked();

    void initWindow();
    void initEdgeLayout();
    void initContent();

private:
    T_Home *_homePage{nullptr};
    T_Icon *_iconPage{nullptr};
    T_DeviceConnection *_deviceConnectionPage{nullptr};
    T_CameraPage *_cameraPage{nullptr};
    T_TelescopePage *_telescopePage{nullptr};
    T_FocuserPage *_focuserPage{nullptr};
    T_FilterWheelPage *_filterWheelPage{nullptr};
    T_GuiderPage *_guiderPage{nullptr};
    T_SimpleSequencerPage *_simpleSequencerPage{nullptr};
    T_TargetSearchPage *_targetSearchPage{nullptr};
    T_ConfigPanel *_configPanel{nullptr};
    T_SerialConfig *_serialConfigPage{nullptr};
    T_SerialDebugPage *_serialDebugPage{nullptr};
    T_SoftwarePage *_softwarePage{nullptr};
    T_ProcessPage *_processPage{nullptr};
    T_SystemInfoPage *_systemInfoPage{nullptr};
    T_LogPanelPage *_logPanelPage{nullptr};
    QString _cameraKey{""};
    QString _elaDxgiKey{""};
    QString _aboutKey{""};
    QString _settingKey{""};
};
#endif  // MAINWINDOW_H
