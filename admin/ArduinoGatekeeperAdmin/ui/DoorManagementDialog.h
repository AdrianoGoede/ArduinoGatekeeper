#ifndef DOORMANAGEMENTDIALOG_H
#define DOORMANAGEMENTDIALOG_H

#include <QDialog>
#include <QJsonObject>

namespace Ui { class DoorManagementDialog; }

struct DoorData {
    qint32 Id = 0;
    QString Label;

    DoorData() = default;
    DoorData(const QJsonObject& content);
    QJsonObject toJson() const;
};

class DoorManagementDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DoorManagementDialog(DoorData& doorData, QWidget* parent = nullptr);
    ~DoorManagementDialog();

private slots:
    void accept() override;

private:
    Ui::DoorManagementDialog* ui;
    DoorData& _doorData;
    void fillData();
};

#endif // DOORMANAGEMENTDIALOG_H
