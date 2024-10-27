#ifndef T_SETTING_H
#define T_SETTING_H

#include "T_BasePage.h"

class ElaRadioButton;
class ElaToggleSwitch;
class ElaComboBox;

class T_Setting : public T_BasePage {
    Q_OBJECT
public:
    Q_INVOKABLE explicit T_Setting(QWidget* parent = nullptr);
    ~T_Setting();

private:
    void loadFromConfig();  // 从配置加载设置
    void saveToConfig();    // 将设置保存到配置

    // UI 控件指针
    ElaComboBox* _themeComboBox{nullptr};
    ElaToggleSwitch* _micaSwitchButton{nullptr};
    ElaToggleSwitch* _logSwitchButton{nullptr};
    ElaRadioButton* _minimumButton{nullptr};
    ElaRadioButton* _compactButton{nullptr};
    ElaRadioButton* _maximumButton{nullptr};
    ElaRadioButton* _autoButton{nullptr};
};

#endif  // T_SETTING_H
