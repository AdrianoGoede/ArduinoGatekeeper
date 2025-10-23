#ifndef CLIENTFILTERPROXYMODEL_H
#define CLIENTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QDateTime>

class ClientFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString _deviceIdFilter READ getDeviceIdFilter WRITE setDeviceIdFilter)
    Q_PROPERTY(QString _statusFilter READ getStatusFilter WRITE setStatusFilter)
    Q_PROPERTY(QDateTime _timestampFrom READ getTimestampFrom WRITE setTimestampFrom)
    Q_PROPERTY(QDateTime _timestampTo READ getTimestampTo WRITE setTimestampTo)

public:
    explicit ClientFilterProxyModel(QObject *parent = nullptr);
    void setDeviceIdFilter(const QString& filter);
    QString getDeviceIdFilter() const;
    void setStatusFilter(const QString& filter);
    QString getStatusFilter() const;
    void setTimestampFrom(const QDateTime& timestamp);
    QDateTime getTimestampFrom() const;
    void setTimestampTo(const QDateTime& timestamp);
    QDateTime getTimestampTo() const;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    QString _deviceIdFilter, _statusFilter;
    QDateTime _timestampFrom, _timestampTo;
};

#endif // CLIENTFILTERPROXYMODEL_H
