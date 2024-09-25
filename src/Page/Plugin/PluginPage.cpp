#include "PluginPage.h"
#include "PluginConfigDialog.h"

#include <QMessageBox>
#include <QTimer>

#include "ElaProgressBar.h"
#include "ElaPushButton.h"
#include "ElaText.h"


PluginPage::PluginPage(const QString& pluginName, const QString& author,
                       const QString& version, const QString& description,
                       const QStringList& dependencies, QWidget* parent)
    : QWidget(parent), pluginName(pluginName), installed(false) {
    // 初始化组件
    nameLabel = new ElaText("<h2>" + pluginName + "</h2>");
    authorLabel = new ElaText("Author: " + author);
    versionLabel = new ElaText("Version: " + version);
    descriptionLabel = new ElaText(description);
    statusLabel = new ElaText("Status: Not Installed");
    dependenciesLabel = new ElaText("Dependencies: " + dependencies.join(", "));
    installButton = new ElaPushButton("Install");
    uninstallButton = new ElaPushButton("Uninstall");
    uninstallButton->setEnabled(false);  // 默认未安装时不可卸载
    configButton = new ElaPushButton("Configure");  // 配置按钮
    configButton->setEnabled(false);  // 默认未安装时不可配置
    installProgress = new ElaProgressBar;
    installProgress->setRange(0, 100);
    installProgress->setValue(0);
    installProgress->setVisible(false);  // 默认隐藏进度条

    // 垂直布局管理器
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(nameLabel);
    layout->addWidget(authorLabel);
    layout->addWidget(versionLabel);
    layout->addWidget(descriptionLabel);
    layout->addWidget(statusLabel);
    layout->addWidget(dependenciesLabel);
    layout->addWidget(installButton);
    layout->addWidget(uninstallButton);
    layout->addWidget(configButton);
    layout->addWidget(installProgress);

    // 设置页面布局
    setLayout(layout);

    // 连接安装按钮的点击信号到槽函数
    connect(installButton, &ElaPushButton::clicked, this,
            &PluginPage::onInstallButtonClicked);
    connect(uninstallButton, &ElaPushButton::clicked, this,
            &PluginPage::onUninstallButtonClicked);
    connect(configButton, &ElaPushButton::clicked, this,
            &PluginPage::onConfigButtonClicked);  // 配置按钮连接槽函数
}

void PluginPage::setInstalled(bool installed) {
    this->installed = installed;
    if (installed) {
        statusLabel->setText("Status: Installed");
        installButton->setEnabled(false);
        uninstallButton->setEnabled(true);
        configButton->setEnabled(true);  // 已安装时允许配置
    } else {
        statusLabel->setText("Status: Not Installed");
        installButton->setEnabled(true);
        uninstallButton->setEnabled(false);
        configButton->setEnabled(false);  // 未安装时禁用配置
    }
}

bool PluginPage::isInstalled() const { return installed; }

void PluginPage::onInstallButtonClicked() {
    // 模拟安装过程，显示进度条
    installProgress->setVisible(true);
    installProgress->setValue(0);
    installButton->setEnabled(false);

    // 使用定时器模拟安装进度
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=]() {
        int progress = installProgress->value();
        if (progress < 100) {
            installProgress->setValue(progress + 10);
        } else {
            timer->stop();
            installProgress->setVisible(false);
            setInstalled(true);
            emit installRequested(pluginName);
            QMessageBox::information(this, "Install Complete",
                                     pluginName + " installed successfully.");
        }
    });
    timer->start(200);  // 每200ms增加10%的进度
}

void PluginPage::onUninstallButtonClicked() {
    // 模拟卸载过程
    int result = QMessageBox::question(
        this, "Uninstall Plugin",
        "Are you sure you want to uninstall " + pluginName + "?");
    if (result == QMessageBox::Yes) {
        setInstalled(false);
        QMessageBox::information(this, "Uninstall Complete",
                                 pluginName + " uninstalled successfully.");
        emit uninstallRequested(pluginName);
    }
}

void PluginPage::onConfigButtonClicked() {
    // 弹出配置对话框
    PluginConfigDialog configDialog(pluginName, this);
    configDialog.exec();  // 显示对话框并等待用户关闭
}
