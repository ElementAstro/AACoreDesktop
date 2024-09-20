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

class T_Navigation;
class T_Popup;
class T_Card;
class T_View;
class MainWindow : public ElaWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    Q_SLOT void onCloseButtonClicked();

    void initWindow();
    void initEdgeLayout();
    void initContent();

private:
    T_Home* _homePage{nullptr};
    T_Icon* _iconPage{nullptr};
    T_DeviceConnection* _deviceConnectionPage{nullptr};
    T_CameraPage *_cameraPage{nullptr};
    T_TelescopePage *_telescopePage{nullptr};
    T_FocuserPage *_focuserPage{nullptr};
    T_FilterWheelPage *_filterWheelPage{nullptr};
    T_GuiderPage *_guiderPage{nullptr};
    T_SimpleSequencerPage *_simpleSequencerPage{nullptr};
    T_TargetSearchPage *_targetSearchPage{nullptr};
    T_ConfigPanel *_configPanel{nullptr};
    QString _cameraKey{""};
    QString _elaDxgiKey{""};
    QString _aboutKey{""};
    QString _settingKey{""};
};
#endif // MAINWINDOW_H