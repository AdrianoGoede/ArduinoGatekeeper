#ifndef LOGFILTERPROXYMODEL_H
#define LOGFILTERPROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>
#include <climits>

class LogFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString _deviceIdFilter READ getDeviceIdFilter WRITE setDeviceIdFilter)
    Q_PROPERTY(QString _userIdFilter READ getUserIdFilter WRITE setUserIdFilter)
    Q_PROPERTY(QString _accessStatus READ getAccessStatus WRITE setAccessStatus)
    Q_PROPERTY(QDateTime _timestampFrom READ getTimestampFrom WRITE setTimestampFrom)
    Q_PROPERTY(QDateTime _timestampTo READ getTimestampTo WRITE setTimestampTo)

public:
    explicit LogFilterProxyModel(QObject* parent = nullptr);
    void setDeviceIdFilter(const QString& filter);
    QString getDeviceIdFilter() const;
    void setUserIdFilter(const QString& filter);
    QString getUserIdFilter() const;
    void setAccessStatus(const QString& filter);
    QString getAccessStatus() const;
    void setTimestampFrom(const QDateTime& timestamp);
    QDateTime getTimestampFrom() const;
    void setTimestampTo(const QDateTime& timestamp);
    QDateTime getTimestampTo() const;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    QString _deviceIdFilter, _userIdFilter, _accessStatus;
    qint64 _timestampFrom = 0, _timestampTo = LONG_LONG_MAX;
};

#endif // LOGFILTERPROXYMODEL_H
