#ifndef T_BASEPAGE_H
#define T_BASEPAGE_H

#include <ElaScrollPage.h>

class QVBoxLayout;
class T_BasePage : public ElaScrollPage {
    Q_OBJECT
public:
    explicit T_BasePage(QWidget* parent = nullptr);
    ~T_BasePage();

protected:
    QVBoxLayout* createTopLayout(const QString &desText);
};

#endif  // T_BASEPAGE_H
