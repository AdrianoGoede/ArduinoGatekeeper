#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPieSeries>
#include <QLineSeries>
#include <QValueAxis>
#include <QDateTimeAxis>
#include "Connection.h"
#include "UserManager.h"
#include "LogExplorer.h"
#include "ClientExplorer.h"
#include "../Models/GatekeeperModel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

struct GrantedDeniedRatioChart {
    QPieSlice* GrantedSlice = nullptr;
    QPieSlice* DeniedSlice = nullptr;
};

struct TimeIntervalActivityChart {
    QLineSeries* GrantedSeries = nullptr;
    QLineSeries* DeniedSeries = nullptr;
    QDateTimeAxis* axisX = nullptr;
    QValueAxis* axisY = nullptr;
    uint32_t grantedCount = 0, deniedCount = 0;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleConnectionStatusChange(QMqttClient::ClientState state, QMqttClient::ClientError error);
    void handleModelMetricsChange(uint32_t connectedDevicesCount, uint32_t granted, uint32_t denied);
    void handleNewLogEntry(const LogEntry& entry);
    void handleActivityChartTimerTimeout();
    void handleConnectionSetupError(const QString& message);

private:
    Ui::MainWindow* ui;
    GatekeeperModel* _gatekeeperModel = nullptr;
    Connection* _connectionManager = nullptr;
    UserManager* _userManager = nullptr;
    LogExplorer* _logExplorer = nullptr;
    ClientExplorer* _clientExplorer = nullptr;
    GrantedDeniedRatioChart _grantedDeniedRationChart;
    TimeIntervalActivityChart _timeIntervalActivityChart;
    void setAccessStatusChart();
    void setTimeIntervalActivityChart();
    void adjustTimeIntervalActivityChart(const QDateTime& timestamp);
};
#endif // MAINWINDOW_H
