#ifndef AUTHDIALOG_H
#define AUTHDIALOG_H

#include <QDialog>
#include <QLineEdit>

namespace Ui { class AuthDialog; }

class AuthDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AuthDialog(QWidget* parent = nullptr);
    ~AuthDialog();

signals:
    void authenticationRequested(QString baseUrl, QString caCert, QString clientCert, QString clientKey);

private slots:
    void selectCaCert();
    void selectClientCert();
    void selectClientKey();
    void accept() override;

private:
    Ui::AuthDialog* ui;
    void selectFile(QLineEdit* output, const QString& filter);
};

#endif // AUTHDIALOG_H
