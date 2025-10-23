#include "LogExplorer.h"
#include "Forms/ui_LogExplorer.h"

LogExplorer::LogExplorer(QWidget *parent) : QDialog(parent), ui(new Ui::LogExplorer), _logData(new LogTableModel(this)), _filterProxyModel(new LogFilterProxyModel(this))
{
    ui->setupUi(this);
    _filterProxyModel->setSourceModel(_logData);
    ui->tvLogs->setModel(_filterProxyModel);

    ui->dteTimestampFrom->setDateTime(QDateTime::currentDateTime().addDays(-1));
    _filterProxyModel->setTimestampFrom(ui->dteTimestampFrom->dateTime());
    ui->dteTimestampTo->setDateTime(QDateTime::currentDateTime().addDays(1));
    _filterProxyModel->setTimestampTo(ui->dteTimestampTo->dateTime());

    QHeaderView* header = ui->tvLogs->horizontalHeader();
    header->setSectionResizeMode(LogTableModelColumns::DeviceId, QHeaderView::ResizeMode::Stretch);
    header->setSectionResizeMode(LogTableModelColumns::UserId, QHeaderView::ResizeMode::Stretch);
    header->setSectionResizeMode(LogTableModelColumns::Granted, QHeaderView::ResizeMode::Stretch);
    header->setSectionResizeMode(LogTableModelColumns::Timestamp, QHeaderView::ResizeMode::Stretch);

    connect(ui->pbCloseDialog, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->cbDeviceId, QOverload<const QString&>::of(&QComboBox::currentTextChanged), _filterProxyModel, &LogFilterProxyModel::setDeviceIdFilter);
    connect(ui->cbUserId, QOverload<const QString&>::of(&QComboBox::currentTextChanged), _filterProxyModel, &LogFilterProxyModel::setUserIdFilter);
    connect(ui->cbAccessGranted, QOverload<const QString&>::of(&QComboBox::currentTextChanged), _filterProxyModel, &LogFilterProxyModel::setAccessStatus);
    connect(ui->dteTimestampFrom, &QDateTimeEdit::dateTimeChanged, this, &LogExplorer::handleTimestampFromFilter);
    connect(ui->dteTimestampFrom, &QDateTimeEdit::dateTimeChanged, _filterProxyModel, &LogFilterProxyModel::setTimestampFrom);
    connect(ui->dteTimestampTo, &QDateTimeEdit::dateTimeChanged, this, &LogExplorer::handleTimestampToFilter);
    connect(ui->dteTimestampTo, &QDateTimeEdit::dateTimeChanged, _filterProxyModel, &LogFilterProxyModel::setTimestampTo);
}

LogExplorer::~LogExplorer() { delete ui; }

void LogExplorer::addLogEntry(const LogEntry &entry) {
    _logData->addEntry(entry);

    ui->cbDeviceId->setEnabled(true);
    if (ui->cbDeviceId->findText(entry.deviceId) < 0)
        ui->cbDeviceId->addItem(QIcon::fromTheme(QIcon::ThemeIcon::MediaFlash), entry.deviceId);

    ui->cbUserId->setEnabled(true);
    if (ui->cbUserId->findText(entry.userId) < 0)
        ui->cbUserId->addItem(QIcon::fromTheme(QIcon::ThemeIcon::UserAvailable), entry.userId);

    ui->dteTimestampFrom->setMinimumDateTime(std::min(
        ui->dteTimestampFrom->minimumDateTime(),
        entry.timestamp
    ));

    ui->dteTimestampTo->setMaximumDateTime(std::max(
        ui->dteTimestampTo->maximumDateTime(),
        entry.timestamp
    ));
}

void LogExplorer::handleTimestampFromFilter(const QDateTime& timestamp) { ui->dteTimestampTo->setMinimumDateTime(timestamp); }

void LogExplorer::handleTimestampToFilter(const QDateTime& timestamp) { ui->dteTimestampFrom->setMaximumDateTime(timestamp); }
