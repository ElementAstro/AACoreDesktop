#ifndef I18NMANAGER_H
#define I18NMANAGER_H

#include "T_BasePage.h"

#include <QTranslator>
#include <QVBoxLayout>
#include <QWidget>

class ElaComboBox;
class ElaPushButton;

class T_I18NPage : public T_BasePage {
    Q_OBJECT

public:
    explicit T_I18NPage(QWidget* parent = nullptr);
    ~T_I18NPage();

signals:
    void languageChanged(const QString& languageCode);  // 当语言改变时发出信号

public slots:
    void on_applyButton_clicked();  // 用户点击“应用”按钮时的处理

private:
    ElaComboBox* languageComboBox;  // 语言选择框
    ElaPushButton* applyButton;     // 应用按钮
    QVBoxLayout* layout;            // 布局管理器

    void loadLanguages();      // 加载可用语言列表
    void showRestartPrompt();  // 显示重启提示框
    QString getCurrentLanguageDisplayName(
        const QString& languageCode);  // 获取当前语言名称
};

#endif  // I18NMANAGER_H
