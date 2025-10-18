#include "LogExplorer.h"
#include "Forms/ui_LogExplorer.h"

LogExplorer::LogExplorer(QWidget *parent) : QDialog(parent), ui(new Ui::LogExplorer), _logData(new LogTableModel(this)), _filterProxyModel(new LogFilterProxyModel(this))
{
    ui->setupUi(this);
    _filterProxyModel->setSourceModel(_logData);
    ui->tvLogs->setModel(_filterProxyModel);
    ui->tvLogs->setSortingEnabled(true);

    ui->dteTimestampFrom->setDateTime(QDateTime::currentDateTime().addDays(-1));
    ui->dteTimestampTo->setDateTime(QDateTime::currentDateTime().addDays(1));

    QHeaderView* header = ui->tvLogs->horizontalHeader();
    header->setSectionResizeMode(LogTableModelColumns::DeviceId, QHeaderView::ResizeMode::Stretch);
    header->setSectionResizeMode(LogTableModelColumns::UserId, QHeaderView::ResizeMode::Stretch);
    header->setSectionResizeMode(LogTableModelColumns::Granted, QHeaderView::ResizeMode::Stretch);
    header->setSectionResizeMode(LogTableModelColumns::Timestamp, QHeaderView::ResizeMode::Stretch);

    connect(ui->pbCloseDialog, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->cbDeviceId, QOverload<const QString&>::of(&QComboBox::currentTextChanged), _filterProxyModel, &LogFilterProxyModel::setDeviceIdFilter);
    connect(ui->cbUserId, QOverload<const QString&>::of(&QComboBox::currentTextChanged), _filterProxyModel, &LogFilterProxyModel::setUserIdFilter);
    connect(ui->cbAccessGranted, QOverload<const QString&>::of(&QComboBox::currentTextChanged), _filterProxyModel, &LogFilterProxyModel::setAccessStatus);
    connect(ui->dteTimestampFrom, &QDateTimeEdit::dateTimeChanged, _filterProxyModel, &LogFilterProxyModel::setTimestampFrom);
    connect(ui->dteTimestampTo, &QDateTimeEdit::dateTimeChanged, _filterProxyModel, &LogFilterProxyModel::setTimestampTo);
}

LogExplorer::~LogExplorer() { delete ui; }

void LogExplorer::addLogEntry(const LogEntry &entry) {
    _logData->addEntry(entry.deviceId, entry.userId, entry.timestamp, entry.accessGranted);

    ui->cbDeviceId->setEnabled(true);
    if (ui->cbDeviceId->findText(entry.deviceId) < 0)
        ui->cbDeviceId->addItem(entry.deviceId);

    ui->cbUserId->setEnabled(true);
    if (ui->cbUserId->findText(entry.userId) < 0)
        ui->cbUserId->addItem(entry.userId);
}
