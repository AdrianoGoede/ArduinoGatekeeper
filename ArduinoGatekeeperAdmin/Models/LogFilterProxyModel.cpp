#include "LogFilterProxyModel.h"
#include "LogTableModel.h"

LogFilterProxyModel::LogFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent), _timestampFrom{QDateTime::fromSecsSinceEpoch(0)}, _timestampTo(QDateTime::fromSecsSinceEpoch(LONG_LONG_MAX)) {}

void LogFilterProxyModel::setDeviceIdFilter(const QString& filter)
{
    QString value = (filter == "All" ? QString() : filter);
    if (value != _deviceIdFilter) {
        _deviceIdFilter = value;
        invalidateFilter();
    }
}

QString LogFilterProxyModel::getDeviceIdFilter() const { return _deviceIdFilter; }

void LogFilterProxyModel::setUserIdFilter(const QString &filter)
{
    QString value = (filter == "All" ? QString() : filter);
    if (value != _userIdFilter) {
        _userIdFilter = value;
        invalidateFilter();
    }
}

QString LogFilterProxyModel::getUserIdFilter() const { return _userIdFilter; }

void LogFilterProxyModel::setAccessStatus(const QString& filter)
{
    QString value = (filter == "All" ? QString() : filter);
    if (value != _accessStatus) {
        _accessStatus = value;
        invalidateFilter();
    }
}

QString LogFilterProxyModel::getAccessStatus() const { return _accessStatus; }

void LogFilterProxyModel::setTimestampFrom(const QDateTime& timestamp)
{
    if (timestamp != _timestampFrom) {
        _timestampFrom = timestamp;
        invalidateFilter();
    }
}

QDateTime LogFilterProxyModel::getTimestampFrom() const{ return _timestampFrom; }

void LogFilterProxyModel::setTimestampTo(const QDateTime& timestamp)
{
    if (timestamp != _timestampTo) {
        _timestampTo = timestamp;
        invalidateFilter();
    }
}

QDateTime LogFilterProxyModel::getTimestampTo() const { return _timestampTo; }

bool LogFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    QModelIndex dataIndex = sourceModel()->index(sourceRow, LogTableModelColumns::DeviceId, sourceParent);
    QString value = sourceModel()->data(dataIndex).toString();
    if (!_deviceIdFilter.isEmpty() && value.compare(_deviceIdFilter, Qt::CaseSensitivity::CaseInsensitive) != 0) return false;

    dataIndex = sourceModel()->index(sourceRow, LogTableModelColumns::UserId, sourceParent);
    value = sourceModel()->data(dataIndex).toString();
    if (!_userIdFilter.isEmpty() && value.compare(_userIdFilter, Qt::CaseSensitivity::CaseInsensitive) != 0) return false;

    dataIndex = sourceModel()->index(sourceRow, LogTableModelColumns::Granted, sourceParent);
    value = sourceModel()->data(dataIndex).toString();
    if (!_accessStatus.isEmpty() && value.compare(_accessStatus, Qt::CaseSensitivity::CaseInsensitive) != 0) return false;

    dataIndex = sourceModel()->index(sourceRow, LogTableModelColumns::Timestamp, sourceParent);
    QDateTime timestamp = sourceModel()->data(dataIndex).toDateTime();
    if (timestamp < _timestampFrom || timestamp > _timestampTo) return false;

    return true;
}
