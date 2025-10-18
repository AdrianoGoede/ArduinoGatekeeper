#include "LogFilterProxyModel.h"
#include "LogTableModel.h"

LogFilterProxyModel::LogFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {}

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
    if (timestamp.toSecsSinceEpoch() != _timestampFrom) {
        _timestampFrom = timestamp.toSecsSinceEpoch();
        invalidateFilter();
    }
}

QDateTime LogFilterProxyModel::getTimestampFrom() const{ return QDateTime::fromMSecsSinceEpoch(_timestampFrom); }

void LogFilterProxyModel::setTimestampTo(const QDateTime &timestamp)
{
    if (timestamp.toSecsSinceEpoch() != _timestampTo) {
        _timestampTo = timestamp.toSecsSinceEpoch();
        invalidateFilter();
    }
}

QDateTime LogFilterProxyModel::getTimestampTo() const { return QDateTime::fromMSecsSinceEpoch(_timestampTo); }

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
    qint64 epoch = sourceModel()->data(dataIndex).toDateTime().toSecsSinceEpoch();
    if (_timestampFrom >= 0 && epoch <= _timestampFrom) return false;
    if (_timestampTo >= 0 && epoch >= _timestampTo) return false;

    return true;
}
