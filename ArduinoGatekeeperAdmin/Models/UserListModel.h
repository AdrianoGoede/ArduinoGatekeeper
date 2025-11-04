#ifndef USERLISTMODEL_H
#define USERLISTMODEL_H

#include "GatekeeperModel.h"
#include <QAbstractItemModel>
#include <QObject>
#include <QList>
#include <QMap>

class UserListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit UserListModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void addEntries(const QList<UserEntry>& entry);
    void addEntry(const UserEntry& entry);
    void removeEntry(const QString& uid);
    UserEntry getEntry(const QString& uid) const;
    QList<UserEntry> getEntries() const;

private:
    QMap<QString, UserEntry> _userEntries;
};

#endif // USERLISTMODEL_H
