#include "T_Guider.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>


#include "Components/C_InfoCard.h"
#include "ElaColorDialog.h"
#include "ElaComboBox.h"
#include "ElaIconButton.h"
#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "ElaTabWidget.h"
#include "ElaText.h"
#include "ElaToggleSwitch.h"
#include "T_Guider_Setting.h"

namespace {
constexpr int kSpacing20 = 20;
constexpr int kFixedSize40 = 40;
constexpr int kFixedSize30 = 30;
constexpr int kMagicNumber100 = 100;
constexpr double kMagicNumber01 = 0.1;
constexpr int kTimerInterval = 1000;  // 1秒
}  // namespace

T_GuiderPage::T_GuiderPage(QWidget *parent) : T_BasePage(parent) {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(kSpacing20);
    mainLayout->setContentsMargins(kSpacing20, kSpacing20, kSpacing20,
                                   kSpacing20);

    // Top section
    auto *topLayout = createTopLayout();
    mainLayout->addLayout(topLayout);

    // Create tab widget
    auto *tabWidget = new ElaTabWidget(this);

    // Create and add "信息" tab
    auto *infoTab = createInfoTab();
    tabWidget->addTab(infoTab, "信息");

    // Create and add "控制" tab
    auto *controlTab = createControlTab();
    tabWidget->addTab(controlTab, "控制");

    // Create and add "设置" tab
    auto *settingsTab = createSettingsTab();
    tabWidget->addTab(settingsTab, "设置");

    mainLayout->addWidget(tabWidget);

    auto *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("导星");
    auto *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);

    _phd2SetupDialog = new T_PHD2SetupDialog(this);
    _phd2SetupDialog->hide();
    connect(_settingButton, &ElaPushButton::clicked, _phd2SetupDialog,
            &T_PHD2SetupDialog::show);
}

T_GuiderPage::~T_GuiderPage() {}

QHBoxLayout *T_GuiderPage::createTopLayout() {
    auto *topLayout = new QHBoxLayout();

    auto *guiderCombo = new ElaComboBox(this);
    guiderCombo->addItem("PHD2");
    connect(guiderCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
                // 处理导星器选择变化
            });

    auto createIconButton = [this](ElaIconType::IconName icon) {
        auto *button = new ElaIconButton(icon, this);
        button->setFixedSize(kFixedSize40, kFixedSize40);
        return button;
    };

    _settingButton = createIconButton(ElaIconType::Gears);
    auto *refreshButton = createIconButton(ElaIconType::ArrowsRotate);
    auto *powerButton = createIconButton(ElaIconType::PowerOff);

    topLayout->addWidget(guiderCombo, 1);
    topLayout->addWidget(_settingButton);
    topLayout->addWidget(refreshButton);
    topLayout->addWidget(powerButton);

    return topLayout;
}

QWidget *T_GuiderPage::createInfoTab() {
    auto *infoWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(infoWidget);

    layout->addWidget(new InfoCard("连接状态", "已连接", this));
    layout->addWidget(new InfoCard("状态", "循环中", this));
    layout->addWidget(new InfoCard("像素比例", "1.31 arcsec/px", this));

    // 添加图表
    auto *chartView = createGuideChart();
    layout->addWidget(chartView);

    layout->addStretch();
    return infoWidget;
}

QWidget *T_GuiderPage::createControlTab() {
    auto *controlWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(controlWidget);

    // 显示修正图开关
    auto *graphShowLayout = new QHBoxLayout();
    auto *graphShowLabel = new ElaText("显示修正图", this);
    auto *graphShowSwitch = new ElaToggleSwitch(this);
    connect(graphShowSwitch, &ElaToggleSwitch::toggled, this,
            [this](bool checked) {
                // 处理显示修正图切换
            });
    graphShowLayout->addWidget(graphShowLabel);
    graphShowLayout->addWidget(graphShowSwitch);
    graphShowLayout->addStretch();
    layout->addLayout(graphShowLayout);

    // RA颜色选择
    auto *raColorLayout = new QHBoxLayout();
    auto *raColorLabel = new ElaText("RA图形颜色", this);
    auto *raColorButton = new ElaPushButton(this);
    raColorButton->setFixedSize(kFixedSize30, kFixedSize30);
    raColorButton->setStyleSheet("background-color: blue;");
    connect(raColorButton, &ElaPushButton::clicked, this,
            &T_GuiderPage::onRAColorButtonClicked);
    raColorLayout->addWidget(raColorLabel);
    raColorLayout->addWidget(raColorButton);
    raColorLayout->addStretch();
    layout->addLayout(raColorLayout);

    // Dec颜色选择
    auto *decColorLayout = new QHBoxLayout();
    auto *decColorLabel = new ElaText("Dec图形颜色", this);
    auto *decColorButton = new ElaPushButton(this);
    decColorButton->setFixedSize(kFixedSize30, kFixedSize30);
    decColorButton->setStyleSheet("background-color: red;");
    connect(decColorButton, &ElaPushButton::clicked, this,
            &T_GuiderPage::onDecColorButtonClicked);
    decColorLayout->addWidget(decColorLabel);
    decColorLayout->addWidget(decColorButton);
    decColorLayout->addStretch();
    layout->addLayout(decColorLayout);

    layout->addStretch();
    return controlWidget;
}

QWidget *T_GuiderPage::createSettingsTab() {
    auto *settingsWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(settingsWidget);

    auto addSettingRow = [this, layout](
                             const QString &label, const QString &value,
                             const QString &unit, bool hasToggle = false) {
        auto *rowLayout = new QHBoxLayout();
        auto *labelWidget = new ElaText(label, this);
        labelWidget->setTextPixelSize(15);
        rowLayout->addWidget(labelWidget);
        auto *settingEdit = new ElaLineEdit(this);
        settingEdit->setText(value);
        rowLayout->addWidget(settingEdit);
        rowLayout->addWidget(new ElaText(unit, this));
        if (hasToggle) {
            auto *toggle = new ElaToggleSwitch(this);
            rowLayout->addWidget(toggle);
        }
        rowLayout->addStretch();
        layout->addLayout(rowLayout);
    };

    addSettingRow("抖动像素", "5", "px");
    addSettingRow("稳定像素容差", "1.5", "px");
    addSettingRow("稳定超时", "40", "s");
    addSettingRow("导星开始超时", "300", "s");

    // PHD2配置
    auto *profileLayout = new QHBoxLayout();
    auto *profileLabel = new ElaText("PHD2配置", this);
    auto *profileCombo = new ElaComboBox(this);
    profileCombo->addItem("Default");
    profileLayout->addWidget(profileLabel);
    profileLayout->addWidget(profileCombo);
    profileLayout->addStretch();
    layout->addLayout(profileLayout);

    addSettingRow("仅在RA中抖动", "", "", true);
    addSettingRow("最小稳定时间", "10", "s");
    addSettingRow("导星开始重试", "", "", true);
    addSettingRow("寻找导星的ROI百分比", "100", "%");

    layout->addStretch();
    return settingsWidget;
}

QGroupBox *T_GuiderPage::createInfoGroup(const QString &title) {
    auto *groupBox = new QGroupBox(title, this);
    groupBox->setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            border: 1px solid #bbb;
            border-radius: 5px;
            margin-top: 10px;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 3px 0 3px;
        }
    )");
    return groupBox;
}

QChartView *T_GuiderPage::createGuideChart() {
    auto *chart = new QChart();
    chart->setTitle("导星图");

    auto *raSeries = new QLineSeries();
    raSeries->setName("RA修正");
    auto *decSeries = new QLineSeries();
    decSeries->setName("Dec修正");

    for (int i = 0; i < kMagicNumber100; ++i) {
        raSeries->append(i, qSin(i * kMagicNumber01));
        decSeries->append(i, qCos(i * kMagicNumber01));
    }

    chart->addSeries(raSeries);
    chart->addSeries(decSeries);

    auto *axisX = new QValueAxis();
    axisX->setTitleText("时间");
    chart->addAxis(axisX, Qt::AlignBottom);
    raSeries->attachAxis(axisX);
    decSeries->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setTitleText("偏差");
    chart->addAxis(axisY, Qt::AlignLeft);
    raSeries->attachAxis(axisY);
    decSeries->attachAxis(axisY);

    chart->setTheme(QChart::ChartThemeDark);

    auto *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView;
}

void T_GuiderPage::onRAColorButtonClicked() {
    ElaColorDialog colorDialog(this);
    colorDialog.setCurrentColor(Qt::blue);
    if (colorDialog.exec() == QDialog::Accepted) {
        QColor color = colorDialog.getCurrentColor();
        if (color.isValid()) {
            auto *button = qobject_cast<ElaPushButton *>(sender());
            if (button) {
                button->setStyleSheet(
                    QString("background-color: %1;").arg(color.name()));
                // 更新图表RA线条颜色的逻辑可以在这里添加
            }
        }
    }
}

void T_GuiderPage::onDecColorButtonClicked() {
    ElaColorDialog colorDialog(this);
    colorDialog.setCurrentColor(Qt::red);
    if (colorDialog.exec() == QDialog::Accepted) {
        QColor color = colorDialog.getCurrentColor();
        if (color.isValid()) {
            auto *button = qobject_cast<ElaPushButton *>(sender());
            if (button) {
                button->setStyleSheet(
                    QString("background-color: %1;").arg(color.name()));
                // 更新图表Dec线条颜色的逻辑可以在这里添加
            }
        }
    }
}