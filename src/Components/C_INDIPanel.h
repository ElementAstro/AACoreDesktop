// C_INDIPanel.h
#ifndef INDIDEVICEPANEL_H
#define INDIDEVICEPANEL_H

#include <QJsonObject>
#include <QMap>
#include <QWidget>

class ElaPlainTextEdit;
class ElaTabWidget;

class C_INDIPanel : public QWidget {
    Q_OBJECT

public:
    explicit C_INDIPanel(const QString &configFile, QWidget *parent = nullptr);

private:
    void setupUI();
    void loadConfiguration(const QString &filename);
    void createTabs(const QJsonObject &config);
    QWidget *createControlWidget(const QJsonObject &control);
    void applyStyles();

    ElaTabWidget *tabWidget;
    ElaPlainTextEdit *logTextEdit;
    QMap<QString, QWidget *> controlWidgets;

    void log(const QString &message);

private slots:
    void onControlValueChanged();
};
#endif