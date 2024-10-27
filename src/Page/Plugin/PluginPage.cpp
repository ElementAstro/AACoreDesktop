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
    // Initialize components
    nameLabel = new ElaText("<h2>" + pluginName + "</h2>");
    authorLabel = new ElaText("Author: " + author);
    versionLabel = new ElaText("Version: " + version);
    descriptionLabel = new ElaText(description);
    statusLabel = new ElaText("Status: Not Installed");
    dependenciesLabel = new ElaText("Dependencies: " + dependencies.join(", "));

    installButton = new ElaPushButton("Install");
    uninstallButton = new ElaPushButton("Uninstall");
    configButton = new ElaPushButton("Configure");

    installProgress = new ElaProgressBar;
    installProgress->setRange(0, 100);
    installProgress->setValue(0);
    installProgress->setVisible(false);

    uninstallButton->setEnabled(false);
    configButton->setEnabled(false);

    // Layout and UI structure
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Adding labels
    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(authorLabel);
    mainLayout->addWidget(versionLabel);
    mainLayout->addWidget(descriptionLabel);
    mainLayout->addWidget(statusLabel);
    mainLayout->addWidget(dependenciesLabel);

    // Divider line
    QFrame* line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line);

    // Button layout
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(installButton);
    buttonLayout->addWidget(uninstallButton);
    buttonLayout->addWidget(configButton);

    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(installProgress);
    setLayout(mainLayout);

    // Connect signals to slots
    connect(installButton, &QPushButton::clicked, this,
            &PluginPage::onInstallButtonClicked);
    connect(uninstallButton, &QPushButton::clicked, this,
            &PluginPage::onUninstallButtonClicked);
    connect(configButton, &QPushButton::clicked, this,
            &PluginPage::onConfigButtonClicked);
}

void PluginPage::setInstalled(bool installed) {
    this->installed = installed;
    if (installed) {
        statusLabel->setText("Status: Installed");
        installButton->setEnabled(false);
        uninstallButton->setEnabled(true);
        configButton->setEnabled(true);
    } else {
        statusLabel->setText("Status: Not Installed");
        installButton->setEnabled(true);
        uninstallButton->setEnabled(false);
        configButton->setEnabled(false);
    }
}

bool PluginPage::isInstalled() const { return installed; }

void PluginPage::onInstallButtonClicked() {
    installProgress->setVisible(true);
    installProgress->setValue(0);
    installButton->setEnabled(false);

    auto* timer = new QTimer(this);
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
    timer->start(200);
}

void PluginPage::onUninstallButtonClicked() {
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
    PluginConfigDialog configDialog(pluginName, this);
    configDialog.exec();
}