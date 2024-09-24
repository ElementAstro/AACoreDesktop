#include "T_SimpleSequencer.h"
#include "T_BasePage.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QStandardItemModel>

#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "ElaSpinBox.h"
#include "ElaTableView.h"
#include "ElaText.h"
#include "ElaToggleSwitch.h"

namespace {
const int TextPixelSize = 15;
const int LayoutSpacing = 10;
const int LayoutMargin = 10;
const int ChartHeight = 200;
const int AxisYRange = 90;
const int AxisXRange = 24;
const int AxisXTickCount = 9;
const int ModelColumnCount = 8;
}  // namespace

T_SimpleSequencerPage::T_SimpleSequencerPage(QWidget *parent)
    : T_BasePage(parent),
      mainLayout(nullptr),
      topWidget(nullptr),
      middleWidget(nullptr),
      bottomWidget(nullptr),
      model(nullptr),
      coolCameraSwitch(nullptr),
      unparkMountSwitch(nullptr),
      meridianFlipSwitch(nullptr),
      warmCameraSwitch(nullptr),
      parkMountSwitch(nullptr),
      delayStartSpinBox(nullptr),
      sequenceModeCombo(nullptr),
      estimatedDownloadTimeEdit(nullptr),
      estimatedFinishTimeEdit(nullptr),
      estFinishTimeThisTargetEdit(nullptr),
      targetTable(nullptr),
      chartView(nullptr),
      backButton(nullptr),
      addButton(nullptr),
      deleteButton(nullptr),
      resetButton(nullptr),
      moveUpButton(nullptr),
      moveDownButton(nullptr),
      startButton(nullptr) {
    setupUI();
    applyStyles();
}

void T_SimpleSequencerPage::setupUI() {
    mainLayout = new QVBoxLayout(this);

    createTopSection();
    createMiddleSection();
    createBottomSection();
    createControlButtons();

    mainLayout->addWidget(topWidget);
    mainLayout->addWidget(middleWidget);
    mainLayout->addWidget(bottomWidget);
    mainLayout->setSpacing(LayoutSpacing);
    mainLayout->setContentsMargins(LayoutMargin, LayoutMargin, LayoutMargin,
                                   LayoutMargin);

    auto centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("简单序列面板");
    auto centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

void T_SimpleSequencerPage::createTopSection() {
    topWidget = new QWidget(this);
    auto topLayout = new QHBoxLayout(topWidget);

    auto startOptionsWidget = new QWidget(topWidget);
    auto startOptionsLayout = new QVBoxLayout(startOptionsWidget);
    auto targetSetStartText =
        new ElaText("Target Set Start Options", startOptionsWidget);
    targetSetStartText->setTextPixelSize(TextPixelSize);

    auto switchesWidget = new QWidget(startOptionsWidget);
    auto switchesLayout = new QHBoxLayout(switchesWidget);

    coolCameraSwitch = new ElaToggleSwitch(switchesWidget);
    auto coolCameraLabel = new ElaText("Cool Camera", switchesWidget);
    coolCameraLabel->setTextPixelSize(TextPixelSize);
    unparkMountSwitch = new ElaToggleSwitch(switchesWidget);
    auto unparkMountLabel = new ElaText("Unpark Mount", switchesWidget);
    unparkMountLabel->setTextPixelSize(TextPixelSize);
    meridianFlipSwitch = new ElaToggleSwitch(switchesWidget);
    auto meridianFlipLabel = new ElaText("Meridian Flip", switchesWidget);
    meridianFlipLabel->setTextPixelSize(TextPixelSize);

    switchesLayout->addWidget(coolCameraSwitch);
    switchesLayout->addWidget(coolCameraLabel);
    switchesLayout->addWidget(unparkMountSwitch);
    switchesLayout->addWidget(unparkMountLabel);
    switchesLayout->addWidget(meridianFlipSwitch);
    switchesLayout->addWidget(meridianFlipLabel);
    switchesLayout->addStretch();

    startOptionsLayout->addWidget(targetSetStartText);
    startOptionsLayout->addWidget(switchesWidget);

    auto endOptionsWidget = new QWidget(topWidget);
    auto endOptionsLayout = new QVBoxLayout(endOptionsWidget);
    auto targetSetEndText =
        new ElaText("Target Set End Options", endOptionsWidget);
    targetSetEndText->setTextPixelSize(TextPixelSize);

    auto endSwitchesWidget = new QWidget(endOptionsWidget);
    auto endSwitchesLayout = new QHBoxLayout(endSwitchesWidget);

    warmCameraSwitch = new ElaToggleSwitch(endSwitchesWidget);
    auto warmCameraLabel = new ElaText("Warm Camera", endSwitchesWidget);
    warmCameraLabel->setTextPixelSize(TextPixelSize);
    parkMountSwitch = new ElaToggleSwitch(endSwitchesWidget);
    auto parkMountLabel = new ElaText("Park Mount", endSwitchesWidget);
    parkMountLabel->setTextPixelSize(TextPixelSize);

    endSwitchesLayout->addWidget(warmCameraSwitch);
    endSwitchesLayout->addWidget(warmCameraLabel);
    endSwitchesLayout->addWidget(parkMountSwitch);
    endSwitchesLayout->addWidget(parkMountLabel);
    endSwitchesLayout->addStretch();

    endOptionsLayout->addWidget(targetSetEndText);
    endOptionsLayout->addWidget(endSwitchesWidget);

    topLayout->addWidget(startOptionsWidget);
    topLayout->addStretch();
    topLayout->addWidget(endOptionsWidget);
}

void T_SimpleSequencerPage::createMiddleSection() {
    middleWidget = new QWidget(this);
    auto middleLayout = new QGridLayout(middleWidget);

    auto delayStartLabel = new ElaText("Delay start:", middleWidget);
    delayStartLabel->setTextPixelSize(TextPixelSize);
    middleLayout->addWidget(delayStartLabel, 0, 0);
    delayStartSpinBox = new ElaSpinBox(middleWidget);
    delayStartSpinBox->setSuffix(" s");
    middleLayout->addWidget(delayStartSpinBox, 0, 1);

    auto sequenceModeLabel = new ElaText("Sequence mode:", middleWidget);
    sequenceModeLabel->setTextPixelSize(TextPixelSize);
    middleLayout->addWidget(sequenceModeLabel, 1, 0);
    sequenceModeCombo = new ElaComboBox(middleWidget);
    sequenceModeCombo->addItem("One after another");
    middleLayout->addWidget(sequenceModeCombo, 1, 1);

    auto estimatedDownloadTimeLabel =
        new ElaText("Estimated download time:", middleWidget);
    estimatedDownloadTimeLabel->setTextPixelSize(TextPixelSize);
    middleLayout->addWidget(estimatedDownloadTimeLabel, 2, 0);
    estimatedDownloadTimeEdit = new ElaLineEdit(middleWidget);
    estimatedDownloadTimeEdit->setReadOnly(true);
    middleLayout->addWidget(estimatedDownloadTimeEdit, 2, 1);

    auto estimatedFinishTimeLabel =
        new ElaText("Estimated finish time:", middleWidget);
    estimatedFinishTimeLabel->setTextPixelSize(TextPixelSize);
    middleLayout->addWidget(estimatedFinishTimeLabel, 3, 0);
    estimatedFinishTimeEdit = new QDateTimeEdit(middleWidget);
    estimatedFinishTimeEdit->setReadOnly(true);
    estimatedFinishTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    middleLayout->addWidget(estimatedFinishTimeEdit, 3, 1);

    auto estFinishTimeThisTargetLabel =
        new ElaText("Est. finish time (this target):", middleWidget);
    estFinishTimeThisTargetLabel->setTextPixelSize(TextPixelSize);
    middleLayout->addWidget(estFinishTimeThisTargetLabel, 4, 0);
    estFinishTimeThisTargetEdit = new QDateTimeEdit(middleWidget);
    estFinishTimeThisTargetEdit->setReadOnly(true);
    estFinishTimeThisTargetEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    middleLayout->addWidget(estFinishTimeThisTargetEdit, 4, 1);
}

void T_SimpleSequencerPage::createBottomSection() {
    bottomWidget = new QWidget(this);
    auto bottomLayout = new QVBoxLayout(bottomWidget);

    targetTable = new ElaTableView(bottomWidget);
    model = new QStandardItemModel(1, ModelColumnCount, this);
    model->setHorizontalHeaderLabels({"Enabled", "Progress", "Total #", "Time",
                                      "Type", "Filter", "Binning", "Dither"});

    auto enabledItem = new QStandardItem();
    enabledItem->setCheckable(true);
    enabledItem->setCheckState(Qt::Checked);
    model->setItem(0, 0, enabledItem);

    model->setItem(0, 1, new QStandardItem("0 / 1"));
    model->setItem(0, 2, new QStandardItem("1"));
    model->setItem(0, 3, new QStandardItem("1 s"));
    model->setItem(0, 4, new QStandardItem("LIGHT"));
    model->setItem(0, 5, new QStandardItem(""));
    model->setItem(0, 6, new QStandardItem("1x1"));
    model->setItem(0, 7, new QStandardItem("OFF"));

    targetTable->setModel(model);
    targetTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    targetTable->verticalHeader()->setVisible(false);

    bottomLayout->addWidget(targetTable);

    createChart();
    // bottomLayout->addWidget(chartView);
}

void T_SimpleSequencerPage::createChart() {
    auto set = new QBarSet("RA");
    *set << 90 << 60 << 30 << 0;

    auto series = new QBarSeries();
    series->append(set);

    auto chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Target");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setBackgroundVisible(false);
    chart->legend()->hide();

    auto axisY = new QValueAxis();
    axisY->setRange(0, AxisYRange);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    auto axisX = new QValueAxis();
    axisX->setRange(0, AxisXRange);
    axisX->setTickCount(AxisXTickCount);
    axisX->setLabelFormat("%.0f");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setFixedHeight(ChartHeight);
}

void T_SimpleSequencerPage::createControlButtons() {
    auto buttonWidget = new QWidget(this);
    auto buttonLayout = new QHBoxLayout(buttonWidget);

    backButton = new ElaPushButton("Back", buttonWidget);
    addButton = new ElaPushButton("Add", buttonWidget);
    deleteButton = new ElaPushButton("Delete", buttonWidget);
    resetButton = new ElaPushButton("Reset", buttonWidget);
    moveUpButton = new ElaPushButton("Move Up", buttonWidget);
    moveDownButton = new ElaPushButton("Move Down", buttonWidget);
    startButton = new ElaPushButton("Start", buttonWidget);

    buttonLayout->addWidget(backButton);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(resetButton);
    buttonLayout->addWidget(moveUpButton);
    buttonLayout->addWidget(moveDownButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(startButton);

    mainLayout->addWidget(buttonWidget);

    connect(backButton, &QPushButton::clicked, this,
            &T_SimpleSequencerPage::onBackButtonClicked);
    connect(addButton, &QPushButton::clicked, this,
            &T_SimpleSequencerPage::onAddButtonClicked);
    connect(deleteButton, &QPushButton::clicked, this,
            &T_SimpleSequencerPage::onDeleteButtonClicked);
    connect(resetButton, &QPushButton::clicked, this,
            &T_SimpleSequencerPage::onResetButtonClicked);
    connect(moveUpButton, &QPushButton::clicked, this,
            &T_SimpleSequencerPage::onMoveUpButtonClicked);
    connect(moveDownButton, &QPushButton::clicked, this,
            &T_SimpleSequencerPage::onMoveDownButtonClicked);
    connect(startButton, &QPushButton::clicked, this,
            &T_SimpleSequencerPage::onStartButtonClicked);
}

void T_SimpleSequencerPage::applyStyles() {}

void T_SimpleSequencerPage::onBackButtonClicked() {
    QMessageBox::information(this, "Back", "Back button clicked");
}

void T_SimpleSequencerPage::onAddButtonClicked() {
    int rowCount = model->rowCount();
    model->insertRow(rowCount);

    auto enabledItem = new QStandardItem();
    enabledItem->setCheckable(true);
    enabledItem->setCheckState(Qt::Checked);
    model->setItem(rowCount, 0, enabledItem);

    model->setItem(rowCount, 1, new QStandardItem("0 / 1"));
    model->setItem(rowCount, 2, new QStandardItem("1"));
    model->setItem(rowCount, 3, new QStandardItem("1 s"));
    model->setItem(rowCount, 4, new QStandardItem("LIGHT"));
    model->setItem(rowCount, 5, new QStandardItem(""));
    model->setItem(rowCount, 6, new QStandardItem("1x1"));
    model->setItem(rowCount, 7, new QStandardItem("OFF"));
}

void T_SimpleSequencerPage::onDeleteButtonClicked() {
    auto selectionModel = targetTable->selectionModel();
    auto selectedRows = selectionModel->selectedRows();

    for (const auto &index : selectedRows) {
        model->removeRow(index.row());
    }
}

void T_SimpleSequencerPage::onResetButtonClicked() {
    model->removeRows(0, model->rowCount());
    onAddButtonClicked();  // 添加一行默认数据
}

void T_SimpleSequencerPage::onMoveUpButtonClicked() {
    auto selectionModel = targetTable->selectionModel();
    auto selectedRows = selectionModel->selectedRows();

    if (selectedRows.isEmpty() || selectedRows.first().row() == 0) {
        return;
    }

    int currentRow = selectedRows.first().row();
    model->insertRow(currentRow - 1, model->takeRow(currentRow));
    selectionModel->select(model->index(currentRow - 1, 0),
                           QItemSelectionModel::Select);
}

void T_SimpleSequencerPage::onMoveDownButtonClicked() {
    auto selectionModel = targetTable->selectionModel();
    auto selectedRows = selectionModel->selectedRows();

    if (selectedRows.isEmpty() ||
        selectedRows.first().row() == model->rowCount() - 1) {
        return;
    }

    int currentRow = selectedRows.first().row();
    model->insertRow(currentRow + 1, model->takeRow(currentRow));
    selectionModel->select(model->index(currentRow + 1, 0),
                           QItemSelectionModel::Select);
}

void T_SimpleSequencerPage::onStartButtonClicked() {
    QMessageBox::information(this, "Start", "Start button clicked");
}