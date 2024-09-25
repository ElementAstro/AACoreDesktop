#include "PluginConfigDialog.h"

#include <QMessageBox>
#include <QVBoxLayout>

#include "ElaLineEdit.h"
#include "ElaPushButton.h"

PluginConfigDialog::PluginConfigDialog(const QString &pluginName,
                                       QWidget *parent)
    : QDialog(parent) {
    setupUI(pluginName);  // 设置UI界面
}

void PluginConfigDialog::setupUI(const QString &pluginName) {
    setWindowTitle("Configure " + pluginName);

    // 配置字段输入
    configField1 = new ElaLineEdit(this);
    configField2 = new ElaLineEdit(this);

    // 布局管理器
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Config Option 1:", configField1);
    formLayout->addRow("Config Option 2:", configField2);

    // 保存按钮
    ElaPushButton *saveButton = new ElaPushButton("Save", this);
    connect(saveButton, &ElaPushButton::clicked, this,
            &PluginConfigDialog::onSaveClicked);

    // 垂直布局
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(formLayout);
    layout->addWidget(saveButton);

    setLayout(layout);
}

void PluginConfigDialog::onSaveClicked() {
    // 获取输入的配置值
    QString config1 = configField1->text();
    QString config2 = configField2->text();

    // 这里可以将配置保存到文件或数据库（简单示例弹出提示框）
    QMessageBox::information(this, "Configurations Saved",
                             "Config 1: " + config1 + "\nConfig 2: " + config2);
    accept();  // 关闭对话框
}
