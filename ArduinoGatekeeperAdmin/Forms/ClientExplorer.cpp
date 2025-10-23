#include "ClientExplorer.h"
#include "../Forms/ui_ClientExplorer.h"

ClientExplorer::ClientExplorer(QWidget *parent) : QDialog(parent), ui(new Ui::ClientExplorer), _clientData(new ClientTableModel(this)), _filterProxyModel(new ClientFilterProxyModel(this))
{
    ui->setupUi(this);
    _filterProxyModel->setSourceModel(_clientData);
    ui->tvClients->setModel(_filterProxyModel);

    ui->dteTimestampFrom->setDateTime(QDateTime::currentDateTime().addDays(-1));
    _filterProxyModel->setTimestampFrom(ui->dteTimestampFrom->dateTime());
    ui->dteTimestampTo->setDateTime(QDateTime::currentDateTime().addDays(1));
    _filterProxyModel->setTimestampTo(ui->dteTimestampTo->dateTime());

    QHeaderView* header = ui->tvClients->horizontalHeader();
    header->setSectionResizeMode(ClientTableModelColumns::ClientId, QHeaderView::ResizeMode::Stretch);
    header->setSectionResizeMode(ClientTableModelColumns::CurrentStatus, QHeaderView::ResizeMode::Stretch);
    header->setSectionResizeMode(ClientTableModelColumns::LastChange, QHeaderView::ResizeMode::Stretch);

    connect(ui->pbCloseDialog, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->leDeviceId, &QLineEdit::textChanged, _filterProxyModel, &ClientFilterProxyModel::setDeviceIdFilter);
    connect(ui->cbStatus, QOverload<const QString&>::of(&QComboBox::currentTextChanged), _filterProxyModel, &ClientFilterProxyModel::setStatusFilter);
    connect(ui->dteTimestampFrom, &QDateTimeEdit::dateTimeChanged, this, &ClientExplorer::handleTimestampFromFilter);
    connect(ui->dteTimestampFrom, &QDateTimeEdit::dateTimeChanged, _filterProxyModel, &ClientFilterProxyModel::setTimestampFrom);
    connect(ui->dteTimestampTo, &QDateTimeEdit::dateTimeChanged, this, &ClientExplorer::handleTimestampToFilter);
    connect(ui->dteTimestampTo, &QDateTimeEdit::dateTimeChanged, _filterProxyModel, &ClientFilterProxyModel::setTimestampTo);
}

ClientExplorer::~ClientExplorer() { delete ui; }

void ClientExplorer::addClientEntry(const DeviceStatusEntry& entry)
{
    _clientData->addEntry(entry);

    ui->dteTimestampFrom->setMinimumDateTime(std::min(
        ui->dteTimestampFrom->minimumDateTime(),
        entry.timestamp
    ));

    ui->dteTimestampTo->setMaximumDateTime(std::max(
        ui->dteTimestampTo->maximumDateTime(),
        entry.timestamp
    ));
}

void ClientExplorer::handleTimestampFromFilter(const QDateTime& timestamp) { ui->dteTimestampTo->setMinimumDateTime(timestamp); }

void ClientExplorer::handleTimestampToFilter(const QDateTime& timestamp) { ui->dteTimestampFrom->setMaximumDateTime(timestamp); }
