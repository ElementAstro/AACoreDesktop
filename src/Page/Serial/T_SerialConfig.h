#ifndef T_SERIALCONFIG_H
#define T_SERIALCONFIG_H

#include <QList>
#include <QStringList>
#include <QWidget>

#include "T_BasePage.h"

class ElaComboBox;
class ElaLineEdit;
class ElaPushButton;

class GetSerialPortTask : public QObject {
    Q_OBJECT
public slots:
    void run();

signals:
    void scanFinished(const QStringList &ports);
};

class T_SerialConfig : public T_BasePage {
    Q_OBJECT

public:
    explicit T_SerialConfig(QWidget *parent = nullptr);
    ~T_SerialConfig() override;

private slots:
    void onOkClicked();
    void onCancelClicked();
    void onComPortChanged(int index);
    void updateComPorts(const QStringList &ports);
    void saveConfig();
    void loadConfig();
    void onEditButtonClicked();
    void onLineEditReturnPressed();

private:
    void setupUi();
    void setupConnections();

    ElaComboBox *comComboBox;
    QList<ElaLineEdit *> dcLineEditList;
    QList<ElaPushButton *> dcEditButtonList;
    QList<ElaLineEdit *> pwmLineEditList;
    QList<ElaPushButton *> pwmEditButtonList;
    QStringList dcLabels;
    QStringList pwmLabels;

    ElaPushButton *okButton;
    ElaPushButton *cancelButton;
};

#endif  // T_SERIALCONFIG_H