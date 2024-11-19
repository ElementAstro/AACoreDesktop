#include <QMainWindow>
#include <QSplitter>
#include <QChartView>
#include <QVBoxLayout>

#include "T_BasePage.h"

class T_DataHistory : public T_BasePage{
    Q_OBJECT

public:
    T_DataHistory(QWidget* parent = nullptr);
};
