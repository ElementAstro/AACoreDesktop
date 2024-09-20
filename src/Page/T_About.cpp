#include "T_About.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>

#include "ElaImageCard.h"
#include "ElaText.h"
#include "ElaPushButton.h"
#include "ElaProgressBar.h"

T_About::T_About(QWidget* parent)
    : ElaWidget(parent), m_progressValue(0)
{
    setWindowTitle("关于 ElaWidgetTools");
    setWindowIcon(QIcon(":/include/Image/Moon.jpg"));
    setIsFixedSize(true);
    setWindowButtonFlags(ElaAppBarType::CloseButtonHint);
    
    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 25, 20, 20);

    // Header layout
    QHBoxLayout* headerLayout = new QHBoxLayout();
    
    // Logo
    ElaImageCard* logoCard = new ElaImageCard(this);
    logoCard->setFixedSize(80, 80);
    logoCard->setIsPreserveAspectCrop(true);
    logoCard->setCardImage(QImage(":/include/Image/Moon.jpg"));
    headerLayout->addWidget(logoCard);

    // Title and version
    QVBoxLayout* titleLayout = new QVBoxLayout();
    ElaText* titleText = new ElaText("ElaWidgetTools", this);
    titleText->setTextPixelSize(24);
    titleText->setFont(QFont("Arial", 24, QFont::Bold));
    
    ElaText* versionText = new ElaText("版本 LK-2024", this);
    versionText->setTextPixelSize(16);
    
    titleLayout->addWidget(titleText);
    titleLayout->addWidget(versionText);
    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();

    mainLayout->addLayout(headerLayout);

    // Separator
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line);

    // Information layout
    QVBoxLayout* infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(10);

    addInfoText(infoLayout, "授权协议:", "MIT");
    addInfoText(infoLayout, "Windows支持:", "Qt 5.12 及以上");
    addInfoText(infoLayout, "Linux支持:", "Qt 5.14 及以上");
    addInfoText(infoLayout, "作者:", "8009963@qq.com");
    addInfoText(infoLayout, "交流群:", "850243692 (QQ)");
    addInfoText(infoLayout, "其他信息:", "用户手册及API文档付费提供\n提供额外的专业技术支持");

    mainLayout->addLayout(infoLayout);

    // Progress bar
    m_progressBar = new ElaProgressBar(this);
    m_progressBar->setFixedHeight(10);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    mainLayout->addWidget(m_progressBar);

    // Buttons layout
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    ElaPushButton* websiteButton = new ElaPushButton("访问网站", this);
    connect(websiteButton, &ElaPushButton::clicked, this, &T_About::onWebsiteClicked);
    
    ElaPushButton* updateButton = new ElaPushButton("检查更新", this);
    connect(updateButton, &ElaPushButton::clicked, this, &T_About::onUpdateClicked);

    buttonLayout->addWidget(websiteButton);
    buttonLayout->addWidget(updateButton);

    mainLayout->addLayout(buttonLayout);

    // Copyright
    ElaText* copyrightText = new ElaText("版权所有 © 2024 Liniyous", this);
    copyrightText->setTextPixelSize(12);
    copyrightText->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(copyrightText);

    // Set up timer for progress bar animation
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &T_About::updateProgressBar);
}

T_About::~T_About()
{
}

void T_About::addInfoText(QVBoxLayout* layout, const QString& label, const QString& value)
{
    QHBoxLayout* rowLayout = new QHBoxLayout();
    
    ElaText* labelText = new ElaText(label, this);
    labelText->setTextPixelSize(14);
    labelText->setFont(QFont("Arial", 14, QFont::Bold));
    
    ElaText* valueText = new ElaText(value, this);
    valueText->setTextPixelSize(14);
    valueText->setTextInteractionFlags(Qt::TextSelectableByMouse);
    
    rowLayout->addWidget(labelText);
    rowLayout->addWidget(valueText, 1);
    
    layout->addLayout(rowLayout);
}

void T_About::onWebsiteClicked()
{
    QDesktopServices::openUrl(QUrl("https://www.example.com"));
}

void T_About::onUpdateClicked()
{
    m_progressValue = 0;
    m_progressBar->setValue(0);
    m_timer->start(50);
}

void T_About::updateProgressBar()
{
    m_progressValue += 2;
    m_progressBar->setValue(m_progressValue);
    
    if (m_progressValue >= 100)
    {
        m_timer->stop();
        // Here you would typically check for updates and show the result
        // For demonstration, we'll just show a message
        ElaText* updateText = new ElaText("已是最新版本", this);
        updateText->setTextPixelSize(14);
        updateText->setAlignment(Qt::AlignCenter);
        qobject_cast<QVBoxLayout*>(layout())->addWidget(updateText);
    }
}