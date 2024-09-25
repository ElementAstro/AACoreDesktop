#ifndef PLUGINPAGE_H
#define PLUGINPAGE_H

#include <QWidget>

class ElaText;
class ElaProgressBar;
class ElaPushButton;

class PluginPage : public QWidget {
    Q_OBJECT

public:
    explicit PluginPage(const QString &pluginName, const QString &author,
                        const QString &version, const QString &description,
                        const QStringList &dependencies,
                        QWidget *parent = nullptr);

    void setInstalled(bool installed);  // 设置插件的安装状态
    bool isInstalled() const;           // 获取插件的安装状态

private:
    ElaText *nameLabel;
    ElaText *authorLabel;
    ElaText *versionLabel;
    ElaText *descriptionLabel;
    ElaText *statusLabel;        // 插件状态显示（已安装/未安装）
    ElaText *dependenciesLabel;  // 插件依赖项显示
    ElaPushButton *installButton;
    ElaPushButton *uninstallButton;   // 新增卸载按钮
    ElaPushButton *configButton;      // 新增配置按钮
    ElaProgressBar *installProgress;  // 安装进度条
    QString pluginName;
    bool installed;

private slots:
    void onInstallButtonClicked();    // 安装按钮点击事件
    void onUninstallButtonClicked();  // 卸载按钮点击事件
    void onConfigButtonClicked();     // 配置按钮点击事件

signals:
    void installRequested(const QString &pluginName);    // 安装请求信号
    void uninstallRequested(const QString &pluginName);  // 卸载请求信号
};

#endif  // PLUGINPAGE_H
