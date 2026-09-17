#include "UserManagementDialog.h"
#include "ui_UserManagementDialog.h"
#include <QByteArray>
#include <QStringList>
#include <QCheckBox>

enum TableColumns {
    DoorId,
    Label,
    Allowed,
    ColumnCount
};

UserData::UserData(const QJsonObject &content) {
    Id = content.value("Id").toInt(0);
    Label = content.value("Id").toString();
    CardId = content.value("CardId").toString({});
    CardKey = QByteArray::fromBase64(content.value("CardKey").toString({}).toUtf8());

    for (QJsonValueConstRef permission : content.value("Permissions").toArray()) {
        QJsonObject obj = permission.toObject();
        AllowedDoors.append(obj.value("DoorId").toInt());
    }
}

QJsonObject UserData::toJson() const {
    QJsonObject json;
    json["Label"] = Label;
    json["CardId"] = CardId;
    json["CardKey"] = QString(CardKey.toBase64());

    QJsonArray doors;
    for (int id : AllowedDoors)
        doors += id;
    json["AllowedDoors"] = doors;

    return json;
}

UserManagementDialog::UserManagementDialog(UserData& userData, const QList<DoorData>& doorData, QWidget* parent) : QDialog(parent), ui(new Ui::UserManagementDialog), _userData(userData)
{
    ui->setupUi(this);
    prepareTable();
    fillData(doorData);
    setValidators();
}

UserManagementDialog::~UserManagementDialog() { delete ui; }

void UserManagementDialog::accept()
{
    QString name = ui->leUserName->text().trimmed();
    if (name.isEmpty())
        throw std::runtime_error("User must have a name");

    QString cardId = ui->leCardId->text().trimmed();
    if (cardId.size() != 8)
        throw std::runtime_error("Card ID invalid");

    _userData.Label = name;
    _userData.CardId = cardId;

    _userData.CardKey.clear();
    _userData.CardKey.append(ui->sbCardKey1->value());
    _userData.CardKey.append(ui->sbCardKey2->value());
    _userData.CardKey.append(ui->sbCardKey3->value());
    _userData.CardKey.append(ui->sbCardKey4->value());
    _userData.CardKey.append(ui->sbCardKey5->value());
    _userData.CardKey.append(ui->sbCardKey6->value());

    _userData.AllowedDoors.clear();
    for (int i = 0; i < ui->twPermissions->rowCount(); i++)
        if (getTableCheckBoxValue(i))
            _userData.AllowedDoors += ui->twPermissions->item(i, TableColumns::DoorId)->text().toInt();

    QDialog::accept();
}

void UserManagementDialog::prepareTable()
{
    QStringList labels;
    for (int i = 0; i < TableColumns::ColumnCount; i++) {
        switch (i) {
            case TableColumns::DoorId: labels += "Door ID"; break;
            case TableColumns::Label: labels += "Door Label"; break;
            case TableColumns::Allowed: labels += "Allowed?"; break;
            default: continue;
        }
    }

    ui->twPermissions->setColumnCount(TableColumns::ColumnCount);
    ui->twPermissions->setHorizontalHeaderLabels(labels);
    ui->twPermissions->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
}

void UserManagementDialog::fillData(const QList<DoorData>& doorData)
{
    this->setWindowTitle(_userData.Id > 0 ? "Edit User" : "New User");
    ui->leUserId->setText((_userData.Id > 0) ? QString::number(_userData.Id) : "-");
    ui->leUserName->setText(_userData.Label);
    ui->leCardId->setText(_userData.CardId);
    ui->sbCardKey1->setValue(!_userData.CardKey.isEmpty() ? (int)_userData.CardKey[0] : 0);
    ui->sbCardKey2->setValue(!_userData.CardKey.isEmpty() ? (int)_userData.CardKey[1] : 0);
    ui->sbCardKey3->setValue(!_userData.CardKey.isEmpty() ? (int)_userData.CardKey[2] : 0);
    ui->sbCardKey4->setValue(!_userData.CardKey.isEmpty() ? (int)_userData.CardKey[3] : 0);
    ui->sbCardKey5->setValue(!_userData.CardKey.isEmpty() ? (int)_userData.CardKey[4] : 0);
    ui->sbCardKey6->setValue(!_userData.CardKey.isEmpty() ? (int)_userData.CardKey[5] : 0);

    int row = 0;
    ui->twPermissions->setRowCount(doorData.size());
    for (const DoorData& door : doorData) {
        ui->twPermissions->setItem(row, TableColumns::DoorId, new QTableWidgetItem(QString::number(door.Id)));
        ui->twPermissions->setItem(row, TableColumns::Label, new QTableWidgetItem(door.Label));

        ui->twPermissions->setCellWidget(row, TableColumns::Allowed, createCheckboxWidget(_userData.AllowedDoors.contains(door.Id), ui->twPermissions));

        row++;
    }
}

void UserManagementDialog::setValidators()
{
    QRegularExpression regex("^[A-Fa-f0-9]*$");
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(regex, ui->leCardId);
    ui->leCardId->setValidator(validator);
}

QWidget* UserManagementDialog::createCheckboxWidget(bool checked, QWidget* parent)
{
    QWidget* widget = new QWidget(parent);
    QCheckBox* checkbox = new QCheckBox(widget);
    QLayout* layout = new QHBoxLayout(widget);

    checkbox->setChecked(checked);
    layout->addWidget(checkbox);
    layout->setAlignment(Qt::AlignmentFlag::AlignCenter);
    layout->setContentsMargins(0, 0, 0, 0);
    widget->setLayout(layout);

    return widget;
}

bool UserManagementDialog::getTableCheckBoxValue(int row)
{
    if (QWidget* widget = ui->twPermissions->cellWidget(row, TableColumns::Allowed))
        if (QCheckBox* checkbox = widget->findChild<QCheckBox*>())
            return checkbox->isChecked();
    return false;
}