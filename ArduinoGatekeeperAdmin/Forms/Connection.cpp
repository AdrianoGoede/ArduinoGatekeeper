#include "Connection.h"
#include "Forms/ui_Connection.h"
#include "../Config/config.h"
#include <QMessageBox>
#include <QFileDialog>

Connection::Connection(QWidget *parent) : QDialog(parent), ui(new Ui::Connection)
{
    ui->setupUi(this);
    connect(ui->pbCaCertPath, &QAbstractButton::clicked, this, &Connection::selectCaCertificateFile);
    connect(ui->pbClientCertPath, &QAbstractButton::clicked, this, &Connection::selectLocalCertificateFile);
    connect(ui->pbClientKeyPath, &QAbstractButton::clicked, this, &Connection::selectLocalKeyFile);

    ui->sbPort->setValue(MQTT_BROKER_DEFAULT_PORT);
}

Connection::~Connection() { delete ui; }

void Connection::accept()
{
    QString message;

    if (ui->leAddress->text().isEmpty())
        message = "Inform the broker's address!";
    else if (ui->leCaCertPath->text().isEmpty())
        message = "Inform the TLS CA certificate!";
    else if (ui->leClientCertPath->text().isEmpty())
        message = "Inform the TLS client certificate!";
    else if (ui->leClientKeyPath->text().isEmpty())
        message = "Inform the TLS client key!";

    if (!message.isEmpty())
        QMessageBox::critical(this, QString(), message, QMessageBox::StandardButton::Ok);
    else {
        emit connectToBroker(
            ui->leAddress->text(),
            (qint16)ui->sbPort->value(),
            ui->leCaCertPath->text(),
            ui->leClientCertPath->text(),
            ui->leClientKeyPath->text()
        );
        QDialog::accept();
    }
}

void Connection::selectCaCertificateFile()
{
    ui->leCaCertPath->setText(QFileDialog::getOpenFileName(
        this,
        "Select certificate file",
        QDir::currentPath(),
        "Certificate Files (*.crt)"
    ));
}

void Connection::selectLocalCertificateFile()
{
    ui->leClientCertPath->setText(QFileDialog::getOpenFileName(
        this,
        "Select certificate file",
        QDir::currentPath(),
        "Certificate Files (*.crt)"
    ));
}

void Connection::selectLocalKeyFile()
{
    ui->leClientKeyPath->setText(QFileDialog::getOpenFileName(
        this,
        "Select key file",
        QDir::currentPath(),
        "Key Files (*.key)"
    ));
}
