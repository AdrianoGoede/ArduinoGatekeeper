#include "AuthDialog.h"
#include "ui_AuthDialog.h"
#include <QFileDialog>
#include <QMessageBox>

AuthDialog::AuthDialog(QWidget* parent) : QDialog(parent), ui(new Ui::AuthDialog)
{
    ui->setupUi(this);
    connect(ui->pbCaCert, &QAbstractButton::clicked, this, &AuthDialog::selectCaCert);
    connect(ui->pbClientCert, &QAbstractButton::clicked, this, &AuthDialog::selectClientCert);
    connect(ui->pbClientKey, &QAbstractButton::clicked, this, &AuthDialog::selectClientKey);
}

AuthDialog::~AuthDialog() { delete ui; }

void AuthDialog::selectCaCert() { selectFile(ui->leCaCert, "Certificate Files (*.crt)"); }

void AuthDialog::selectClientCert() { selectFile(ui->leClientCert, "Certificate Files (*.crt)"); }

void AuthDialog::selectClientKey() { selectFile(ui->leClientKey, "Certificate Key Files (*.key)"); }

void AuthDialog::accept()
{
    QString baseUrl = ui->leBaseUrl->text();
    if (baseUrl.isEmpty()) {
        QMessageBox::critical(this, "Error", "Base URL must be informed!", QMessageBox::StandardButton::Ok);
        return;
    }

    QString caCert = ui->leCaCert->text();
    if (caCert.isEmpty()) {
        QMessageBox::critical(this, "Error", "CA Cert must be informed!", QMessageBox::StandardButton::Ok);
        return;
    }

    QString clientCert = ui->leClientCert->text();
    if (clientCert.isEmpty()) {
        QMessageBox::critical(this, "Error", "Client Cert must be informed!", QMessageBox::StandardButton::Ok);
        return;
    }

    QString clientKey = ui->leClientKey->text();
    if (clientKey.isEmpty()) {
        QMessageBox::critical(this, "Error", "Client Key must be informed!", QMessageBox::StandardButton::Ok);
        return;
    }

    authenticationRequested(baseUrl, caCert, clientCert, clientKey);
    QDialog::accept();
}

void AuthDialog::selectFile(QLineEdit* output, const QString& filter)
{
    if (!output) return;

    output->setText(QFileDialog::getOpenFileName(
        this,
        "Select File",
        QDir::currentPath(),
        filter
    ));
}
