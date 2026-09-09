#include "DashboardDialog.h"
#include "ui_DashboardDialog.h"

DashboardDialog::DashboardDialog(const QSslConfiguration* sslConfig, QWidget* parent) : QDialog(parent), ui(new Ui::DashboardDialog), _sslConfig(sslConfig)
{
    ui->setupUi(this);
}

DashboardDialog::~DashboardDialog() { delete ui; }
