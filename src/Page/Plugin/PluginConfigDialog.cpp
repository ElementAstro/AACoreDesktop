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
    setFixedSize(300, 200);

    // 配置字段输入
    configField1 = new ElaLineEdit(this);
    configField1->setPlaceholderText("Enter first config option...");
    configField2 = new ElaLineEdit(this);
    configField2->setPlaceholderText("Enter second config option...");

    // 布局管理器
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Config Option 1:", configField1);
    formLayout->addRow("Config Option 2:", configField2);

    // 保存和重置按钮
    ElaPushButton *saveButton = new ElaPushButton("Save", this);
    ElaPushButton *resetButton = new ElaPushButton("Reset", this);
    saveButton->setStyleSheet(
        "QPushButton { background-color: #5AA9E6; color: white; padding: 8px; "
        "}");
    resetButton->setStyleSheet(
        "QPushButton { background-color: #D9534F; color: white; padding: 8px; "
        "}");

    connect(saveButton, &ElaPushButton::clicked, this,
            &PluginConfigDialog::onSaveClicked);
    connect(resetButton, &ElaPushButton::clicked, this,
            &PluginConfigDialog::onResetClicked);

    // 垂直布局
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(formLayout);

    // 将按钮添加到水平布局
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(resetButton);
    layout->addLayout(buttonLayout);

    setLayout(layout);
}

void PluginConfigDialog::onSaveClicked() {
    // 获取输入的配置值
    QString config1 = configField1->text();
    QString config2 = configField2->text();

    // 验证配置是否为空
    if (config1.isEmpty() || config2.isEmpty()) {
        QMessageBox::warning(this, "Incomplete Configurations",
                             "Please fill in both configuration options.");
        return;
    }

    // 这里可以将配置保存到文件或数据库（简单示例弹出提示框）
    QMessageBox::information(this, "Configurations Saved",
                             "Config 1: " + config1 + "\nConfig 2: " + config2);
    accept();  // 关闭对话框
}

void PluginConfigDialog::onResetClicked() {
    // 重置字段到默认值
    configField1->clear();
    configField2->clear();
    QMessageBox::information(this, "Configurations Reset",
                             "Configurations have been reset to default.");
}