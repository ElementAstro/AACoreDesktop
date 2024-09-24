#include "T_BasePage.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QDate>

#include "ElaMenu.h"
#include "ElaText.h"
#include "ElaTheme.h"
#include "ElaToolButton.h"

namespace {
constexpr int kTextPixelSize11 = 11;
constexpr int kTextPixelSize13 = 13;
constexpr int kFixedSize35 = 35;
constexpr int kSpacing5 = 5;
constexpr int kSpacing15 = 15;
}  // namespace

T_BasePage::T_BasePage(QWidget* parent) : ElaScrollPage(parent) {}

T_BasePage::~T_BasePage() = default;

auto T_BasePage::createTopLayout(const QString& desText) -> QVBoxLayout* {
    // 顶部元素
    auto* subTitleText = new ElaText(this);
    subTitleText->setText("https://github.com/Liniyous/ElaWidgetTools");
    subTitleText->setTextInteractionFlags(Qt::TextSelectableByMouse);
    subTitleText->setTextPixelSize(kTextPixelSize11);

    auto* documentationButton = new ElaToolButton(this);
    documentationButton->setFixedHeight(kFixedSize35);
    documentationButton->setIsTransparent(false);
    documentationButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    documentationButton->setText("Documentation");
    documentationButton->setElaIcon(ElaIconType::FileDoc);
    auto* documentationMenu = new ElaMenu(this);
    documentationMenu->addElaIconAction(ElaIconType::CardsBlank, "CardsBlank");
    documentationMenu->addElaIconAction(ElaIconType::EarthAmericas,
                                        "EarthAmericas");
    documentationButton->setMenu(documentationMenu);

    auto* sourceButton = new ElaToolButton(this);
    sourceButton->setFixedHeight(kFixedSize35);
    sourceButton->setIsTransparent(false);
    sourceButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    sourceButton->setText("Source");
    sourceButton->setElaIcon(ElaIconType::NfcSymbol);
    auto* sourceMenu = new ElaMenu(this);
    sourceMenu->addElaIconAction(ElaIconType::FireBurner, "FireBurner");
    sourceMenu->addElaIconAction(ElaIconType::Galaxy, "Galaxy~~~~");
    sourceButton->setMenu(sourceMenu);

    auto* themeButton = new ElaToolButton(this);
    themeButton->setFixedSize(kFixedSize35, kFixedSize35);
    themeButton->setIsTransparent(false);
    themeButton->setElaIcon(ElaIconType::MoonStars);
    connect(themeButton, &ElaToolButton::clicked, this, [=]() {
        eTheme->setThemeMode(eTheme->getThemeMode() == ElaThemeType::Light
                                 ? ElaThemeType::Dark
                                 : ElaThemeType::Light);
    });

    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(documentationButton);
    buttonLayout->addSpacing(kSpacing5);
    buttonLayout->addWidget(sourceButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(themeButton);
    buttonLayout->addSpacing(kSpacing15);

    auto* descText = new ElaText(this);
    descText->setText(desText);
    descText->setTextPixelSize(kTextPixelSize13);

    auto* topLayout = new QVBoxLayout();
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->addWidget(subTitleText);
    topLayout->addSpacing(kSpacing5);
    topLayout->addLayout(buttonLayout);
    topLayout->addSpacing(kSpacing5);
    topLayout->addWidget(descText);
    return topLayout;
}