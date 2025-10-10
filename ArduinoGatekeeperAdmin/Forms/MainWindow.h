#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPieSeries>
#include <QLineSeries>
#include <QBarCategoryAxis>
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
    QLineSeries* Series = nullptr;
    QBarCategoryAxis* axisX = nullptr;
    QBarCategoryAxis* axisY = nullptr;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleConnectionStatusChange(bool isConnected);
    void handleModelMetricsChange(uint32_t connectedDevicesCount, uint32_t granted, uint32_t denied);

private:
    Ui::MainWindow* ui;
    GatekeeperModel* _gatekeeperModel = nullptr;
    GrantedDeniedRatioChart _grantedDeniedRationChart;
    TimeIntervalActivityChart _timeIntervalActivityChart;
    void setAccessStatusChart();
    void setTimeIntervalActivityChart();
};
#endif // MAINWINDOW_H
