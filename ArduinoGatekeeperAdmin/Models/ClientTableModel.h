#ifndef CLIENTTABLEMODEL_H
#define CLIENTTABLEMODEL_H

#include "GatekeeperModel.h"
#include <QAbstractTableModel>
#include <QDateTime>
#include <QString>
#include <QList>
#include <QMap>

enum ClientTableModelColumns {
    ClientId = 0,
    CurrentStatus = 1,
    LastChange = 2,
    NumColumns
};

class ClientTableModel : public QAbstractTableModel
{
public:
    explicit ClientTableModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    void addEntry(const DeviceStatusEntry& entry);

private:
    QMap<QString, DeviceStatusEntry> _clientEntries;
};

#endif // CLIENTTABLEMODEL_H
