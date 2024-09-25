#ifndef PLUGINCONFIGDIALOG_H
#define PLUGINCONFIGDIALOG_H

#include <QDialog>
#include <QFormLayout>

class ElaLineEdit;

class PluginConfigDialog : public QDialog {
    Q_OBJECT

public:
    explicit PluginConfigDialog(const QString& pluginName, QWidget *parent = nullptr);

private:
    ElaLineEdit *configField1;  // 配置字段示例
    ElaLineEdit *configField2;  // 配置字段示例

    void setupUI(const QString& pluginName);

private slots:
    void onSaveClicked();  // 点击保存按钮的槽函数
};

#endif // PLUGINCONFIGDIALOG_H
