#include "DoorsDialog.h"
#include "ui_DoorsDialog.h"

DoorsDialog::DoorsDialog(const QSslConfiguration* sslConfig, QWidget* parent) : QDialog(parent), ui(new Ui::DoorsDialog), _sslConfig(sslConfig)
{
    ui->setupUi(this);
}

DoorsDialog::~DoorsDialog() { delete ui; }
