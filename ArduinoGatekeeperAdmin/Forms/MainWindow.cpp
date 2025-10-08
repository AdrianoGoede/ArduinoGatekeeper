#include "MainWindow.h"
#include "Forms/ui_MainWindow.h"
//#include "Connection.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), _gatekeeperModel(new GatekeeperModel(this))
{
    ui->setupUi(this);
    connect(_gatekeeperModel, &GatekeeperModel::connectionStatusChanged, this, &MainWindow::handleConnectionStatusChange);
    connect(_gatekeeperModel, &GatekeeperModel::metricsUpdated, this, &MainWindow::handleModelMetricsChange);

    //ConnectionSettings connectionSettings;
    //Connection connectionForm(this, &connectionSettings);
    //if (connectionForm.exec() == QDialog::DialogCode::Rejected) exit(0);

    _gatekeeperModel->connectToBroker("192.168.1.11", 1883);
}

MainWindow::~MainWindow() { delete ui;}

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
}
