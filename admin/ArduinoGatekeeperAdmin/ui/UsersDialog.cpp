#include "UsersDialog.h"
#include "ui_UsersDialog.h"
#include <QMessageBox>

enum TableColumns {
    UserId,
    Label,
    CreatedAt,
    ColumnCount
};

UsersDialog::UsersDialog(const QString* baseUrl, const QSslConfiguration* sslConfig, QWidget* parent) : QDialog(parent)
                                                                                                      , ui(new Ui::UsersDialog)
                                                                                                      , _odataClient(new ODataClient(baseUrl, sslConfig, this))
{
    ui->setupUi(this);
    prepareTable();

    connect(ui->twUsers, &QTableWidget::itemSelectionChanged, this, &UsersDialog::handleRowSelectionChange);
    connect(ui->pbRefresh, &QAbstractButton::clicked, this, &UsersDialog::fetchData);
    connect(ui->pbAdd, &QAbstractButton::clicked, this, &UsersDialog::createUser);
    connect(ui->pbEdit, &QAbstractButton::clicked, this, &UsersDialog::editUser);
    connect(ui->pbDelete, &QAbstractButton::clicked, this, &UsersDialog::deleteUser);
    connect(_odataClient, &ODataClient::getRequestFinished, this, &UsersDialog::handleUserDataToEdit);
    connect(_odataClient, &ODataClient::getCollectionRequestFinished, this, &UsersDialog::handleCollectionRequestResult);
    connect(_odataClient, &ODataClient::postRequestFinished, this, &UsersDialog::handleUserCreationResult);
    connect(_odataClient, &ODataClient::patchRequestFinished, this, &UsersDialog::handleUserEditResult);
    connect(_odataClient, &ODataClient::deleteRequestFinished, this, &UsersDialog::handleUserDeleteResult);
    connect(_odataClient, &ODataClient::requestFailed, this, &UsersDialog::requestFailed);
}

UsersDialog::~UsersDialog() { delete ui; }

void UsersDialog::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);
    fetchData();
}

void UsersDialog::fetchData()
{
    toggleDialogEnabled();

    QMap<QString, QString> params = {
        { "$filter", "Active eq true" },
        { "$select", "Id,Label" },
        { "$orderby", "Id asc" }
    };
    _odataClient->getRequest("Doors", params);

    params = {
        { "$filter", "Active eq true" },
        { "$select", "Id,Label,CreatedAt" },
        { "$orderby", "Id asc" }
    };
    _odataClient->getRequest("Users", params);
}

void UsersDialog::createUser()
{
    try {
        UserData user;
        UserManagementDialog dialog(user, _doorData, this);
        if (dialog.exec() == QDialog::DialogCode::Accepted)
            _odataClient->postRequest("Users", user.toJson());
    }
    catch (const std::runtime_error& error) {
        QMessageBox::critical(this, "Error", error.what(), QMessageBox::StandardButton::Ok);
    }
}

void UsersDialog::editUser()
{
    QList<QTableWidgetItem*> selectedItems = ui->twUsers->selectedItems();
    if (selectedItems.isEmpty()) return;

    toggleDialogEnabled();
    fetchUserToEdit(ui->twUsers->item(selectedItems.first()->row(), TableColumns::UserId)->text().toInt());
}

void UsersDialog::deleteUser()
{
    QList<QTableWidgetItem*> selectedItems = ui->twUsers->selectedItems();
    if (selectedItems.isEmpty()) return;
    int id = ui->twUsers->item(selectedItems.first()->row(), TableColumns::UserId)->text().toInt();

    if (QMessageBox::question(
        this,
        "?",
        QString("Are you sure you want to delete User '%1'?").arg(id),
        (QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No),
        QMessageBox::StandardButton::No
    ) != QMessageBox::StandardButton::Yes) return;

    _odataClient->deleteRequest("Users", id);
}

void UsersDialog::handleUserDataToEdit(const QString& entity, const QJsonObject& result)
{
    try {
        if (entity != "Users") return;

        QList<QTableWidgetItem*> selectedItems = ui->twUsers->selectedItems();
        if (selectedItems.isEmpty()) return;

        UserData user(result);
        user.Id = ui->twUsers->item(selectedItems.first()->row(), TableColumns::UserId)->text().toInt();
        user.Label = ui->twUsers->item(selectedItems.first()->row(), TableColumns::Label)->text();

        UserManagementDialog dialog(user, _doorData, this);
        if (dialog.exec() == QDialog::DialogCode::Accepted)
            _odataClient->patchRequest("Users", user.Id, user.toJson());
        else
            toggleDialogEnabled();
    }
    catch (const std::runtime_error& error) {
        QMessageBox::critical(this, "Error", error.what(), QMessageBox::StandardButton::Ok);
    }
}

void UsersDialog::handleCollectionRequestResult(const QString& entity, const QJsonArray& results)
{
    if (entity == "Users")
        handleUsersList(results);
    else if (entity == "Doors")
        handleDoorsList(results);
}

void UsersDialog::handleUserCreationResult(const QString& entity, const QJsonObject& result)
{
    fetchData();
    QMessageBox::information(this, {}, QString("User created successfuly with ID '%1'").arg(result.value("Id").toInt()), QMessageBox::StandardButton::Ok);
}

void UsersDialog::handleUserEditResult(const QString& entity, const QJsonObject& result)
{
    QMessageBox::information(this, {}, "User saved successfuly!", QMessageBox::StandardButton::Ok);
    toggleDialogEnabled();
}

void UsersDialog::handleUserDeleteResult(const QString& entity)
{
    fetchData();
    QMessageBox::information(this, {}, "User deleted successfuly!", QMessageBox::StandardButton::Ok);
}

void UsersDialog::requestFailed(const QString& entity, const QString& message)
{
    QMessageBox::critical(this, QString("Endpoint '%1'").arg(entity.trimmed()), message, QMessageBox::StandardButton::Ok);
    toggleDialogEnabled();
}

void UsersDialog::handleRowSelectionChange()
{
    ui->pbEdit->setEnabled(!ui->twUsers->selectedItems().isEmpty());
    ui->pbDelete->setEnabled(!ui->twUsers->selectedItems().isEmpty());
}

void UsersDialog::prepareTable()
{
    QStringList labels;
    for (int i = 0; i < TableColumns::ColumnCount; i++) {
        switch (i) {
            case TableColumns::UserId: labels += "ID"; break;
            case TableColumns::Label: labels += "Name"; break;
            case TableColumns::CreatedAt: labels += "Created At"; break;
            default: continue;
        }
    }

    ui->twUsers->setColumnCount(TableColumns::ColumnCount);
    ui->twUsers->setHorizontalHeaderLabels(labels);
    ui->twUsers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
}

void UsersDialog::toggleDialogEnabled()
{
    ui->twUsers->setEnabled(!ui->twUsers->isEnabled());
    ui->pbRefresh->setEnabled(!ui->pbRefresh->isEnabled());
    ui->pbAdd->setEnabled(!ui->pbAdd->isEnabled());
}

void UsersDialog::fetchUserToEdit(int id)
{
    QMap<QString, QString> params = {
        { "$select", "CardId,CardKey" },
        { "$expand", "Permissions($select=DoorId)" },
    };
    _odataClient->getRequest("Users", id, params);
}

void UsersDialog::handleUsersList(const QJsonArray& results)
{
    ui->twUsers->setRowCount(0);

    int row = 0;
    ui->twUsers->setRowCount(results.size());
    for (QJsonValueConstRef item : results) {
        QJsonObject obj = item.toObject();
        QString timestamp = QDateTime::fromString(obj.value("CreatedAt").toString(), Qt::DateFormat::ISODateWithMs).toString("dd/MM/yyyy HH:mm:ss");

        ui->twUsers->setItem(row, TableColumns::UserId, new QTableWidgetItem(QString::number(obj.value("Id").toInt())));
        ui->twUsers->setItem(row, TableColumns::Label, new QTableWidgetItem(obj.value("Label").toString()));
        ui->twUsers->setItem(row, TableColumns::CreatedAt, new QTableWidgetItem(timestamp));
        row++;
    }

    toggleDialogEnabled();
}

void UsersDialog::handleDoorsList(const QJsonArray& results)
{
    _doorData.clear();
    for (QJsonValueConstRef item : results) {
        QJsonObject obj = item.toObject();
        _doorData += DoorData{ obj.value("Id").toInt(), obj.value("Label").toString() };
    }
}
