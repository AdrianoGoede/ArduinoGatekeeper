#ifndef CLIENTEXPLORER_H
#define CLIENTEXPLORER_H

#include <QDialog>
#include "../Models/GatekeeperModel.h"
#include "../Models/ClientTableModel.h"
#include "../Models/ClientFilterProxyModel.h"

namespace Ui {
    class ClientExplorer;
}

class ClientExplorer : public QDialog
{
    Q_OBJECT

public:
    explicit ClientExplorer(QWidget *parent = nullptr);
    ~ClientExplorer();

public slots:
    void addClientEntry(const DeviceStatusEntry& entry);

private slots:
    void handleTimestampFromFilter(const QDateTime& timestamp);
    void handleTimestampToFilter(const QDateTime& timestamp);

private:
    Ui::ClientExplorer *ui;
    ClientTableModel* _clientData = nullptr;
    ClientFilterProxyModel* _filterProxyModel = nullptr;
};

#endif // CLIENTEXPLORER_H
