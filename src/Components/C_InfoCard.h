#ifndef INFOCARD_H
#define INFOCARD_H

#include <QString>
#include <QWidget>


class InfoCard : public QWidget {
    Q_OBJECT

public:
    InfoCard(QString title, QString value,
             QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setValue(const QString &value);

protected:
private:
    QString m_title;
    QString m_value;
};

#endif  // INFOCARD_H