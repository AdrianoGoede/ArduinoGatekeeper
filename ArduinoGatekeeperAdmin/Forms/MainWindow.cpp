#include "MainWindow.h"
#include "Forms/ui_MainWindow.h"
#include "../Config/config.h"
#include <QMessageBox>
#include <QChartView>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), _gatekeeperModel(new GatekeeperModel(this)), _connectionManager(new Connection(this)), _logExplorer(new LogExplorer(this)), _clientExplorer(new ClientExplorer(this))
{
    ui->setupUi(this);
    ui->metricsLayout->setColumnStretch(0, 2);
    this->setAccessStatusChart();
    this->setTimeIntervalActivityChart();

    QTimer* activityByTimeIntervalTimer = new QTimer(this);
    connect(activityByTimeIntervalTimer, &QTimer::timeout, this, &MainWindow::handleActivityChartTimerTimeout);
    activityByTimeIntervalTimer->setInterval(ACTIVITY_CHART_TIME_INTERVAL);
    activityByTimeIntervalTimer->start();

    connect(_gatekeeperModel, &GatekeeperModel::clientStateChanged, this, &MainWindow::handleConnectionStatusChange);
    connect(_gatekeeperModel, &GatekeeperModel::metricsUpdated, this, &MainWindow::handleModelMetricsChange);
    connect(_gatekeeperModel, &GatekeeperModel::newLogEntry, this, &MainWindow::handleNewLogEntry);
    connect(_gatekeeperModel, &GatekeeperModel::newLogEntry, _logExplorer, &LogExplorer::addLogEntry);
    connect(_gatekeeperModel, &GatekeeperModel::newDeviceStatusEntry, _clientExplorer, &ClientExplorer::addClientEntry);
    connect(_connectionManager, &Connection::connectToBroker, _gatekeeperModel, &GatekeeperModel::connectToBroker);
    connect(ui->pbManageBrokerConnection, &QAbstractButton::clicked, _connectionManager, &QWidget::show);
    connect(ui->pbLogExplorer, &QAbstractButton::clicked, _logExplorer, &QWidget::show);
    connect(ui->pbClientsExplorer, &QAbstractButton::clicked, _clientExplorer, &QWidget::show);

    if (_connectionManager->exec() == QDialog::DialogCode::Rejected) exit(0);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::handleConnectionStatusChange(QMqttClient::ClientState state, QMqttClient::ClientError error) {
    switch (state) {
        case QMqttClient::ClientState::Connected: {
            ui->centralwidget->setEnabled(true);
            ui->statusbar->showMessage(
                QString("Connected to MQTT broker at %1:%2").arg(_gatekeeperModel->getBrokerAddress()).arg(_gatekeeperModel->getBrokerPort())
            );
        }; break;
        case QMqttClient::ClientState::Connecting: {
            ui->centralwidget->setEnabled(false);
            ui->statusbar->showMessage("Connecting to broker...");
        }; break;
        case QMqttClient::ClientState::Disconnected: {
            ui->centralwidget->setEnabled(false);
            QString message = "Connection error: ";
            switch (error) {
                case QMqttClient::ClientError::InvalidProtocolVersion: message += "Invalid protocol version"; break;
                case QMqttClient::ClientError::IdRejected: message += "ID rejected"; break;
                case QMqttClient::ClientError::ServerUnavailable: message += "Service unavailable"; break;
                case QMqttClient::ClientError::BadUsernameOrPassword: message += "Incorrect credentials"; break;
                case QMqttClient::ClientError::NotAuthorized: message += "Not authorized"; break;
                case QMqttClient::ClientError::TransportInvalid: message += "Transport invalid"; break;
                case QMqttClient::ClientError::ProtocolViolation: message += "Protocol violation"; break;
                default: message += "Unknown";
            }
            ui->statusbar->showMessage(message);
            _connectionManager->open();
        }; break;
    }
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

void MainWindow::handleNewLogEntry(const LogEntry& entry)
{
    _timeIntervalActivityChart.grantedCount += (entry.accessGranted ? 1 : 0);
    _timeIntervalActivityChart.deniedCount += (entry.accessGranted ? 0 : 1);
}

void MainWindow::handleActivityChartTimerTimeout()
{
    QDateTime timestamp = QDateTime::currentDateTime();
    qreal msSinceEpoch = (qreal)timestamp.toMSecsSinceEpoch();
    _timeIntervalActivityChart.GrantedSeries->append(msSinceEpoch, _timeIntervalActivityChart.grantedCount);
    _timeIntervalActivityChart.DeniedSeries->append(msSinceEpoch, _timeIntervalActivityChart.deniedCount);
    _timeIntervalActivityChart.grantedCount = _timeIntervalActivityChart.deniedCount = 0;
    adjustTimeIntervalActivityChart(timestamp);
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
    chart->setTitle("Granted/Denied Ratio");
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
    _timeIntervalActivityChart.GrantedSeries = new QLineSeries(this);
    _timeIntervalActivityChart.GrantedSeries->setName("Granted");
    _timeIntervalActivityChart.GrantedSeries->setColor(QColor::fromRgb(0, 255, 0));

    _timeIntervalActivityChart.DeniedSeries = new QLineSeries(this);
    _timeIntervalActivityChart.DeniedSeries->setName("Denied");
    _timeIntervalActivityChart.DeniedSeries->setColor(QColor::fromRgb(255, 0, 0));

    QChart* chart = new QChart();
    chart->legend()->setVisible(true);
    chart->addSeries(_timeIntervalActivityChart.GrantedSeries);
    chart->addSeries(_timeIntervalActivityChart.DeniedSeries);
    chart->setTitle("Activity");

    _timeIntervalActivityChart.axisX = new QDateTimeAxis(chart);
    _timeIntervalActivityChart.axisX->setTitleText("Timestamp");
    _timeIntervalActivityChart.axisX->setFormat("hh:mm:ss");
    chart->addAxis(_timeIntervalActivityChart.axisX, Qt::AlignBottom);
    _timeIntervalActivityChart.GrantedSeries->attachAxis(_timeIntervalActivityChart.axisX);
    _timeIntervalActivityChart.DeniedSeries->attachAxis(_timeIntervalActivityChart.axisX);

    _timeIntervalActivityChart.axisY = new QValueAxis(chart);
    chart->addAxis(_timeIntervalActivityChart.axisY, Qt::AlignLeft);
    _timeIntervalActivityChart.GrantedSeries->attachAxis(_timeIntervalActivityChart.axisY);
    _timeIntervalActivityChart.DeniedSeries->attachAxis(_timeIntervalActivityChart.axisY);

    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    ui->metricsLayout->addWidget(chartView, 0, 2);
    ui->metricsLayout->setColumnStretch(2, 6);
    adjustTimeIntervalActivityChart(QDateTime::currentDateTime());
}

void MainWindow::adjustTimeIntervalActivityChart(const QDateTime& timestamp)
{
    //qint64 windowMsOffset = (ACTIVITY_CHART_TIME_INTERVAL * 30);
    if (!_timeIntervalActivityChart.axisX || !_timeIntervalActivityChart.axisY) return;

    qint64 currentMs = timestamp.toMSecsSinceEpoch();
    qint64 minVisibleMs = (currentMs - ACTIVITY_CHART_TIME_WINDOW);

    QDateTime newMax = QDateTime::fromMSecsSinceEpoch(currentMs);
    QDateTime newMin = QDateTime::fromMSecsSinceEpoch(minVisibleMs);

    qreal maxY = 0;
    auto findMaxY = [&](QLineSeries* series) {
        for (const QPointF& point : series->points())
            if (point.x() >= minVisibleMs && point.y() > maxY)
                maxY = point.y();
    };

    findMaxY(_timeIntervalActivityChart.GrantedSeries);
    findMaxY(_timeIntervalActivityChart.DeniedSeries);

    _timeIntervalActivityChart.axisY->setRange(0, (maxY * 1.1 + 1));
    _timeIntervalActivityChart.axisX->setRange(newMin, newMax);
}
