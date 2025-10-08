#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../Models/GatekeeperModel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

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
};
#endif // MAINWINDOW_H
