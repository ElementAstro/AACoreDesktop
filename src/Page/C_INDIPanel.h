// C_INDIPanel.h
#ifndef INDIDEVICEPANEL_H
#define INDIDEVICEPANEL_H

#include <QWidget>
#include <QTabWidget>
#include <QJsonObject>
#include <QMap>
#include <QPlainTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSlider>
#include <QCheckBox>

class ElaPlainTextEdit;

class C_INDIPanel : public QWidget
{
    Q_OBJECT

public:
    explicit C_INDIPanel(const QString &configFile, QWidget *parent = nullptr);

private:
    void setupUI();
    void loadConfiguration(const QString &filename);
    void createTabs(const QJsonObject &config);
    QWidget* createControlWidget(const QJsonObject &control);
    void applyStyles();

    QTabWidget *tabWidget;
    ElaPlainTextEdit *logTextEdit;
    QMap<QString, QWidget*> controlWidgets;

    void log(const QString &message);

private slots:
    void onControlValueChanged();
};
#endif