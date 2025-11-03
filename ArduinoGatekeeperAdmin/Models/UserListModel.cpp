#include "UserListModel.h"

UserListModel::UserListModel(QObject *parent) : QAbstractListModel{parent} {}

int UserListModel::rowCount(const QModelIndex& parent) const{ return _userEntries.size(); }

QVariant UserListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= _userEntries.count() || role != Qt::DisplayRole)
        return QVariant();
    return (_userEntries.cbegin() + index.row()).value().uid;
}

void UserListModel::addEntries(const QList<UserEntry>& entries)
{
    beginResetModel();
    for (const UserEntry& entry : entries)
        _userEntries.insert(entry.uid, entry);
    endResetModel();
}

QList<UserEntry> UserListModel::getCurrentList() const { return _userEntries.values(); }
