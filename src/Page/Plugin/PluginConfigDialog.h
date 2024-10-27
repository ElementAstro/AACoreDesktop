#ifndef PLUGINCONFIGDIALOG_H
#define PLUGINCONFIGDIALOG_H

#include <QDialog>
#include <QFormLayout>

class ElaLineEdit;

class PluginConfigDialog : public QDialog {
    Q_OBJECT

public:
    explicit PluginConfigDialog(const QString &pluginName,
                                QWidget *parent = nullptr);

private:
    ElaLineEdit *configField1;
    ElaLineEdit *configField2;

    void setupUI(const QString &pluginName);
    void resetFields();

private slots:
    void onSaveClicked();
    void onResetClicked();
};

#endif  // PLUGINCONFIGDIALOG_H
