#include "T_Setting.h"
#include "Config/GlobalConfig.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "ElaApplication.h"
#include "ElaComboBox.h"
#include "ElaLog.h"
#include "ElaRadioButton.h"
#include "ElaScrollPageArea.h"
#include "ElaText.h"
#include "ElaTheme.h"
#include "ElaToggleSwitch.h"
#include "ElaWindow.h"

T_Setting::T_Setting(QWidget* parent) : T_BasePage(parent) {
    // 预览窗口标题
    auto* window = dynamic_cast<ElaWindow*>(parent);
    setWindowTitle("Setting");

    auto* themeText = new ElaText("主题设置", this);
    themeText->setWordWrap(false);
    themeText->setTextPixelSize(18);

    _themeComboBox = new ElaComboBox(this);
    _themeComboBox->addItem("日间模式");
    _themeComboBox->addItem("夜间模式");
    auto* themeSwitchArea = new ElaScrollPageArea(this);
    auto* themeSwitchLayout = new QHBoxLayout(themeSwitchArea);
    auto* themeSwitchText = new ElaText("主题切换", this);
    themeSwitchText->setWordWrap(false);
    themeSwitchText->setTextPixelSize(15);
    themeSwitchLayout->addWidget(themeSwitchText);
    themeSwitchLayout->addStretch();
    themeSwitchLayout->addWidget(_themeComboBox);
    connect(_themeComboBox,
            QOverload<int>::of(&ElaComboBox::currentIndexChanged), this,
            [=](int index) {
                if (index == 0) {
                    eTheme->setThemeMode(ElaThemeType::Light);
                } else {
                    eTheme->setThemeMode(ElaThemeType::Dark);
                }
                globalConfig.setState("Settings", "themeMode", index);
            });
    connect(eTheme, &ElaTheme::themeModeChanged, this,
            [=](ElaThemeType::ThemeMode themeMode) {
                _themeComboBox->blockSignals(true);
                if (themeMode == ElaThemeType::Light) {
                    _themeComboBox->setCurrentIndex(0);
                } else {
                    _themeComboBox->setCurrentIndex(1);
                }
                _themeComboBox->blockSignals(false);
            });

    auto* helperText = new ElaText("应用程序设置", this);
    helperText->setWordWrap(false);
    helperText->setTextPixelSize(18);

    _micaSwitchButton = new ElaToggleSwitch(this);
    auto* micaSwitchArea = new ElaScrollPageArea(this);
    auto* micaSwitchLayout = new QHBoxLayout(micaSwitchArea);
    auto* micaSwitchText = new ElaText("启用云母效果(跨平台)", this);
    micaSwitchText->setWordWrap(false);
    micaSwitchText->setTextPixelSize(15);
    micaSwitchLayout->addWidget(micaSwitchText);
    micaSwitchLayout->addStretch();
    micaSwitchLayout->addWidget(_micaSwitchButton);
    connect(_micaSwitchButton, &ElaToggleSwitch::toggled, this,
            [=](bool checked) {
                eApp->setIsEnableMica(checked);
                globalConfig.setState("Settings", "enableMica", checked);
            });

    _logSwitchButton = new ElaToggleSwitch(this);
    auto* logSwitchArea = new ElaScrollPageArea(this);
    auto* logSwitchLayout = new QHBoxLayout(logSwitchArea);
    auto* logSwitchText = new ElaText("启用日志功能", this);
    logSwitchText->setWordWrap(false);
    logSwitchText->setTextPixelSize(15);
    logSwitchLayout->addWidget(logSwitchText);
    logSwitchLayout->addStretch();
    logSwitchLayout->addWidget(_logSwitchButton);
    connect(_logSwitchButton, &ElaToggleSwitch::toggled, this,
            [=](bool checked) {
                ElaLog::getInstance()->initMessageLog(checked);
                globalConfig.setState("Settings", "enableLog", checked);
                if (checked) {
                    qDebug() << "日志已启用!";
                } else {
                    qDebug() << "日志已关闭!";
                }
            });

    _minimumButton = new ElaRadioButton("Minimum", this);
    _compactButton = new ElaRadioButton("Compact", this);
    _maximumButton = new ElaRadioButton("Maximum", this);
    _autoButton = new ElaRadioButton("Auto", this);
    _autoButton->setChecked(true);
    auto* displayModeArea = new ElaScrollPageArea(this);
    auto* displayModeLayout = new QHBoxLayout(displayModeArea);
    auto* displayModeText = new ElaText("导航栏模式选择", this);
    displayModeText->setWordWrap(false);
    displayModeText->setTextPixelSize(15);
    displayModeLayout->addWidget(displayModeText);
    displayModeLayout->addStretch();
    displayModeLayout->addWidget(_minimumButton);
    displayModeLayout->addWidget(_compactButton);
    displayModeLayout->addWidget(_maximumButton);
    displayModeLayout->addWidget(_autoButton);

    auto updateNavMode =
        [this, window](ElaNavigationType::NavigationDisplayMode type) {
            globalConfig.setState("Settings", "navigationMode",
                                  static_cast<int>(type));
            window->setNavigationBarDisplayMode(type);
        };

    connect(_minimumButton, &ElaRadioButton::toggled, this, [=](bool checked) {
        if (checked)
            updateNavMode(ElaNavigationType::Minimal);
    });
    connect(_compactButton, &ElaRadioButton::toggled, this, [=](bool checked) {
        if (checked)
            updateNavMode(ElaNavigationType::Compact);
    });
    connect(_maximumButton, &ElaRadioButton::toggled, this, [=](bool checked) {
        if (checked)
            updateNavMode(ElaNavigationType::Maximal);
    });
    connect(_autoButton, &ElaRadioButton::toggled, this, [=](bool checked) {
        if (checked)
            updateNavMode(ElaNavigationType::Auto);
    });

    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("Setting");
    QVBoxLayout* centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addSpacing(30);
    centerLayout->addWidget(themeText);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(themeSwitchArea);
    centerLayout->addSpacing(15);
    centerLayout->addWidget(helperText);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(logSwitchArea);
    centerLayout->addWidget(micaSwitchArea);
    centerLayout->addWidget(displayModeArea);
    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);

    // 初始化加载配置
    loadFromConfig();
}

void T_Setting::loadFromConfig() {
    auto settings = globalConfig.useStore(QString("Settings"));

    if (settings) {
        if (settings->contains("themeMode")) {
            _themeComboBox->setCurrentIndex(
                settings->value("themeMode").toInt());
        }
        if (settings->contains("enableMica")) {
            _micaSwitchButton->setIsToggled(
                settings->value("enableMica").toBool());
        }
        if (settings->contains("enableLog")) {
            _logSwitchButton->setIsToggled(
                settings->value("enableLog").toBool());
        }
        if (settings->contains("navigationMode")) {
            int navMode = settings->value("navigationMode").toInt();
            switch (static_cast<ElaNavigationType::NavigationDisplayMode>(
                navMode)) {
                case ElaNavigationType::Minimal:
                    _minimumButton->setChecked(true);
                    break;
                case ElaNavigationType::Compact:
                    _compactButton->setChecked(true);
                    break;
                case ElaNavigationType::Maximal:
                    _maximumButton->setChecked(true);
                    break;
                case ElaNavigationType::Auto:
                default:
                    _autoButton->setChecked(true);
                    break;
            }
        }
    }
}

T_Setting::~T_Setting() { saveToConfig(); }

void T_Setting::saveToConfig() {
    globalConfig.setState("Settings", "themeMode",
                          _themeComboBox->currentIndex());
    globalConfig.setState("Settings", "enableMica",
                          _micaSwitchButton->getIsToggled());
    globalConfig.setState("Settings", "enableLog",
                          _logSwitchButton->getIsToggled());

    if (_minimumButton->isChecked()) {
        globalConfig.setState("Settings", "navigationMode",
                              static_cast<int>(ElaNavigationType::Minimal));
    } else if (_compactButton->isChecked()) {
        globalConfig.setState("Settings", "navigationMode",
                              static_cast<int>(ElaNavigationType::Compact));
    } else if (_maximumButton->isChecked()) {
        globalConfig.setState("Settings", "navigationMode",
                              static_cast<int>(ElaNavigationType::Maximal));
    } else {
        globalConfig.setState("Settings", "navigationMode",
                              static_cast<int>(ElaNavigationType::Auto));
    }
}