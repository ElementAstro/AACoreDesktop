#include "T_DataHistory.h"
#include "SettingDialog.h"

#include "ElaText.h"

T_DataHistory::T_DataHistory(QWidget* parent) : T_BasePage(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 左侧面板
    QWidget* leftPanel = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);

    QLabel* infoLabel = new QLabel("Temperature Information");
    leftLayout->addWidget(infoLabel);

    QChartView* chartView = new QChartView();
    leftLayout->addWidget(chartView);

    // 右侧面板（设置）
    SettingsWidget* settings = new SettingsWidget();

    // 使用 QSplitter 将左侧面板和右侧面板分开
    QSplitter* splitter = new QSplitter(this);
    splitter->addWidget(leftPanel);
    splitter->addWidget(settings);

    // 将 splitter 添加到 mainLayout
    mainLayout->addWidget(splitter);

    // 设置 centralWidget
    auto* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("数据历史");
    auto* centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}