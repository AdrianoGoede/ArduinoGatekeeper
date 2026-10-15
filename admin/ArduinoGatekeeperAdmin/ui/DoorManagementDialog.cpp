#include "DoorManagementDialog.h"
#include "ui_DoorManagementDialog.h"

DoorData::DoorData(const QJsonObject& content)
{
    Id = content.value("Id").toInt(0);
    Label = content.value("Id").toString();
}

QJsonObject DoorData::toJson() const
{
    QJsonObject json;
    json["Label"] = Label;
    return json;
}

DoorManagementDialog::DoorManagementDialog(DoorData& doorData, QWidget* parent) : QDialog(parent), ui(new Ui::DoorManagementDialog), _doorData(doorData)
{
    ui->setupUi(this);
    fillData();
}

DoorManagementDialog::~DoorManagementDialog() { delete ui; }

void DoorManagementDialog::accept()
{
    QString name = ui->leDoorName->text();
    if (name.isEmpty())
        throw std::runtime_error("Door must have a name");
    _doorData.Label = name;
    QDialog::accept();
}

void DoorManagementDialog::fillData()
{
    this->setWindowTitle(_doorData.Id > 0 ? "Edit Door" : "New Door");
    ui->leDoorId->setText((_doorData.Id > 0) ? QString::number(_doorData.Id) : "-");
    ui->leDoorName->setText(_doorData.Label);
}
