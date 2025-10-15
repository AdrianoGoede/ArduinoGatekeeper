#include "LogExplorer.h"
#include "Forms/ui_LogExplorer.h"

LogExplorer::LogExplorer(QWidget *parent) : QDialog(parent), ui(new Ui::LogExplorer), _logData(new LogTableModel(this))
{
    ui->setupUi(this);
    ui->tvLogs->setModel(_logData);
    connect(ui->pbCloseDialog, &QPushButton::clicked, this, &QWidget::close);
}

LogExplorer::~LogExplorer() { delete ui; }

void LogExplorer::addLogEntry(const LogEntry &entry) { _logData->addEntry(entry.deviceId, entry.userId, entry.timestamp, entry.accessGranted); }
