#ifndef USERSDIALOG_H
#define USERSDIALOG_H

#include <QDialog>
#include <QList>
#include <QJsonArray>
#include <QSslConfiguration>
#include "UserManagementDialog.h"
#include "../core/ODataClient.h"

namespace Ui { class UsersDialog; }

class UsersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UsersDialog(const QString* baseUrl, const QSslConfiguration* sslConfig, QWidget* parent = nullptr);
    ~UsersDialog();

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void fetchData();
    void createUser();
    void editUser();
    void deleteUser();
    void handleUserDataToEdit(const QString& entity, const QJsonObject& result);
    void handleCollectionRequestResult(const QString& entity, const QJsonArray& results);
    void handleUserCreationResult(const QString& entity, const QJsonObject& result);
    void handleUserEditResult(const QString& entity, const QJsonObject& result);
    void handleUserDeleteResult(const QString& entity);
    void requestFailed(const QString& entity, const QString& message);
    void handleRowSelectionChange();

private:
    Ui::UsersDialog* ui;
    ODataClient* _odataClient = nullptr;
    QList<DoorData> _doorData;
    void prepareTable();
    void toggleDialogEnabled();
    void fetchUserToEdit(int id);
    void handleUsersList(const QJsonArray& results);
    void handleDoorsList(const QJsonArray& results);
};

#endif // USERSDIALOG_H
