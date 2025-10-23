#include "ClientTableModel.h"
#include <QColor>

ClientTableModel::ClientTableModel(QObject* parent) : QAbstractTableModel{parent} {}

int ClientTableModel::rowCount(const QModelIndex& parent) const { return _clientEntries.count(); }

int ClientTableModel::columnCount(const QModelIndex& parent) const { return ClientTableModelColumns::NumColumns; }

QVariant ClientTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= _clientEntries.count())
        return QVariant();

    const DeviceStatusEntry& entry = (_clientEntries.cbegin() + index.row()).value();

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case ClientTableModelColumns::ClientId: return entry.deviceId;
            case ClientTableModelColumns::CurrentStatus: return QString(entry.online ? "Online" : "Offline");
            case ClientTableModelColumns::LastChange: return entry.timestamp.toString("yyyy-MM-dd hh:mm:ss");
        }
    }
    else if (role == Qt::ForegroundRole && index.column() == ClientTableModelColumns::CurrentStatus)
        return QColor(entry.online ? Qt::green : Qt::red);

    return QVariant();
}

QVariant ClientTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return QVariant();

    switch (section) {
        case ClientTableModelColumns::ClientId: return "Device ID";
        case ClientTableModelColumns::CurrentStatus: return "Current Status";
        case ClientTableModelColumns::LastChange: return "Last Status Change";
        default: return QVariant();
    }
}

void ClientTableModel::addEntry(const DeviceStatusEntry& entry)
{
    beginResetModel();
    _clientEntries.insert(entry.deviceId.trimmed(), entry);
    endResetModel();
}
