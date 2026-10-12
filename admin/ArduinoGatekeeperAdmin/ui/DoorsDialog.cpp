#include "DoorsDialog.h"
#include "ui_DoorsDialog.h"
#include <QMessageBox>

enum TableColumns {
    DoorId,
    Label,
    CreatedAt,
    ColumnCount
};

DoorsDialog::DoorsDialog(const QString* baseUrl, const QSslConfiguration* sslConfig, QWidget* parent) : QDialog(parent)
                                                                                                      , ui(new Ui::DoorsDialog)
                                                                                                      , _odataClient(new ODataClient(baseUrl, sslConfig, this))
{
    ui->setupUi(this);
    prepareTable();

    connect(ui->twDoors, &QTableWidget::itemSelectionChanged, this, &DoorsDialog::handleRowSelectionChange);
    connect(ui->pbRefresh, &QAbstractButton::clicked, this, &DoorsDialog::fetchData);
    connect(ui->pbDelete, &QAbstractButton::clicked, this, &DoorsDialog::deleteDoor);
    connect(_odataClient, &ODataClient::getCollectionRequestFinished, this, &DoorsDialog::handleCollectionRequestResult);
    connect(_odataClient, &ODataClient::deleteRequestFinished, this, &DoorsDialog::handleDoorDeleteResult);
    connect(_odataClient, &ODataClient::requestFailed, this, &DoorsDialog::requestFailed);
}

DoorsDialog::~DoorsDialog() { delete ui; }

void DoorsDialog::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);
    fetchData();
}

void DoorsDialog::fetchData()
{
    toggleDialogEnabled();

    QMap<QString, QString> params = {
        { "$select", "Id,Label,CreatedAt" },
        { "$orderby", "Id asc" }
    };
    _odataClient->getRequest("Doors", params);
}

void DoorsDialog::deleteDoor()
{
    QList<QTableWidgetItem*> selectedItems = ui->twDoors->selectedItems();
    if (selectedItems.isEmpty()) return;
    int id = ui->twDoors->item(selectedItems.first()->row(), TableColumns::DoorId)->text().toInt();
    QString label = ui->twDoors->item(selectedItems.first()->row(), TableColumns::Label)->text();

    if (QMessageBox::question(
            this,
            "?",
            QString("Are you sure you want to delete Door '%1 - %2'?").arg(id).arg(label),
            (QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No),
            QMessageBox::StandardButton::No
    ) != QMessageBox::StandardButton::Yes) return;

    _odataClient->deleteRequest("Doors", id);
}

void DoorsDialog::handleCollectionRequestResult(const QString& entity, const QJsonArray& results)
{
    ui->twDoors->setRowCount(0);

    int row = 0;
    ui->twDoors->setRowCount(results.size());
    for (QJsonValueConstRef item : results) {
        QJsonObject obj = item.toObject();
        QString timestamp = QDateTime::fromString(obj.value("CreatedAt").toString(), Qt::DateFormat::ISODateWithMs).toString("dd/MM/yyyy HH:mm:ss");

        ui->twDoors->setItem(row, TableColumns::DoorId, new QTableWidgetItem(QString::number(obj.value("Id").toInt())));
        ui->twDoors->setItem(row, TableColumns::Label, new QTableWidgetItem(obj.value("Label").toString()));
        ui->twDoors->setItem(row, TableColumns::CreatedAt, new QTableWidgetItem(timestamp));
        row++;
    }

    toggleDialogEnabled();
}

void DoorsDialog::handleDoorDeleteResult(const QString& entity)
{
    fetchData();
    QMessageBox::information(this, {}, "Door deleted successfuly!", QMessageBox::StandardButton::Ok);
}

void DoorsDialog::requestFailed(const QString& entity, const QString& message)
{
    QMessageBox::critical(this, QString("Endpoint '%1'").arg(entity.trimmed()), message, QMessageBox::StandardButton::Ok);
    toggleDialogEnabled();
}

void DoorsDialog::handleRowSelectionChange()
{
    ui->pbEdit->setEnabled(!ui->twDoors->selectedItems().isEmpty());
    ui->pbDelete->setEnabled(!ui->twDoors->selectedItems().isEmpty());
}

void DoorsDialog::prepareTable()
{
    QStringList labels;
    for (int i = 0; i < TableColumns::ColumnCount; i++) {
        switch (i) {
            case TableColumns::DoorId: labels += "ID"; break;
            case TableColumns::Label: labels += "Name"; break;
            case TableColumns::CreatedAt: labels += "Created At"; break;
            default: continue;
        }
    }

    ui->twDoors->setColumnCount(TableColumns::ColumnCount);
    ui->twDoors->setHorizontalHeaderLabels(labels);
    ui->twDoors->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
}

void DoorsDialog::toggleDialogEnabled()
{
    ui->twDoors->setEnabled(!ui->twDoors->isEnabled());
    ui->pbRefresh->setEnabled(!ui->pbRefresh->isEnabled());
    ui->pbAdd->setEnabled(!ui->pbAdd->isEnabled());
}
