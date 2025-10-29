#include "Connection.h"
#include "Forms/ui_Connection.h"
#include <QMessageBox>

Connection::Connection(QWidget *parent) : QDialog(parent), ui(new Ui::Connection)
{
    ui->setupUi(this);
    connect(ui->connectionFormButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->connectionFormButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

Connection::~Connection() { delete ui; }

void Connection::accept()
{
    emit connectToBroker(
        ui->leAddress->text(),
        (qint16)ui->sbPort->value(),
        ui->leUsername->text(),
        ui->lePassword->text()
    );
    QDialog::accept();
}
