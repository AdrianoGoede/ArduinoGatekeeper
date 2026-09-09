#include "UsersDialog.h"
#include "ui_UsersDialog.h"

UsersDialog::UsersDialog(const QSslConfiguration* sslConfig, QWidget* parent) : QDialog(parent), ui(new Ui::UsersDialog), _sslConfig(sslConfig)
{
    ui->setupUi(this);
}

UsersDialog::~UsersDialog() { delete ui; }
