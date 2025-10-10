#include "MainWindow.h"
#include "Forms/ui_MainWindow.h"
#include <QMessageBox>
#include <QChartView>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), _gatekeeperModel(new GatekeeperModel(this))
{
    ui->setupUi(this);
    ui->metricsLayout->setColumnStretch(0, 2);
    this->setAccessStatusChart();
    this->setTimeIntervalActivityChart();

    connect(_gatekeeperModel, &GatekeeperModel::connectionStatusChanged, this, &MainWindow::handleConnectionStatusChange);
    connect(_gatekeeperModel, &GatekeeperModel::metricsUpdated, this, &MainWindow::handleModelMetricsChange);

    _gatekeeperModel->connectToBroker("", 1883);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::handleConnectionStatusChange(bool isConnected) {
    if (isConnected)
        ui->statusbar->showMessage(
            QString("Connected to MQTT broker at %1:%2").arg(_gatekeeperModel->getBrokerAddress()).arg(_gatekeeperModel->getBrokerPort())
        );
    else
        ui->statusbar->showMessage("Connection to broker lost!");
}

void MainWindow::handleModelMetricsChange(uint32_t connectedDevicesCount, uint32_t granted, uint32_t denied)
{
    ui->lcdNumConnectedGatekeepers->display((int)connectedDevicesCount);
    ui->lcdNumLogCount->display((int)(granted + denied));

    double total = std::max((double)1.0, (double)(granted + denied));
    _grantedDeniedRationChart.GrantedSlice->setValue(granted);
    _grantedDeniedRationChart.GrantedSlice->setLabel(QString("Granted (%1 %)").arg(QString::number((granted / total * 100), 'f', 1)));
    _grantedDeniedRationChart.DeniedSlice->setValue(denied);
    _grantedDeniedRationChart.DeniedSlice->setLabel(QString("Denied (%1 %)").arg(QString::number((denied / total * 100), 'f', 1)));
}

void MainWindow::setAccessStatusChart()
{
    QPieSeries* series = new QPieSeries(this);
    series->setLabelsPosition(QPieSlice::LabelInsideHorizontal);

    _grantedDeniedRationChart.GrantedSlice = series->append("Granted", 0);
    _grantedDeniedRationChart.GrantedSlice->setLabelVisible(true);
    _grantedDeniedRationChart.GrantedSlice->setColor(QColor::fromRgb(0, 255, 0));

    _grantedDeniedRationChart.DeniedSlice = series->append("Denied", 0);
    _grantedDeniedRationChart.DeniedSlice->setLabelVisible(true);
    _grantedDeniedRationChart.DeniedSlice->setColor(QColor::fromRgb(255, 0, 0));

    QChart* chart = new QChart();
    chart->setTitle("Granted/Denied Ration");
    chart->addSeries(series);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    ui->metricsLayout->addWidget(chartView, 0, 1);
    ui->metricsLayout->setColumnStretch(1, 4);
}

void MainWindow::setTimeIntervalActivityChart()
{
    _timeIntervalActivityChart.Series = new QLineSeries(this);

    QChart* chart = new QChart();
    chart->addSeries(_timeIntervalActivityChart.Series);
    chart->setTitle("Activity");

    _timeIntervalActivityChart.axisX = new QBarCategoryAxis(chart);
    _timeIntervalActivityChart.axisX->setTitleText("Timestamp");
    chart->addAxis(_timeIntervalActivityChart.axisX, Qt::AlignBottom);
    _timeIntervalActivityChart.Series->attachAxis(_timeIntervalActivityChart.axisX);

    _timeIntervalActivityChart.axisY = new QBarCategoryAxis(chart);
    chart->addAxis(_timeIntervalActivityChart.axisY, Qt::AlignLeft);
    _timeIntervalActivityChart.Series->attachAxis(_timeIntervalActivityChart.axisY);

    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    ui->metricsLayout->addWidget(chartView, 0, 2);
    ui->metricsLayout->setColumnStretch(2, 6);
}
