#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QSslCertificate>
#include <QSslKey>
#include <QMessageBox>
#include <QFile>
#include <QList>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _baseUrl = std::make_unique<QString>();
    _sslConfiguration = std::make_unique<QSslConfiguration>(QSslConfiguration::defaultConfiguration());

    _authDialog = new AuthDialog(this);
    connect(_authDialog, &AuthDialog::authenticationRequested, this, &MainWindow::authenticate);
    connect(ui->pbAuthentication, &QAbstractButton::clicked, this, &MainWindow::openAuthDialog);

    _dashboardDialog = new DashboardDialog(_sslConfiguration.get(), this);
    connect(ui->pbDashboard, &QAbstractButton::clicked, this, &MainWindow::openDashboardDialog);

    _usersDialog = new UsersDialog(_baseUrl.get(), _sslConfiguration.get(), this);
    connect(ui->pbUsers, &QAbstractButton::clicked, this, &MainWindow::openUsersDialog);

    _doorsDialog = new DoorsDialog(_sslConfiguration.get(), this);
    connect(ui->pbDoors, &QAbstractButton::clicked, this, &MainWindow::openDoorsDialog);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::openAuthDialog() { if (_authDialog) _authDialog->exec(); }

void MainWindow::openDashboardDialog() { if (_dashboardDialog) _dashboardDialog->show(); }

void MainWindow::openUsersDialog() { if (_usersDialog) _usersDialog->show(); }

void MainWindow::openDoorsDialog() { if (_doorsDialog) _doorsDialog->show(); }

void MainWindow::authenticate(const QString& baseUrl, const QString& caCertPath, const QString& clientCertPath, const QString& clientKeyPath)
{
    QFile caFile(caCertPath);
    QFile clientCertFile(clientCertPath);
    QFile clientKeyFile(clientKeyPath);

    try {
        if (!caFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Could not open TLS CA certificate file");
        if (!clientCertFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Could not open TLS client certificate file");
        if (!clientKeyFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Could not open TLS client key file");

        QList<QSslCertificate> caCerts = QSslCertificate::fromDevice(&caFile);
        if (caCerts.isEmpty()) throw std::runtime_error("TLS CA certificate file corrupted or invalid");
        QSslCertificate clientCert(&clientCertFile, QSsl::EncodingFormat::Pem);
        if (clientCert.isNull()) throw std::runtime_error("TLS client certificate file corrupted or invalid");
        QSslKey clientKey(&clientKeyFile, QSsl::KeyAlgorithm::Ec, QSsl::EncodingFormat::Pem);
        if (clientKey.isNull()) throw std::runtime_error("TLS client key file corrupted or invalid");

        _sslConfiguration->setCaCertificates(caCerts);
        _sslConfiguration->setLocalCertificate(clientCert);
        _sslConfiguration->setPrivateKey(clientKey);
        _sslConfiguration->setPeerVerifyMode(QSslSocket::PeerVerifyMode::VerifyPeer);
        _sslConfiguration->setProtocol(QSsl::TlsV1_3OrLater);

        *_baseUrl = baseUrl;
        ui->pbDashboard->setEnabled(true);
        ui->pbUsers->setEnabled(true);
        ui->pbDoors->setEnabled(true);
        ui->statusbar->showMessage(QString("Authenticated as '%1'").arg(clientCert.subjectInfo(QSslCertificate::SubjectInfo::CommonName).first().trimmed()));
    }
    catch (const std::runtime_error& error) {
        deauthenticate();
        QMessageBox::critical(this, QString(), error.what(), QMessageBox::StandardButton::Ok);
    }

    caFile.close();
    clientCertFile.close();
    clientKeyFile.close();
}

void MainWindow::deauthenticate()
{
    _baseUrl.reset(nullptr);
    _sslConfiguration.reset(nullptr);
    ui->pbDashboard->setEnabled(false);
    ui->pbUsers->setEnabled(false);
    ui->pbDoors->setEnabled(false);
    ui->statusbar->showMessage("Not Authenticated");
}
