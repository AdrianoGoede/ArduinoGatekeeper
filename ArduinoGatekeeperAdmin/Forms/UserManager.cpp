#include "UserManager.h"
#include "Forms/ui_UserManager.h"

UserManager::UserManager(QWidget *parent) : QDialog(parent), ui(new Ui::UserManager), _userListModel(new UserListModel(this))
{
    ui->setupUi(this);
    ui->lvUsers->setModel(_userListModel);
    connect(ui->pbSave, &QAbstractButton::clicked, this, &QDialog::accept);
    connect(ui->pbClose, &QAbstractButton::clicked, this, &QDialog::reject);
    connect(ui->lvUsers, &QAbstractItemView::clicked, this, &UserManager::handleListViewClick);
}

UserManager::~UserManager() { delete ui; }

void UserManager::accept()
{
    emit newUserList(_userListModel->getCurrentList());
    QDialog::accept();
}

void UserManager::addUserEntries(const QList<UserEntry>& entries) { _userListModel->addEntries(entries); }

void UserManager::handleListViewClick(const QModelIndex& index)
{
    ui->pbDeleteUser->setEnabled(index.isValid());
}
