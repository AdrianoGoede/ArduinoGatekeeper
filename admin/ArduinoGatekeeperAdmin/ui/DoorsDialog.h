#ifndef DOORSDIALOG_H
#define DOORSDIALOG_H

#include <QDialog>
#include <QSslConfiguration>

namespace Ui { class DoorsDialog; }

class DoorsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DoorsDialog(const QSslConfiguration* sslConfig, QWidget* parent = nullptr);
    ~DoorsDialog();

private:
    Ui::DoorsDialog* ui;
    const QSslConfiguration* _sslConfig = nullptr;
};

#endif // DOORSDIALOG_H
