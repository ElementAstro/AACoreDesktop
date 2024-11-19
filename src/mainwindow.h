#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTranslator>

#include "ElaWindow.h"

class ElaContentDialog;

class T_Home;

class T_SwitchPage;

class T_SerialConfig;
class T_SerialDebugPage;

class T_ImageViewerPage;

class T_LogPanelPage;

class T_DataHistory;

class T_Setting;
class T_I18NPage;

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
    T_SwitchPage *_switchPage{nullptr};
    T_SerialConfig *_serialConfigPage{nullptr};
    T_SerialDebugPage *_serialDebugPage{nullptr};
    T_LogPanelPage *_logPanelPage{nullptr};
    T_Setting *_settingPage{nullptr};
    T_DataHistory *_dataHistoryPage{nullptr};

    QTranslator translator;
    T_I18NPage *i18nManager;  // I18nManager 组件实例

    QString _cameraKey{""};
    QString _elaDxgiKey{""};
    QString _aboutKey{""};
    QString _settingKey{""};
    QString _i18nKey{""};
};
#endif  // MAINWINDOW_H
