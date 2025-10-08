#include "Connection.h"
#include "Forms/ui_Connection.h"

Connection::Connection(QWidget *parent, ConnectionSettings* settings) : QDialog(parent), _settings(settings), ui(new Ui::Connection)
{
    ui->setupUi(this);
    connect(ui->connectionFormButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->connectionFormButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

Connection::~Connection() { delete ui; }

void Connection::accept()
{
    _settings->Address = ui->leAddress->text();
    _settings->port = (uint16_t)ui->sbPort->value();
    _settings->username = ui->leUsername->text();
    _settings->password = ui->lePassword->text();
}
