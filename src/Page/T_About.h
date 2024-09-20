#ifndef T_ABOUT_H
#define T_ABOUT_H

#include "ElaWidget.h"

class QVBoxLayout;
class ElaProgressBar;

class T_About : public ElaWidget {
    Q_OBJECT

public:
    T_About(QWidget* parent = nullptr);
    ~T_About();

private slots:
    void onWebsiteClicked();
    void onUpdateClicked();
    void updateProgressBar();

private:
    void addInfoText(QVBoxLayout* layout, const QString& label,
                     const QString& value);

    ElaProgressBar* m_progressBar;
    QTimer* m_timer;
    int m_progressValue;
};

#endif  // T_ABOUT_H
