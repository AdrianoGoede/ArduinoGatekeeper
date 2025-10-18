#ifndef LOGEXPLORER_H
#define LOGEXPLORER_H

#include <QDialog>
#include <QAbstractTableModel>
#include "../Models/GatekeeperModel.h"
#include "../Models/LogTableModel.h"
#include "../Models/LogFilterProxyModel.h"

namespace Ui {
    class LogExplorer;
}

class LogExplorer : public QDialog
{
    Q_OBJECT

public:
    explicit LogExplorer(QWidget *parent = nullptr);
    ~LogExplorer();

public slots:
    void addLogEntry(const LogEntry& entry);

private:
    Ui::LogExplorer *ui;
    LogTableModel* _logData = nullptr;
    LogFilterProxyModel* _filterProxyModel = nullptr;
};

#endif // LOGEXPLORER_H
