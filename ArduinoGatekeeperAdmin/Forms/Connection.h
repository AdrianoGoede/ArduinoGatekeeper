#ifndef CONNECTION_H
#define CONNECTION_H

#include <QDialog>

namespace Ui {
    class Connection;
}

struct ConnectionSettings {
    QString Address;
    uint16_t port;
    QString username, password;
};

class Connection : public QDialog
{
    Q_OBJECT

public:
    explicit Connection(QWidget* parent = nullptr, ConnectionSettings* settings = nullptr);
    ~Connection();

private slots:
    void accept() override;

private:
    Ui::Connection* ui;
    ConnectionSettings* _settings;
};

#endif // CONNECTION_H
