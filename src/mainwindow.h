#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTranslator>

#include "ElaWindow.h"

class ElaContentDialog;

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

class T_ImageViewerPage;

class T_SoftwarePage;
class T_ProcessPage;
class T_SystemInfoPage;
class T_TerminalPage;

class T_LogPanelPage;

class PluginManagerPage;

class HelpWindow;

class T_Setting;
class T_I18NPage;

class T_WebSocketClientPage;
class T_HttpClientPage;
class T_TcpClientPage;

class MainWindow : public ElaWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Q_SLOT void onCloseButtonClicked();

    void initWindow();
    void initEdgeLayout();
    void initContent();

private slots:
    void changeLanguage(const QString &languageCode);

private:
    ElaContentDialog *_closeDialog{nullptr};

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
    T_Setting *_settingPage{nullptr};
    T_TerminalPage *_terminalPage{nullptr};
    T_ImageViewerPage *_imageViewerPage{nullptr};
    T_WebSocketClientPage *_webSocketClientPage{nullptr};
    T_HttpClientPage *_httpClientPage{nullptr};
    T_TcpClientPage *_tcpClientPage{nullptr};

    PluginManagerPage *_pluginManagerPage{nullptr};

    HelpWindow *_helpWindow{nullptr};

    QTranslator translator;
    T_I18NPage *i18nManager;  // I18nManager 组件实例

    QString _cameraKey{""};
    QString _elaDxgiKey{""};
    QString _aboutKey{""};
    QString _settingKey{""};
    QString _i18nKey{""};
};
#endif  // MAINWINDOW_H
