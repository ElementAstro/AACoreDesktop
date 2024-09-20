#include "T_TargetSearch.h"
#include <QHBoxLayout>
#include <QListWidget>
#include <QRandomGenerator>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QtCharts>
#include "ElaListView.h"
#include "ElaScrollArea.h"
#include "ElaText.h"
#include "T_BasePage.h"


T_TargetSearchPage::T_TargetSearchPage(QWidget *parent) : T_BasePage(parent) {
    setupUI();
    styleLeftPanel();
    styleObjectDetails();
}

T_TargetSearchPage::~T_TargetSearchPage() {}

void T_TargetSearchPage::setupUI() {
    mainLayout = new QVBoxLayout(this);
    contentLayout = new QHBoxLayout();

    createLeftPanel();
    createRightPanel();

    contentLayout->addWidget(leftPanel);
    contentLayout->addWidget(rightPanel, 1);

    mainLayout->addLayout(contentLayout);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("天体搜索面板");
    QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addLayout(mainLayout);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

void T_TargetSearchPage::createLeftPanel() {
    leftPanel = new QWidget(this);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);

    observationCombo = new ElaComboBox(leftPanel);
    observationCombo->addItem("Observation");

    dateEdit = new ElaPlainTextEdit(leftPanel);
    dateEdit->setPlainText(QDate::currentDate().toString("yyyy-MM-dd"));
    dateEdit->setMaximumHeight(30);

    altitudeCombo = new ElaComboBox(leftPanel);
    altitudeCombo->addItem("Any");

    apparentSizeCombo = new ElaComboBox(leftPanel);
    apparentSizeCombo->addItem("Apparent size");

    fromCombo = new ElaComboBox(leftPanel);
    fromCombo->addItem("From");

    throughCombo = new ElaComboBox(leftPanel);
    throughCombo->addItem("Through");

    moonCombo = new ElaComboBox(leftPanel);
    moonCombo->addItem("Moon");

    minDistanceSpinBox = new ElaSpinBox(leftPanel);
    minDistanceSpinBox->setPrefix("Minimum distance: ");
    minDistanceSpinBox->setSuffix("°");

    objectTypeCombo = new ElaComboBox(leftPanel);
    objectTypeCombo->addItem("Object type");

    constellationCombo = new ElaComboBox(leftPanel);
    constellationCombo->addItem("Constellation");

    coordinatesCombo = new ElaComboBox(leftPanel);
    coordinatesCombo->addItem("Coordinates");

    surfaceBrightnessCombo = new ElaComboBox(leftPanel);
    surfaceBrightnessCombo->addItem("Surface brightness");

    apparentMagnitudeCombo = new ElaComboBox(leftPanel);
    apparentMagnitudeCombo->addItem("Apparent magnitude");

    orderByCombo = new ElaComboBox(leftPanel);
    orderByCombo->addItem("Size");

    descendingCombo = new ElaComboBox(leftPanel);
    descendingCombo->addItem("Descending");

    itemsPerPageSpinBox = new ElaSpinBox(leftPanel);
    itemsPerPageSpinBox->setValue(50);
    itemsPerPageSpinBox->setPrefix("Items per page: ");

    searchButton = new ElaPushButton("Search", leftPanel);
    connect(searchButton, &ElaPushButton::clicked, this,
            &T_TargetSearchPage::onSearchClicked);

    leftLayout->addWidget(observationCombo);
    leftLayout->addWidget(dateEdit);
    leftLayout->addWidget(altitudeCombo);
    leftLayout->addWidget(apparentSizeCombo);
    leftLayout->addWidget(fromCombo);
    leftLayout->addWidget(throughCombo);
    leftLayout->addWidget(moonCombo);
    leftLayout->addWidget(minDistanceSpinBox);
    leftLayout->addWidget(objectTypeCombo);
    leftLayout->addWidget(constellationCombo);
    leftLayout->addWidget(coordinatesCombo);
    leftLayout->addWidget(surfaceBrightnessCombo);
    leftLayout->addWidget(apparentMagnitudeCombo);
    leftLayout->addWidget(orderByCombo);
    leftLayout->addWidget(descendingCombo);
    leftLayout->addWidget(itemsPerPageSpinBox);
    leftLayout->addWidget(searchButton);
    leftLayout->addStretch();

    leftPanel->setMaximumWidth(250);
}

void T_TargetSearchPage::createRightPanel() {
    rightPanel = new QWidget(this);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    objectTabWidget = new ElaTabWidget(rightPanel);
    createObjectList();
    createObjectDetails();

    objectTabWidget->addTab(objectListWidget, "Object List");
    objectTabWidget->addTab(objectDetailsWidget, "Object Details");

    rightLayout->addWidget(objectTabWidget);
}

void T_TargetSearchPage::createObjectList() {
    objectListWidget = new QListWidget(this);
    objectListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    // connect(objectListWidget, &QListWidget::itemClicked, this,
    //        &T_TargetSearchPage::onObjectSelected);
}

void T_TargetSearchPage::createObjectDetails() {
    objectDetailsWidget = new QWidget(this);
    QVBoxLayout *detailsLayout = new QVBoxLayout(objectDetailsWidget);

    objectNameLabel = new ElaText("Object Name", objectDetailsWidget);
    raLabel = new ElaText("RA: ", objectDetailsWidget);
    decLabel = new ElaText("Dec: ", objectDetailsWidget);
    typeLabel = new ElaText("Type: ", objectDetailsWidget);
    constellationLabel = new ElaText("Constellation: ", objectDetailsWidget);

    altitudeSlider = new ElaSlider(Qt::Horizontal, objectDetailsWidget);
    altitudeSlider->setRange(0, 90);
    altitudeSlider->setValue(45);

    illuminationProgress = new ElaProgressBar(objectDetailsWidget);
    illuminationProgress->setRange(0, 100);
    illuminationProgress->setValue(50);

    createVisibilityChart();
    createMoonPhaseInfo();

    detailsLayout->addWidget(objectNameLabel);
    detailsLayout->addWidget(raLabel);
    detailsLayout->addWidget(decLabel);
    detailsLayout->addWidget(typeLabel);
    detailsLayout->addWidget(constellationLabel);
    detailsLayout->addWidget(new ElaText("Altitude:"));
    detailsLayout->addWidget(altitudeSlider);
    detailsLayout->addWidget(new ElaText("Illumination:"));
    detailsLayout->addWidget(illuminationProgress);
    detailsLayout->addWidget(visibilityChartView);
    detailsLayout->addWidget(moonPhaseWidget);
}

void T_TargetSearchPage::createVisibilityChart() {
    QChart *chart = createVisibilityQChart();
    visibilityChartView = new QChartView(chart);
    visibilityChartView->setRenderHint(QPainter::Antialiasing);
    visibilityChartView->setMinimumHeight(200);
}

QChart *T_TargetSearchPage::createVisibilityQChart() {
    QChart *chart = new QChart();
    chart->setTitle("Object Visibility");

    QLineSeries *series = new QLineSeries();

    // Generate some random data for the chart
    for (int i = 0; i < 24; ++i) {
        series->append(i, QRandomGenerator::global()->bounded(90));
    }

    chart->addSeries(series);

    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, 23);
    axisX->setTickCount(13);
    axisX->setLabelFormat("%d");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 90);
    axisY->setTickCount(10);
    axisY->setLabelFormat("%d");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    return chart;
}

void T_TargetSearchPage::createMoonPhaseInfo() {
    moonPhaseWidget = new QWidget(objectDetailsWidget);
    QVBoxLayout *moonLayout = new QVBoxLayout(moonPhaseWidget);

    moonPhaseLabel = new ElaText("Moon Phase: Waxing Gibbous", moonPhaseWidget);
    moonriseLabel = new ElaText("Moonrise: 14:36", moonPhaseWidget);
    moonsetLabel = new ElaText("Moonset: 03:08", moonPhaseWidget);
    illuminationLabel = new ElaText("Illumination: 1.21%", moonPhaseWidget);

    moonLayout->addWidget(moonPhaseLabel);
    moonLayout->addWidget(moonriseLabel);
    moonLayout->addWidget(moonsetLabel);
    moonLayout->addWidget(illuminationLabel);
}

void T_TargetSearchPage::onObjectSelected(int index) {
    updateObjectDetails(index);
    objectTabWidget->setCurrentIndex(1);  // Switch to Object Details tab
}

// ... (previous code remains the same)

void T_TargetSearchPage::updateObjectDetails(int index) {
    objectNameLabel->setText(QString("Object %1").arg(index + 1));
    raLabel->setText(QString("RA: 14:40:%1").arg(index * 10));
    decLabel->setText(QString("Dec: %1° 34' 00\"").arg(60 + index));
    typeLabel->setText(QString("Type: OPNCL"));
    constellationLabel->setText(QString("Constellation: UMA"));

    altitudeSlider->setValue(QRandomGenerator::global()->bounded(90));
    illuminationProgress->setValue(QRandomGenerator::global()->bounded(100));

    // Update visibility chart
    QChart *newChart = createVisibilityQChart();
    visibilityChartView->setChart(newChart);

    // Update moon phase info
    updateMoonPhaseInfo();
}

void T_TargetSearchPage::updateMoonPhaseInfo() {
    static const QStringList phases = {
        "New Moon",  "Waxing Crescent", "First Quarter", "Waxing Gibbous",
        "Full Moon", "Waning Gibbous",  "Last Quarter",  "Waning Crescent"};
    int randomPhase = QRandomGenerator::global()->bounded(phases.size());

    moonPhaseLabel->setText("Moon Phase: " + phases[randomPhase]);
    moonriseLabel->setText(
        QString("Moonrise: %1:%2")
            .arg(QRandomGenerator::global()->bounded(24), 2, 10, QChar('0'))
            .arg(QRandomGenerator::global()->bounded(60), 2, 10, QChar('0')));
    moonsetLabel->setText(
        QString("Moonset: %1:%2")
            .arg(QRandomGenerator::global()->bounded(24), 2, 10, QChar('0'))
            .arg(QRandomGenerator::global()->bounded(60), 2, 10, QChar('0')));
    illuminationLabel->setText(
        QString("Illumination: %1%")
            .arg(QRandomGenerator::global()->bounded(100)));
}

void T_TargetSearchPage::onSearchClicked() {
    objectListWidget->clear();

    int objectCount = itemsPerPageSpinBox->value();
    for (int i = 0; i < objectCount; ++i) {
        QListWidgetItem *item = new QListWidgetItem(objectListWidget);

        QWidget *itemWidget = new QWidget(objectListWidget);
        QVBoxLayout *itemLayout = new QVBoxLayout(itemWidget);

        QLabel *nameLabel =
            new QLabel(QString("Object %1").arg(i + 1), itemWidget);
        nameLabel->setStyleSheet("font-weight: bold; color: #4a4a4a;");

        QLabel *detailsLabel = new QLabel(
            QString("RA: %1:%2:%3, Dec: %4° %5' %6\"")
                .arg(QRandomGenerator::global()->bounded(24), 2, 10, QChar('0'))
                .arg(QRandomGenerator::global()->bounded(60), 2, 10, QChar('0'))
                .arg(QRandomGenerator::global()->bounded(60), 2, 10, QChar('0'))
                .arg(QRandomGenerator::global()->bounded(90), 2, 10, QChar('0'))
                .arg(QRandomGenerator::global()->bounded(60), 2, 10, QChar('0'))
                .arg(QRandomGenerator::global()->bounded(60), 2, 10,
                     QChar('0')),
            itemWidget);
        detailsLabel->setStyleSheet("color: #6a6a6a;");

        itemLayout->addWidget(nameLabel);
        itemLayout->addWidget(detailsLabel);

        QHBoxLayout *buttonLayout = new QHBoxLayout();
        ElaPushButton *addToSequenceButton =
            new ElaPushButton("Add target to sequence", itemWidget);
        ElaPushButton *setFramingButton =
            new ElaPushButton("Set for framing assistant", itemWidget);
        ElaPushButton *slewButton = new ElaPushButton("Slew", itemWidget);

        addToSequenceButton->setStyleSheet(
            "background-color: #4CAF50; color: white;");
        setFramingButton->setStyleSheet(
            "background-color: #2196F3; color: white;");
        slewButton->setStyleSheet("background-color: #FF9800; color: white;");

        buttonLayout->addWidget(addToSequenceButton);
        buttonLayout->addWidget(setFramingButton);
        buttonLayout->addWidget(slewButton);

        itemLayout->addLayout(buttonLayout);

        item->setSizeHint(itemWidget->sizeHint());
        objectListWidget->setItemWidget(item, itemWidget);
    }

    objectTabWidget->setCurrentIndex(0);
}

// Add this method to improve the visual appeal of the left panel
void T_TargetSearchPage::styleLeftPanel() {}

// Add this method to create a more visually appealing object details view
void T_TargetSearchPage::styleObjectDetails() {}
