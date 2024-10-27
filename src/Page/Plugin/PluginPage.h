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

    void setInstalled(bool installed);
    bool isInstalled() const;

private:
    ElaText *nameLabel;
    ElaText *authorLabel;
    ElaText *versionLabel;
    ElaText *descriptionLabel;
    ElaText *statusLabel;
    ElaText *dependenciesLabel;
    ElaPushButton *installButton;
    ElaPushButton *uninstallButton;
    ElaPushButton *configButton;
    ElaProgressBar *installProgress;
    QString pluginName;
    bool installed;

private slots:
    void onInstallButtonClicked();
    void onUninstallButtonClicked();
    void onConfigButtonClicked();

signals:
    void installRequested(const QString &pluginName);
    void uninstallRequested(const QString &pluginName);
};

#endif  // PLUGINPAGE_H
