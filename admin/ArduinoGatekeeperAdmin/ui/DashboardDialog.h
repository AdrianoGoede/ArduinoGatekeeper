#ifndef DASHBOARDDIALOG_H
#define DASHBOARDDIALOG_H

#include <QDialog>
#include <QSslConfiguration>

namespace Ui { class DashboardDialog; }

class DashboardDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DashboardDialog(const QSslConfiguration* sslConfig, QWidget* parent = nullptr);
    ~DashboardDialog();

private:
    Ui::DashboardDialog* ui;
    const QSslConfiguration* _sslConfig = nullptr;
};

#endif // DASHBOARDDIALOG_H
