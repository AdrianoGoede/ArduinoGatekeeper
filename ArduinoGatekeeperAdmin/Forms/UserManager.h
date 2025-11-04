#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QDialog>
#include <QList>
#include "../Models/GatekeeperModel.h"
#include "../Models/UserListModel.h"
#include "../Forms/UserEditor.h"

namespace Ui {
    class UserManager;
}

class UserManager : public QDialog
{
    Q_OBJECT

public:
    explicit UserManager(QWidget *parent = nullptr);
    ~UserManager();

public slots:
    void accept() override;
    void addUserEntries(const QList<UserEntry>& entries);

private slots:
    void handleListViewClick(const QModelIndex& index);
    void handleUserEdit();
    void handleUserDelete();

signals:
    void newUserList(const QList<UserEntry>& entries);

private:
    Ui::UserManager *ui;
    UserListModel* _userListModel = nullptr;
    UserEditor* _userEditor = nullptr;
};

#endif // USERMANAGER_H
