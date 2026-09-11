#include "UsersDialog.h"
#include "ui_UsersDialog.h"
#include <QMessageBox>

UsersDialog::UsersDialog(const QString* baseUrl, const QSslConfiguration* sslConfig, QWidget* parent) : QDialog(parent)
                                                                                                      , ui(new Ui::UsersDialog)
                                                                                                      , _odataClient(new ODataClient(baseUrl, sslConfig, this))
{
    ui->setupUi(this);
    prepareTable();

    connect(ui->twUsers, &QTableWidget::itemSelectionChanged, this, &UsersDialog::handleRowSelectionChange);
    connect(ui->pbRefresh, &QAbstractButton::clicked, this, &UsersDialog::fetchUsers);
    connect(_odataClient, &ODataClient::getCollectionRequestFinished, this, &UsersDialog::getCollectionRequestFinished);
    connect(_odataClient, &ODataClient::requestFailed, this, &UsersDialog::requestFailed);
}

UsersDialog::~UsersDialog() { delete ui; }

void UsersDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    fetchUsers();
}

void UsersDialog::fetchUsers()
{
    toggleDialogEnabled();
    QMap<QString, QString> params = {
        { "$select", "Id,Label,CreatedAt" },
        { "$orderby", "Id asc" }
    };
    _odataClient->getRequest("Users", params);
}

void UsersDialog::getCollectionRequestFinished(const QJsonArray& results)
{
    ui->twUsers->setRowCount(0);

    int row = 0;
    ui->twUsers->setRowCount(results.size());
    for (QJsonValueConstRef item : results) {
        QJsonObject obj = item.toObject();
        QString timestamp = QDateTime::fromString(obj.value("CreatedAt").toString(), Qt::DateFormat::ISODateWithMs).toString("dd/MM/yyyy HH:mm:ss");

        ui->twUsers->setItem(row, TableColumnNames::UserId, new QTableWidgetItem(QString::number(obj.value("Id").toInt())));
        ui->twUsers->setItem(row, TableColumnNames::Label, new QTableWidgetItem(obj.value("Label").toString()));
        ui->twUsers->setItem(row, TableColumnNames::CreatedAt, new QTableWidgetItem(timestamp));
        row++;
    }

    toggleDialogEnabled();
}

void UsersDialog::requestFailed(const QString& message)
{
    QMessageBox::critical(this, "Error", message, QMessageBox::StandardButton::Ok);
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
    for (int i = 0; i < TableColumnNames::ColumnCount; i++) {
        switch (i) {
            case TableColumnNames::UserId: labels += "ID"; break;
            case TableColumnNames::Label: labels += "Name"; break;
            case TableColumnNames::CreatedAt: labels += "Created At"; break;
            default: continue;
        }
    }

    ui->twUsers->setColumnCount(TableColumnNames::ColumnCount);
    ui->twUsers->setHorizontalHeaderLabels(labels);
    ui->twUsers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
}

void UsersDialog::toggleDialogEnabled()
{
    ui->twUsers->setEnabled(!ui->twUsers->isEnabled());
    ui->pbRefresh->setEnabled(!ui->pbRefresh->isEnabled());
    ui->pbAdd->setEnabled(!ui->pbAdd->isEnabled());
}
