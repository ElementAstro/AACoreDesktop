#include "T_About.h"

#include <QDesktopServices>
#include <QHBoxLayout>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>

#include "ElaImageCard.h"
#include "ElaProgressBar.h"
#include "ElaPushButton.h"
#include "ElaText.h"

namespace {
constexpr int kMargin20 = 20;
constexpr int kMargin25 = 25;
constexpr int kFixedSize80 = 80;
constexpr int kTextPixelSize24 = 24;
constexpr int kTextPixelSize16 = 16;
constexpr int kSpacing10 = 10;
constexpr int kFixedHeight10 = 10;
constexpr int kRange100 = 100;
constexpr int kTextPixelSize12 = 12;
constexpr int kTextPixelSize14 = 14;
constexpr int kTimerInterval50 = 50;
}  // namespace

T_About::T_About(QWidget* parent) : ElaWidget(parent), m_progressValue(0) {
    setWindowTitle("关于 AACoreDesktop");
    setWindowIcon(QIcon(":/include/Image/Moon.jpg"));
    setIsFixedSize(true);
    setWindowButtonFlags(ElaAppBarType::CloseButtonHint);

    // Main layout
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(kMargin20, kMargin25, kMargin20, kMargin20);

    // Header layout
    auto* headerLayout = new QHBoxLayout();

    // Logo
    auto* logoCard = new ElaImageCard(this);
    logoCard->setFixedSize(kFixedSize80, kFixedSize80);
    logoCard->setIsPreserveAspectCrop(true);
    logoCard->setCardImage(QImage(":/include/Image/Moon.jpg"));
    headerLayout->addWidget(logoCard);

    // Title and version
    auto* titleLayout = new QVBoxLayout();
    auto* titleText = new ElaText("ElaWidgetTools", this);
    titleText->setTextPixelSize(kTextPixelSize24);
    titleText->setFont(QFont("Arial", kTextPixelSize24, QFont::Bold));

    auto* versionText = new ElaText("版本1.0.0", this);
    versionText->setTextPixelSize(kTextPixelSize16);

    titleLayout->addWidget(titleText);
    titleLayout->addWidget(versionText);
    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();

    mainLayout->addLayout(headerLayout);

    // Separator
    auto* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line);

    // Information layout
    auto* infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(kSpacing10);

    addInfoText(infoLayout, "授权协议:", "AGPL-3");
    addInfoText(infoLayout, "作者:", "astro_air@126.com");

    mainLayout->addLayout(infoLayout);

    // Progress bar
    m_progressBar = new ElaProgressBar(this);
    m_progressBar->setFixedHeight(kFixedHeight10);
    m_progressBar->setRange(0, kRange100);
    m_progressBar->setValue(0);
    mainLayout->addWidget(m_progressBar);

    // Buttons layout
    auto* buttonLayout = new QHBoxLayout();

    auto* websiteButton = new ElaPushButton("访问网站", this);
    connect(websiteButton, &ElaPushButton::clicked, this,
            &T_About::onWebsiteClicked);

    auto* updateButton = new ElaPushButton("检查更新", this);
    connect(updateButton, &ElaPushButton::clicked, this,
            &T_About::onUpdateClicked);

    auto* helpButton = new ElaPushButton("帮助", this);
    connect(helpButton, &ElaPushButton::clicked, this, &T_About::onHelpClicked);

    auto* feedbackButton = new ElaPushButton("反馈", this);
    connect(feedbackButton, &ElaPushButton::clicked, this,
            &T_About::onFeedbackClicked);

    buttonLayout->addWidget(websiteButton);
    buttonLayout->addWidget(updateButton);
    buttonLayout->addWidget(helpButton);
    buttonLayout->addWidget(feedbackButton);

    mainLayout->addLayout(buttonLayout);

    // Copyright
    auto* copyrightText = new ElaText("版权所有 © 2024-present Max Qian", this);
    copyrightText->setTextPixelSize(kTextPixelSize12);
    copyrightText->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(copyrightText);

    // Set up timer for progress bar animation
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &T_About::updateProgressBar);
}

T_About::~T_About() = default;

void T_About::addInfoText(QVBoxLayout* layout, const QString& label,
                          const QString& value) {
    auto* rowLayout = new QHBoxLayout();

    auto* labelText = new ElaText(label, this);
    labelText->setTextPixelSize(kTextPixelSize14);
    labelText->setFont(QFont("Arial", kTextPixelSize14, QFont::Bold));

    auto* valueText = new ElaText(value, this);
    valueText->setTextPixelSize(kTextPixelSize14);
    valueText->setTextInteractionFlags(Qt::TextSelectableByMouse);

    rowLayout->addWidget(labelText);
    rowLayout->addWidget(valueText, 1);

    layout->addLayout(rowLayout);
}

void T_About::onWebsiteClicked() {
    QDesktopServices::openUrl(QUrl("https://github.com/ElementAstro/Lithium"));
}

void T_About::onUpdateClicked() {
    m_progressValue = 0;
    m_progressBar->setValue(0);
    m_timer->start(kTimerInterval50);
}

void T_About::onHelpClicked() {
    QDesktopServices::openUrl(
        QUrl("https://github.com/ElementAstro/Lithium/wiki"));
}

void T_About::onFeedbackClicked() {
    QDesktopServices::openUrl(
        QUrl("https://github.com/ElementAstro/Lithium/issues"));
}

void T_About::updateProgressBar() {
    m_progressValue += 2;
    m_progressBar->setValue(m_progressValue);

    if (m_progressValue >= kRange100) {
        m_timer->stop();
        // Here you would typically check for updates and show the result
        // For demonstration, we'll just show a message
        auto* updateText = new ElaText("已是最新版本", this);
        updateText->setTextPixelSize(kTextPixelSize14);
        updateText->setAlignment(Qt::AlignCenter);
        qobject_cast<QVBoxLayout*>(layout())->addWidget(updateText);
    }
}