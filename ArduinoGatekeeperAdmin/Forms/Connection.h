#ifndef CONNECTION_H
#define CONNECTION_H

#include <QDialog>

namespace Ui {
    class Connection;
}

class Connection : public QDialog
{
    Q_OBJECT

public:
    explicit Connection(QWidget* parent = nullptr);
    ~Connection();

private slots:
    void accept() override;

signals:
    void connectToBroker(const QString& address, qint16 port, const QString& userName, const QString& password);

private:
    Ui::Connection* ui;
};

#endif // CONNECTION_H
