#include "ClientFilterProxyModel.h"
#include "ClientTableModel.h"
#include <QDateTime>

ClientFilterProxyModel::ClientFilterProxyModel(QObject *parent) : QSortFilterProxyModel{parent}, _timestampFrom{QDateTime::fromSecsSinceEpoch(0)}, _timestampTo(QDateTime::fromSecsSinceEpoch(LONG_LONG_MAX)) {}

void ClientFilterProxyModel::setDeviceIdFilter(const QString& filter)
{
    if (filter != _deviceIdFilter) {
        _deviceIdFilter = filter;
        invalidateFilter();
    }
}

QString ClientFilterProxyModel::getDeviceIdFilter() const { return _deviceIdFilter; }

void ClientFilterProxyModel::setStatusFilter(const QString& filter)
{
    QString value = (filter == "All" ? QString() : filter);
    if (value != _statusFilter) {
        _statusFilter = value;
        invalidateFilter();
    }
}

QString ClientFilterProxyModel::getStatusFilter() const { return _statusFilter; }

void ClientFilterProxyModel::setTimestampFrom(const QDateTime& timestamp)
{
    if (timestamp != _timestampFrom) {
        _timestampFrom = timestamp;
        invalidateFilter();
    }
}

QDateTime ClientFilterProxyModel::getTimestampFrom() const { return _timestampFrom; }

void ClientFilterProxyModel::setTimestampTo(const QDateTime& timestamp)
{
    if (timestamp != _timestampTo) {
        _timestampTo = timestamp;
        invalidateFilter();
    }
}

QDateTime ClientFilterProxyModel::getTimestampTo() const { return _timestampTo; }

bool ClientFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    QModelIndex dataIndex = sourceModel()->index(sourceRow, ClientTableModelColumns::ClientId, sourceParent);
    QString value = sourceModel()->data(dataIndex).toString();
    if (!_deviceIdFilter.isEmpty() && !value.contains(_deviceIdFilter, Qt::CaseSensitivity::CaseInsensitive)) return false;

    dataIndex = sourceModel()->index(sourceRow, ClientTableModelColumns::CurrentStatus, sourceParent);
    value = sourceModel()->data(dataIndex).toString();
    if (!_statusFilter.isEmpty() && value.compare(_statusFilter, Qt::CaseSensitivity::CaseInsensitive) != 0) return false;

    dataIndex = sourceModel()->index(sourceRow, ClientTableModelColumns::LastChange, sourceParent);
    QDateTime timestamp = sourceModel()->data(dataIndex).toDateTime();
    if (timestamp < _timestampFrom || timestamp > _timestampTo) return false;

    return true;
}
