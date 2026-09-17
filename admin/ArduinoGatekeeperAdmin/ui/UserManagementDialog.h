#ifndef USERMANAGEMENTDIALOG_H
#define USERMANAGEMENTDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonArray>

namespace Ui { class UserManagementDialog; }

struct UserData {
    qint32 Id = 0;
    QString CardId, Label;
    QByteArray CardKey;
    QList<int> AllowedDoors;

    UserData() = default;
    UserData(const QJsonObject& content);
    QJsonObject toJson() const;
};

struct DoorData {
    qint32 Id;
    QString Label;
};

class UserManagementDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserManagementDialog(UserData& userData, const QList<DoorData>& doorData, QWidget* parent = nullptr);
    ~UserManagementDialog();

private slots:
    void accept() override;

private:
    Ui::UserManagementDialog* ui;
    UserData& _userData;
    void prepareTable();
    void fillData(const QList<DoorData>& doorData);
    void setValidators();
    QWidget* createCheckboxWidget(bool checked, QWidget* parent = nullptr);
    bool getTableCheckBoxValue(int row);
};

#endif // USERMANAGEMENTDIALOG_H
