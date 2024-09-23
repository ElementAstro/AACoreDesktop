#include "T_I18n.h"

#include <QIcon>
#include <QLocale>
#include <QMessageBox>
#include <QSettings>

#include "ElaComboBox.h"
#include "ElaPushButton.h"

T_I18NPage::T_I18NPage(QWidget *parent) : T_BasePage(parent) {
    // 创建语言选择框和应用按钮
    languageComboBox = new ElaComboBox(this);
    applyButton = new ElaPushButton("Apply", this);

    // 创建布局
    layout = new QVBoxLayout(this);
    layout->addWidget(languageComboBox);
    layout->addWidget(applyButton);

    // 设置布局到主窗口
    setLayout(layout);

    // 加载语言列表和样式
    loadLanguages();

    // 连接按钮点击信号
    connect(applyButton, &QPushButton::clicked, this,
            &T_I18NPage::on_applyButton_clicked);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("I18n");
    QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(layout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

T_I18NPage::~T_I18NPage() {}

// 加载语言列表
void T_I18NPage::loadLanguages() {
    // 添加语言和图标
    languageComboBox->addItem(QIcon(":/icons/english_flag.png"), "English",
                              "en");
    languageComboBox->addItem(QIcon(":/icons/chinese_flag.png"), "中文 (简体)",
                              "zh_CN");
    languageComboBox->addItem(QIcon(":/icons/french_flag.png"), "Français",
                              "fr");

    // 根据系统语言设置默认值
    QString systemLanguage = QLocale::system().name();
    int index = languageComboBox->findData(systemLanguage);
    if (index != -1) {
        languageComboBox->setCurrentIndex(index);
    }
}

// 应用按钮点击事件
void T_I18NPage::on_applyButton_clicked() {
    QString selectedLanguage = languageComboBox->currentData().toString();
    emit languageChanged(selectedLanguage);  // 触发语言变更信号

    showRestartPrompt();  // 提示用户重启应用
}

// 显示提示框，提醒用户重启应用
void T_I18NPage::showRestartPrompt() {
    QMessageBox::information(this, "Restart Required",
                             "The language change will take effect after "
                             "restarting the application.");
}

// 根据语言代码返回可读的语言名称
QString T_I18NPage::getCurrentLanguageDisplayName(const QString &languageCode) {
    if (languageCode == "en")
        return "English";
    if (languageCode == "zh_CN")
        return "中文 (简体)";
    if (languageCode == "fr")
        return "Français";
    return "Unknown";
}
