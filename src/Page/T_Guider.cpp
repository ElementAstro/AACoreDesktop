#include "T_Guider.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include "C_InfoCard.h"
#include "ElaComboBox.h"
#include "ElaIconButton.h"
#include "ElaTabWidget.h"
#include "ElaText.h"
#include "ElaToggleSwitch.h"
#include "T_Guider_Setting.h"

T_GuiderPage::T_GuiderPage(QWidget *parent) : T_BasePage(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Top section
    QHBoxLayout *topLayout = createTopLayout();
    mainLayout->addLayout(topLayout);

    // Create tab widget
    ElaTabWidget *tabWidget = new ElaTabWidget(this);

    // Create and add "Information" tab
    QWidget *infoTab = createInfoTab();
    tabWidget->addTab(infoTab, "信息");

    // Create and add "Control" tab
    QWidget *controlTab = createControlTab();
    tabWidget->addTab(controlTab, "控制");

    // Create and add "Settings" tab
    QWidget *settingsTab = createSettingsTab();
    tabWidget->addTab(settingsTab, "设置");

    mainLayout->addWidget(tabWidget);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("导星器面板");
    QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);

    _phd2SetupDialog = new T_PHD2SetupDialog(this);
    _phd2SetupDialog->hide();
    connect(_settingButton, &QPushButton::clicked, _phd2SetupDialog,
            &T_PHD2SetupDialog::show);
}

QHBoxLayout *T_GuiderPage::createTopLayout() {
    QHBoxLayout *topLayout = new QHBoxLayout();

    ElaComboBox *guiderCombo = new ElaComboBox(this);
    guiderCombo->addItem("PHD2");

    auto createIconButton = [this](ElaIconType::IconName icon) {
        ElaIconButton *button = new ElaIconButton(icon, this);
        button->setFixedSize(40, 40);
        return button;
    };

    _settingButton = createIconButton(ElaIconType::Gears);
    ElaIconButton *refreshButton = createIconButton(ElaIconType::ArrowsRotate);
    ElaIconButton *powerButton = createIconButton(ElaIconType::PowerOff);

    topLayout->addWidget(guiderCombo, 1);
    topLayout->addWidget(_settingButton);
    topLayout->addWidget(refreshButton);
    topLayout->addWidget(powerButton);

    return topLayout;
}

QWidget *T_GuiderPage::createInfoTab() {
    QWidget *infoWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(infoWidget);

    layout->addWidget(new InfoCard("连接状态", "已连接", this));
    layout->addWidget(new InfoCard("状态", "循环中", this));
    layout->addWidget(new InfoCard("像素比例", "1.31 arcsec/px", this));

    // 添加图表
    QChartView *chartView = createGuideChart();
    layout->addWidget(chartView);

    layout->addStretch();
    return infoWidget;
}

QWidget *T_GuiderPage::createControlTab() {
    QWidget *controlWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(controlWidget);

    QHBoxLayout *graphShowLayout = new QHBoxLayout();
    graphShowLayout->addWidget(new ElaText("显示修正图", this));
    graphShowLayout->addWidget(new ElaToggleSwitch(this));
    layout->addLayout(graphShowLayout);

    // RA颜色选择
    QHBoxLayout *raColorLayout = new QHBoxLayout();
    raColorLayout->addWidget(new ElaText("RA图形颜色", this));
    QPushButton *raColorButton = new QPushButton(this);
    raColorButton->setFixedSize(30, 30);
    raColorButton->setStyleSheet("background-color: blue;");
    connect(raColorButton, &QPushButton::clicked, this,
            &T_GuiderPage::onRAColorButtonClicked);
    raColorLayout->addWidget(raColorButton);
    layout->addLayout(raColorLayout);

    // Dec颜色选择
    QHBoxLayout *decColorLayout = new QHBoxLayout();
    decColorLayout->addWidget(new ElaText("Dec图形颜色", this));
    QPushButton *decColorButton = new QPushButton(this);
    decColorButton->setFixedSize(30, 30);
    decColorButton->setStyleSheet("background-color: red;");
    connect(decColorButton, &QPushButton::clicked, this,
            &T_GuiderPage::onDecColorButtonClicked);
    decColorLayout->addWidget(decColorButton);
    layout->addLayout(decColorLayout);

    layout->addStretch();
    return controlWidget;
}

QWidget *T_GuiderPage::createSettingsTab() {
    QWidget *settingsWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(settingsWidget);

    auto addSettingRow = [this, layout](
                             const QString &label, const QString &value,
                             const QString &unit, bool hasToggle = false) {
        QHBoxLayout *rowLayout = new QHBoxLayout();
        rowLayout->addWidget(new ElaText(label, this));
        rowLayout->addWidget(new QLineEdit(value, this));
        rowLayout->addWidget(new ElaText(unit, this));
        if (hasToggle) {
            rowLayout->addWidget(new ElaToggleSwitch(this));
        }
        layout->addLayout(rowLayout);
    };

    addSettingRow("抖动像素", "5", "px");
    addSettingRow("稳定像素容差", "1.5", "px");
    addSettingRow("稳定超时", "40", "s");
    addSettingRow("导星开始超时", "300", "s");

    QHBoxLayout *profileLayout = new QHBoxLayout();
    profileLayout->addWidget(new ElaText("PHD2配置", this));
    ElaComboBox *profileCombo = new ElaComboBox(this);
    profileCombo->addItem("test");
    profileLayout->addWidget(profileCombo);
    layout->addLayout(profileLayout);

    addSettingRow("仅在RA中抖动", "", "", true);
    addSettingRow("最小稳定时间", "10", "s");
    addSettingRow("导星开始重试", "", "", true);
    addSettingRow("寻找导星的ROI百分比", "100", "%");

    layout->addStretch();
    return settingsWidget;
}

QChartView *T_GuiderPage::createGuideChart() {
    QChart *chart = new QChart();
    chart->setTitle("导星图");

    QLineSeries *raSeries = new QLineSeries();
    raSeries->setName("RA修正");
    QLineSeries *decSeries = new QLineSeries();
    decSeries->setName("Dec修正");

    for (int i = 0; i < 100; ++i) {
        raSeries->append(i, qSin(i * 0.1));
        decSeries->append(i, qCos(i * 0.1));
    }

    chart->addSeries(raSeries);
    chart->addSeries(decSeries);

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("时间");
    chart->addAxis(axisX, Qt::AlignBottom);
    raSeries->attachAxis(axisX);
    decSeries->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("偏差");
    chart->addAxis(axisY, Qt::AlignLeft);
    raSeries->attachAxis(axisY);
    decSeries->attachAxis(axisY);

    chart->setTheme(QChart::ChartThemeDark);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView;
}

void T_GuiderPage::onRAColorButtonClicked() {
    QColor color = QColorDialog::getColor(Qt::blue, this, "选择RA图形颜色");
    if (color.isValid()) {
        QPushButton *button = qobject_cast<QPushButton *>(sender());
        if (button) {
            button->setStyleSheet(
                QString("background-color: %1;").arg(color.name()));
        }
        // 更新图表RA线条颜色的逻辑可以在这里添加
    }
}

void T_GuiderPage::onDecColorButtonClicked() {
    QColor color = QColorDialog::getColor(Qt::red, this, "选择Dec图形颜色");
    if (color.isValid()) {
        QPushButton *button = qobject_cast<QPushButton *>(sender());
        if (button) {
            button->setStyleSheet(
                QString("background-color: %1;").arg(color.name()));
        }
        // 更新图表Dec线条颜色的逻辑可以在这里添加
    }
}
