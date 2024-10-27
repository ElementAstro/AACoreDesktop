#include "T_SimpleSequencer.h"

#include <QBarSeries>
#include <QHeaderView>
#include <QMessageBox>

#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "ElaSpinBox.h"
#include "ElaTableView.h"
#include "ElaText.h"
#include "ElaToggleSwitch.h"

namespace {
const int TextPixelSize = 15;
const int LayoutSpacing = 8;
const int LayoutMargin = 10;
const int ChartHeight = 180;
const int ModelColumnCount = 8;
}  // namespace

T_SimpleSequencerPage::T_SimpleSequencerPage(QWidget *parent)
    : T_BasePage(parent), mainLayout(nullptr), model(nullptr) {
    setupUI();
    applyStyles();
}

void T_SimpleSequencerPage::setupUI() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(LayoutSpacing);
    mainLayout->setContentsMargins(LayoutMargin, LayoutMargin, LayoutMargin,
                                   LayoutMargin);

    createTopSection();
    createMiddleSection();
    createBottomSection();
    createControlButtons();

    mainLayout->addWidget(topWidget);
    mainLayout->addWidget(middleWidget);
    mainLayout->addWidget(bottomWidget);

    setLayout(mainLayout);
}

void T_SimpleSequencerPage::createTopSection() {
    topWidget = new QWidget(this);
    auto topLayout = new QHBoxLayout(topWidget);

    // Left side switches
    auto startOptionsWidget = new QWidget(topWidget);
    auto startOptionsLayout = new QVBoxLayout(startOptionsWidget);
    auto targetSetStartText = new ElaText("Start Options", startOptionsWidget);
    targetSetStartText->setTextPixelSize(TextPixelSize);

    auto switchesWidget = new QWidget(startOptionsWidget);
    auto switchesLayout = new QHBoxLayout(switchesWidget);

    coolCameraSwitch = new ElaToggleSwitch(switchesWidget);
    unparkMountSwitch = new ElaToggleSwitch(switchesWidget);
    meridianFlipSwitch = new ElaToggleSwitch(switchesWidget);

    switchesLayout->addWidget(new ElaText("Cool Camera", switchesWidget));
    switchesLayout->addWidget(coolCameraSwitch);
    switchesLayout->addWidget(new ElaText("Unpark Mount", switchesWidget));
    switchesLayout->addWidget(unparkMountSwitch);
    switchesLayout->addWidget(new ElaText("Meridian Flip", switchesWidget));
    switchesLayout->addWidget(meridianFlipSwitch);

    startOptionsLayout->addWidget(targetSetStartText);
    startOptionsLayout->addWidget(switchesWidget);

    // Right side switches
    auto endOptionsWidget = new QWidget(topWidget);
    auto endOptionsLayout = new QVBoxLayout(endOptionsWidget);
    auto targetSetEndText = new ElaText("End Options", endOptionsWidget);
    targetSetEndText->setTextPixelSize(TextPixelSize);

    auto endSwitchesWidget = new QWidget(endOptionsWidget);
    auto endSwitchesLayout = new QHBoxLayout(endSwitchesWidget);

    warmCameraSwitch = new ElaToggleSwitch(endSwitchesWidget);
    parkMountSwitch = new ElaToggleSwitch(endSwitchesWidget);

    endSwitchesLayout->addWidget(new ElaText("Warm Camera", endSwitchesWidget));
    endSwitchesLayout->addWidget(warmCameraSwitch);
    endSwitchesLayout->addWidget(new ElaText("Park Mount", endSwitchesWidget));
    endSwitchesLayout->addWidget(parkMountSwitch);

    endOptionsLayout->addWidget(targetSetEndText);
    endOptionsLayout->addWidget(endSwitchesWidget);

    topLayout->addWidget(startOptionsWidget);
    topLayout->addStretch();
    topLayout->addWidget(endOptionsWidget);
}

void T_SimpleSequencerPage::createMiddleSection() {
    middleWidget = new QWidget(this);
    auto middleLayout = new QGridLayout(middleWidget);

    delayStartSpinBox = new ElaSpinBox(middleWidget);
    delayStartSpinBox->setSuffix(" s");
    sequenceModeCombo = new ElaComboBox(middleWidget);
    sequenceModeCombo->addItem("One after another");

    middleLayout->addWidget(new ElaText("Delay Start:", middleWidget), 0, 0);
    middleLayout->addWidget(delayStartSpinBox, 0, 1);
    middleLayout->addWidget(new ElaText("Sequence Mode:", middleWidget), 1, 0);
    middleLayout->addWidget(sequenceModeCombo, 1, 1);

    estimatedDownloadTimeEdit = new ElaLineEdit(middleWidget);
    estimatedFinishTimeEdit = new QDateTimeEdit(middleWidget);
    estFinishTimeThisTargetEdit = new QDateTimeEdit(middleWidget);
    estimatedFinishTimeEdit->setReadOnly(true);
    estFinishTimeThisTargetEdit->setReadOnly(true);

    middleLayout->addWidget(new ElaText("Est. Download Time:", middleWidget), 2,
                            0);
    middleLayout->addWidget(estimatedDownloadTimeEdit, 2, 1);
    middleLayout->addWidget(new ElaText("Est. Finish Time:", middleWidget), 3,
                            0);
    middleLayout->addWidget(estimatedFinishTimeEdit, 3, 1);
    middleLayout->addWidget(new ElaText("Est. Finish (Target):", middleWidget),
                            4, 0);
    middleLayout->addWidget(estFinishTimeThisTargetEdit, 4, 1);
}

void T_SimpleSequencerPage::createBottomSection() {
    bottomWidget = new QWidget(this);
    auto bottomLayout = new QVBoxLayout(bottomWidget);

    targetTable = new ElaTableView(bottomWidget);
    model = new QStandardItemModel(1, ModelColumnCount, this);
    model->setHorizontalHeaderLabels({"Enabled", "Progress", "Total #", "Time",
                                      "Type", "Filter", "Binning", "Dither"});

    targetTable->setModel(model);
    bottomLayout->addWidget(targetTable);
    createChart();
}

void T_SimpleSequencerPage::createChart() {
    auto series = new QBarSeries();
    auto chart = new QChart();

    chart->addSeries(series);
    chart->legend()->hide();
    chartView = new QChartView(chart);
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

    // 连接按钮点击事件到槽函数
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

void T_SimpleSequencerPage::applyStyles() {
    // 设置样式
    setStyleSheet(R"(
        QWidget {
            font-size: 13px;
            color: #333;
        }
        QTableView {
            border: 1px solid #ddd;
        }
        QHeaderView::section {
            background-color: #f3f3f3;
            padding: 5px;
            border: 1px solid #ddd;
        }
        QPushButton {
            background-color: #0078d7;
            color: white;
            padding: 5px 15px;
            border-radius: 4px;
        }
        QPushButton:hover {
            background-color: #005a9e;
        }
    )");
}

// 槽函数实现
void T_SimpleSequencerPage::onBackButtonClicked() {
    QMessageBox::information(this, "Back", "Returning to the previous page");
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
    QMessageBox::information(this, "Start", "Sequence started");
}
