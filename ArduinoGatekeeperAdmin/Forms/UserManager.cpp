#include "UserManager.h"
#include "Forms/ui_UserManager.h"
#include <QMessageBox>

UserManager::UserManager(QWidget *parent) : QDialog(parent), ui(new Ui::UserManager), _userListModel(new UserListModel(this)), _userEditor(new UserEditor(this))
{
    ui->setupUi(this);
    ui->lvUsers->setModel(_userListModel);
    connect(ui->pbSave, &QAbstractButton::clicked, this, &QDialog::accept);
    connect(ui->pbClose, &QAbstractButton::clicked, this, &QDialog::reject);
    connect(ui->lvUsers, &QAbstractItemView::clicked, this, &UserManager::handleListViewClick);
    connect(ui->pbEditUser, &QAbstractButton::clicked, this, &UserManager::handleUserEdit);
    connect(ui->pbDeleteUser, &QAbstractButton::clicked, this, &UserManager::handleUserDelete);
}

UserManager::~UserManager() { delete ui; }

void UserManager::accept()
{
    emit newUserList(_userListModel->getEntries());
    QDialog::accept();
}

void UserManager::addUserEntries(const QList<UserEntry>& entries) { _userListModel->addEntries(entries); }

void UserManager::handleListViewClick(const QModelIndex& index)
{
    ui->pbEditUser->setEnabled(index.isValid());
    ui->pbDeleteUser->setEnabled(index.isValid());
}

void UserManager::handleUserEdit()
{
    QModelIndex index = ui->lvUsers->currentIndex();
    if (!index.isValid()) return;
    UserEntry entry = _userListModel->getEntry(index.data(Qt::DisplayRole).toString());
    if (_userEditor->exec(&entry) == QDialog::DialogCode::Accepted)
        _userListModel->addEntry(entry);
}

void UserManager::handleUserDelete()
{
    bool sure = (QMessageBox::question(
        this,
        "Are you sure?",
        "Are you sure you want to remove this user?",
        (QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No),
        QMessageBox::StandardButton::No
    ) == QMessageBox::StandardButton::Yes);
    if (!sure) return;

    QModelIndex index = ui->lvUsers->currentIndex();
    if (!index.isValid()) return;
    QString uid = index.data(Qt::DisplayRole).toString();
    _userListModel->removeEntry(uid);
}
