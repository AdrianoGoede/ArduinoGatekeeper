#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSslConfiguration>
#include "AuthDialog.h"
#include "DashboardDialog.h"
#include "UsersDialog.h"
#include "DoorsDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void openAuthDialog();
    void openDashboardDialog();
    void openUsersDialog();
    void openDoorsDialog();
    void authenticate(const QString& caCertPath, const QString& clientCertPath, const QString& clientKeyPath);
    void deauthenticate();

private:
    Ui::MainWindow* ui;
    std::unique_ptr<QSslConfiguration> _sslConfiguration = nullptr;
    AuthDialog* _authDialog = nullptr;
    DashboardDialog* _dashboardDialog = nullptr;
    UsersDialog* _usersDialog = nullptr;
    DoorsDialog* _doorsDialog = nullptr;
};
#endif // MAINWINDOW_H
