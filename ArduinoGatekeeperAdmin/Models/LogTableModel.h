#ifndef LOGTABLEMODEL_H
#define LOGTABLEMODEL_H

#include "GatekeeperModel.h"
#include <QAbstractTableModel>
#include <QDateTime>
#include <QString>
#include <QList>

enum LogTableModelColumns {
    DeviceId = 0,
    UserId = 1,
    Granted = 2,
    Timestamp = 3,
    ColumnCount
};

class LogTableModel : public QAbstractTableModel
{
public:
    explicit LogTableModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    void addEntry(const QString& deviceId, const QString& userId, const QDateTime& timestamp, bool granted);

private:
    QList<LogEntry> _logEntries;
};

#endif // LOGTABLEMODEL_H
