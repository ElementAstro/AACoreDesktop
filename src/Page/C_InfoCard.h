#ifndef INFOCARD_H
#define INFOCARD_H

#include <QWidget>
#include <QString>

class InfoCard : public QWidget {
    Q_OBJECT

public:
    InfoCard(const QString &title, const QString &value, QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setValue(const QString &value);

protected:

private:
    QString m_title;
    QString m_value;
};

#endif // INFOCARD_H