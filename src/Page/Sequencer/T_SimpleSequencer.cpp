// T_SimpleSequencerPage.cpp
#include "T_SimpleSequencer.h"

#include <QBarSeries>
#include <QBarSet>
#include <QFileDialog>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
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

    // Start Options
    auto *startOptionsWidget = new QWidget(topWidget);
    auto *startOptionsLayout = new QVBoxLayout(startOptionsWidget);
    auto *startText = new ElaText("Start Options", startOptionsWidget);
    startText->setTextPixelSize(TextPixelSize);

    auto *startSwitchesWidget = new QWidget(startOptionsWidget);
    auto *startSwitchesLayout = new QHBoxLayout(startSwitchesWidget);

    coolCameraSwitch = new ElaToggleSwitch(startSwitchesWidget);
    unparkMountSwitch = new ElaToggleSwitch(startSwitchesWidget);
    meridianFlipSwitch = new ElaToggleSwitch(startSwitchesWidget);

    startSwitchesLayout->addWidget(
        new ElaText("Cool Camera", startSwitchesWidget));
    startSwitchesLayout->addWidget(coolCameraSwitch);
    startSwitchesLayout->addWidget(
        new ElaText("Unpark Mount", startSwitchesWidget));
    startSwitchesLayout->addWidget(unparkMountSwitch);
    startSwitchesLayout->addWidget(
        new ElaText("Meridian Flip", startSwitchesWidget));
    startSwitchesLayout->addWidget(meridianFlipSwitch);

    startOptionsLayout->addWidget(startText);
    startOptionsLayout->addWidget(startSwitchesWidget);

    // End Options
    auto *endOptionsWidget = new QWidget(topWidget);
    auto *endOptionsLayout = new QVBoxLayout(endOptionsWidget);
    auto *endText = new ElaText("End Options", endOptionsWidget);
    endText->setTextPixelSize(TextPixelSize);

    auto *endSwitchesWidget = new QWidget(endOptionsWidget);
    auto *endSwitchesLayout = new QHBoxLayout(endSwitchesWidget);

    warmCameraSwitch = new ElaToggleSwitch(endSwitchesWidget);
    parkMountSwitch = new ElaToggleSwitch(endSwitchesWidget);

    endSwitchesLayout->addWidget(new ElaText("Warm Camera", endSwitchesWidget));
    endSwitchesLayout->addWidget(warmCameraSwitch);
    endSwitchesLayout->addWidget(new ElaText("Park Mount", endSwitchesWidget));
    endSwitchesLayout->addWidget(parkMountSwitch);

    endOptionsLayout->addWidget(endText);
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
    delayStartSpinBox->setRange(0, 3600);
    delayStartSpinBox->setValue(10);

    sequenceModeCombo = new ElaComboBox(middleWidget);
    sequenceModeCombo->addItem("One after another");
    sequenceModeCombo->addItem("Simultaneously");
    connect(sequenceModeCombo, &ElaComboBox::currentTextChanged, this,
            &T_SimpleSequencerPage::onSequenceModeChanged);

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
    model = new QStandardItemModel(0, ModelColumnCount, this);
    model->setHorizontalHeaderLabels({"Enabled", "Progress", "Total #", "Time",
                                      "Type", "Filter", "Binning", "Dither"});

    targetTable->setModel(model);
    targetTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    bottomLayout->addWidget(targetTable);
    createChart();
    bottomLayout->addWidget(chartView);

    connect(model, &QStandardItemModel::itemChanged, this,
            &T_SimpleSequencerPage::updateEstimatedTimes);
}

void T_SimpleSequencerPage::createChart() {
    auto *series = new QBarSeries();
    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Sequence Progress");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chartView = new QChartView(chart, this);
    chartView->setFixedHeight(ChartHeight);
    chartView->setRenderHint(QPainter::Antialiasing);
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

    // Connect buttons
    connect(backButton, &ElaPushButton::clicked, this,
            &T_SimpleSequencerPage::onBackButtonClicked);
    connect(addButton, &ElaPushButton::clicked, this,
            &T_SimpleSequencerPage::onAddButtonClicked);
    connect(deleteButton, &ElaPushButton::clicked, this,
            &T_SimpleSequencerPage::onDeleteButtonClicked);
    connect(resetButton, &ElaPushButton::clicked, this,
            &T_SimpleSequencerPage::onResetButtonClicked);
    connect(moveUpButton, &ElaPushButton::clicked, this,
            &T_SimpleSequencerPage::onMoveUpButtonClicked);
    connect(moveDownButton, &ElaPushButton::clicked, this,
            &T_SimpleSequencerPage::onMoveDownButtonClicked);
    connect(startButton, &ElaPushButton::clicked, this,
            &T_SimpleSequencerPage::onStartButtonClicked);
}

void T_SimpleSequencerPage::applyStyles() {
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
    model->setItem(rowCount, 5, new QStandardItem("None"));
    model->setItem(rowCount, 6, new QStandardItem("1x1"));
    model->setItem(rowCount, 7, new QStandardItem("OFF"));

    populateChart();
}

void T_SimpleSequencerPage::onDeleteButtonClicked() {
    auto selectionModel = targetTable->selectionModel();
    auto selectedRows = selectionModel->selectedRows();

    for (const auto &index : selectedRows) {
        model->removeRow(index.row());
    }

    populateChart();
}

void T_SimpleSequencerPage::onResetButtonClicked() {
    model->removeRows(0, model->rowCount());
    onAddButtonClicked();  // Add default row
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

    populateChart();
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

    populateChart();
}

void T_SimpleSequencerPage::onStartButtonClicked() {
    QMessageBox::information(this, "Start", "Sequence started");
}

void T_SimpleSequencerPage::onSequenceModeChanged(const QString &mode) {
    Q_UNUSED(mode);
    updateEstimatedTimes();
}

void T_SimpleSequencerPage::updateEstimatedTimes() {
    // Placeholder for actual time calculations
    estimatedDownloadTimeEdit->setText("5 min");
    estimatedFinishTimeEdit->setDateTime(
        QDateTime::currentDateTime().addSecs(300));
    estFinishTimeThisTargetEdit->setDateTime(
        QDateTime::currentDateTime().addSecs(300));
}

void T_SimpleSequencerPage::loadSequenceFromFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Load Error", "Failed to open file.");
        return;
    }

    QByteArray data = file.readAll();
    file.close();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();

    QJsonArray sequenceArray = obj["sequence"].toArray();
    for (const QJsonValue &value : sequenceArray) {
        QJsonObject seqObj = value.toObject();
        onAddButtonClicked();
        int row = model->rowCount() - 1;
        model->setData(model->index(row, 1), seqObj["progress"].toString());
        model->setData(model->index(row, 2), seqObj["total"].toString());
        model->setData(model->index(row, 3), seqObj["time"].toString());
        model->setData(model->index(row, 4), seqObj["type"].toString());
        model->setData(model->index(row, 5), seqObj["filter"].toString());
        model->setData(model->index(row, 6), seqObj["binning"].toString());
        model->setData(model->index(row, 7), seqObj["dither"].toString());
    }

    populateChart();
}

void T_SimpleSequencerPage::saveSequenceToFile(const QString &filePath) {
    QJsonObject obj;
    QJsonArray sequenceArray;
    for (int row = 0; row < model->rowCount(); ++row) {
        QJsonObject seqObj;
        seqObj["progress"] = model->item(row, 1)->text();
        seqObj["total"] = model->item(row, 2)->text();
        seqObj["time"] = model->item(row, 3)->text();
        seqObj["type"] = model->item(row, 4)->text();
        seqObj["filter"] = model->item(row, 5)->text();
        seqObj["binning"] = model->item(row, 6)->text();
        seqObj["dither"] = model->item(row, 7)->text();
        sequenceArray.append(seqObj);
    }
    obj["sequence"] = sequenceArray;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Save Error", "Failed to save file.");
        return;
    }

    file.write(QJsonDocument(obj).toJson());
    file.close();
    QMessageBox::information(this, "Save", "Sequence saved successfully.");
}

void T_SimpleSequencerPage::populateChart() {
    // Example chart population
    auto chart = chartView->chart();
    chart->removeAllSeries();

    QBarSeries *series = new QBarSeries();
    for (int row = 0; row < model->rowCount(); ++row) {
        QString progress = model->item(row, 1)->text().split(" / ").first();
        bool ok;
        double value = progress.toDouble(&ok);
        if (ok) {
            QBarSet *set = new QBarSet(model->item(row, 4)->text());
            set->append(value);
            series->append(set);
        }
    }
    chart->addSeries(series);
    chart->createDefaultAxes();
}