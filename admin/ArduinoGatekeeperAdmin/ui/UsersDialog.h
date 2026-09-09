#ifndef USERSDIALOG_H
#define USERSDIALOG_H

#include <QDialog>
#include <QSslConfiguration>

namespace Ui { class UsersDialog; }

class UsersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UsersDialog(const QSslConfiguration* sslConfig, QWidget* parent = nullptr);
    ~UsersDialog();

private:
    Ui::UsersDialog* ui;
    const QSslConfiguration* _sslConfig = nullptr;
};

#endif // USERSDIALOG_H
