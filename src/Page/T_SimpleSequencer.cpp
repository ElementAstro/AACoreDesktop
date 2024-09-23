#include "T_SimpleSequencer.h"
#include "T_BasePage.h"

#include <QHeaderView>
#include <QStandardItemModel>
#include <QTableView>
#include <QMessageBox>

#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "ElaSpinBox.h"
#include "ElaTableView.h"
#include "ElaText.h"
#include "ElaToggleSwitch.h"

T_SimpleSequencerPage::T_SimpleSequencerPage(QWidget *parent)
    : T_BasePage(parent) {
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
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("简单序列面板");
    QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

void T_SimpleSequencerPage::createTopSection() {
    topWidget = new QWidget(this);
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);

    QWidget *startOptionsWidget = new QWidget(topWidget);
    QVBoxLayout *startOptionsLayout = new QVBoxLayout(startOptionsWidget);
    ElaText *targetSetStartText =
        new ElaText("Target Set Start Options", startOptionsWidget);
    targetSetStartText->setTextPixelSize(14);

    QWidget *switchesWidget = new QWidget(startOptionsWidget);
    QHBoxLayout *switchesLayout = new QHBoxLayout(switchesWidget);

    coolCameraSwitch = new ElaToggleSwitch(switchesWidget);
    QLabel *coolCameraLabel = new QLabel("Cool Camera", switchesWidget);
    unparkMountSwitch = new ElaToggleSwitch(switchesWidget);
    QLabel *unparkMountLabel = new QLabel("Unpark Mount", switchesWidget);
    meridianFlipSwitch = new ElaToggleSwitch(switchesWidget);
    QLabel *meridianFlipLabel = new QLabel("Meridian Flip", switchesWidget);

    switchesLayout->addWidget(coolCameraSwitch);
    switchesLayout->addWidget(coolCameraLabel);
    switchesLayout->addWidget(unparkMountSwitch);
    switchesLayout->addWidget(unparkMountLabel);
    switchesLayout->addWidget(meridianFlipSwitch);
    switchesLayout->addWidget(meridianFlipLabel);
    switchesLayout->addStretch();

    startOptionsLayout->addWidget(targetSetStartText);
    startOptionsLayout->addWidget(switchesWidget);

    QWidget *endOptionsWidget = new QWidget(topWidget);
    QVBoxLayout *endOptionsLayout = new QVBoxLayout(endOptionsWidget);
    ElaText *targetSetEndText =
        new ElaText("Target Set End Options", endOptionsWidget);
    targetSetEndText->setTextPixelSize(14);

    QWidget *endSwitchesWidget = new QWidget(endOptionsWidget);
    QHBoxLayout *endSwitchesLayout = new QHBoxLayout(endSwitchesWidget);

    warmCameraSwitch = new ElaToggleSwitch(endSwitchesWidget);
    QLabel *warmCameraLabel = new QLabel("Warm Camera", endSwitchesWidget);
    parkMountSwitch = new ElaToggleSwitch(endSwitchesWidget);
    QLabel *parkMountLabel = new QLabel("Park Mount", endSwitchesWidget);

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
    QGridLayout *middleLayout = new QGridLayout(middleWidget);

    middleLayout->addWidget(new QLabel("Delay start:"), 0, 0);
    delayStartSpinBox = new ElaSpinBox(middleWidget);
    delayStartSpinBox->setSuffix(" s");
    middleLayout->addWidget(delayStartSpinBox, 0, 1);

    middleLayout->addWidget(new QLabel("Sequence mode:"), 1, 0);
    sequenceModeCombo = new ElaComboBox(middleWidget);
    sequenceModeCombo->addItem("One after another");
    middleLayout->addWidget(sequenceModeCombo, 1, 1);

    middleLayout->addWidget(new QLabel("Estimated download time:"), 2, 0);
    estimatedDownloadTimeEdit = new ElaLineEdit(middleWidget);
    estimatedDownloadTimeEdit->setReadOnly(true);
    middleLayout->addWidget(estimatedDownloadTimeEdit, 2, 1);

    middleLayout->addWidget(new QLabel("Estimated finish time:"), 3, 0);
    estimatedFinishTimeEdit = new QDateTimeEdit(middleWidget);
    estimatedFinishTimeEdit->setReadOnly(true);
    estimatedFinishTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    middleLayout->addWidget(estimatedFinishTimeEdit, 3, 1);

    middleLayout->addWidget(new QLabel("Est. finish time (this target):"), 4,
                            0);
    estFinishTimeThisTargetEdit = new QDateTimeEdit(middleWidget);
    estFinishTimeThisTargetEdit->setReadOnly(true);
    estFinishTimeThisTargetEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    middleLayout->addWidget(estFinishTimeThisTargetEdit, 4, 1);
}

void T_SimpleSequencerPage::createBottomSection() {
    bottomWidget = new QWidget(this);
    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomWidget);

    targetTable = new ElaTableView(bottomWidget);
    model = new QStandardItemModel(1, 8, this);
    model->setHorizontalHeaderLabels({"Enabled", "Progress", "Total #", "Time",
                                      "Type", "Filter", "Binning", "Dither"});

    QStandardItem *enabledItem = new QStandardItem();
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
    QBarSet *set = new QBarSet("RA");
    *set << 90 << 60 << 30 << 0;

    QBarSeries *series = new QBarSeries();
    series->append(set);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Target");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setBackgroundVisible(false);
    chart->legend()->hide();

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 90);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(0, 24);
    axisX->setTickCount(9);
    axisX->setLabelFormat("%.0f");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setFixedHeight(200);
}

void T_SimpleSequencerPage::createControlButtons() {
    QWidget *buttonWidget = new QWidget(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);

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

    connect(backButton, &QPushButton::clicked, this, &T_SimpleSequencerPage::onBackButtonClicked);
    connect(addButton, &QPushButton::clicked, this, &T_SimpleSequencerPage::onAddButtonClicked);
    connect(deleteButton, &QPushButton::clicked, this, &T_SimpleSequencerPage::onDeleteButtonClicked);
    connect(resetButton, &QPushButton::clicked, this, &T_SimpleSequencerPage::onResetButtonClicked);
    connect(moveUpButton, &QPushButton::clicked, this, &T_SimpleSequencerPage::onMoveUpButtonClicked);
    connect(moveDownButton, &QPushButton::clicked, this, &T_SimpleSequencerPage::onMoveDownButtonClicked);
    connect(startButton, &QPushButton::clicked, this, &T_SimpleSequencerPage::onStartButtonClicked);
}

void T_SimpleSequencerPage::applyStyles() {}

void T_SimpleSequencerPage::onBackButtonClicked() {
    QMessageBox::information(this, "Back", "Back button clicked");
}

void T_SimpleSequencerPage::onAddButtonClicked() {
    int rowCount = model->rowCount();
    model->insertRow(rowCount);

    QStandardItem *enabledItem = new QStandardItem();
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
    QItemSelectionModel *selectionModel = targetTable->selectionModel();
    QModelIndexList selectedRows = selectionModel->selectedRows();

    for (const QModelIndex &index : selectedRows) {
        model->removeRow(index.row());
    }
}

void T_SimpleSequencerPage::onResetButtonClicked() {
    model->removeRows(0, model->rowCount());
    onAddButtonClicked(); // 添加一行默认数据
}

void T_SimpleSequencerPage::onMoveUpButtonClicked() {
    QItemSelectionModel *selectionModel = targetTable->selectionModel();
    QModelIndexList selectedRows = selectionModel->selectedRows();

    if (selectedRows.isEmpty() || selectedRows.first().row() == 0) {
        return;
    }

    int currentRow = selectedRows.first().row();
    model->insertRow(currentRow - 1, model->takeRow(currentRow));
    selectionModel->select(model->index(currentRow - 1, 0), QItemSelectionModel::Select);
}

void T_SimpleSequencerPage::onMoveDownButtonClicked() {
    QItemSelectionModel *selectionModel = targetTable->selectionModel();
    QModelIndexList selectedRows = selectionModel->selectedRows();

    if (selectedRows.isEmpty() || selectedRows.first().row() == model->rowCount() - 1) {
        return;
    }

    int currentRow = selectedRows.first().row();
    model->insertRow(currentRow + 1, model->takeRow(currentRow));
    selectionModel->select(model->index(currentRow + 1, 0), QItemSelectionModel::Select);
}

void T_SimpleSequencerPage::onStartButtonClicked() {
    QMessageBox::information(this, "Start", "Start button clicked");
}