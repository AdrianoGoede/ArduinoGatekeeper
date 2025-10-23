#include "LogTableModel.h"
#include <QColor>
#include <QIcon>

LogTableModel::LogTableModel(QObject *parent) : QAbstractTableModel{parent} {}

int LogTableModel::rowCount(const QModelIndex &parent) const { return _logEntries.count(); }

int LogTableModel::columnCount(const QModelIndex &parent) const { return LogTableModelColumns::ColumnCount; }

QVariant LogTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= _logEntries.count())
        return QVariant();

    const LogEntry& entry = _logEntries.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case LogTableModelColumns::DeviceId: return entry.deviceId;
            case LogTableModelColumns::UserId: return entry.userId;
            case LogTableModelColumns::Granted: return QString(entry.accessGranted ? "Yes" : "No");
            case LogTableModelColumns::Timestamp: return entry.timestamp.toString("yyyy-MM-dd hh:mm:ss");
        }
    }
    else if (role == Qt::ForegroundRole && index.column() == LogTableModelColumns::Granted)
        return QColor(entry.accessGranted ? Qt::green : Qt::red);

    return QVariant();
}

QVariant LogTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return QVariant();

    switch (section) {
        case LogTableModelColumns::DeviceId: return "Device ID";
        case LogTableModelColumns::UserId: return "User ID";
        case LogTableModelColumns::Granted: return "Granted";
        case LogTableModelColumns::Timestamp: return "Timestamp";
        default: return QVariant();
    }
}

void LogTableModel::addEntry(const LogEntry& entry)
{
    beginInsertRows(QModelIndex(), _logEntries.count(), _logEntries.count());
    _logEntries.append(entry);
    endInsertRows();
}
