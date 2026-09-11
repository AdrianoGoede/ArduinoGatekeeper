#ifndef USERSDIALOG_H
#define USERSDIALOG_H

#include <QDialog>
#include <QJsonArray>
#include <QSslConfiguration>
#include "../core/ODataClient.h"

namespace Ui { class UsersDialog; }

enum TableColumnNames {
    UserId,
    Label,
    CreatedAt,
    ColumnCount
};

class UsersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UsersDialog(const QString* baseUrl, const QSslConfiguration* sslConfig, QWidget* parent = nullptr);
    ~UsersDialog();

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void fetchUsers();
    void getCollectionRequestFinished(const QJsonArray& results);
    void requestFailed(const QString& message);
    void handleRowSelectionChange();

private:
    Ui::UsersDialog* ui;
    ODataClient* _odataClient = nullptr;
    void prepareTable();
    void toggleDialogEnabled();
};

#endif // USERSDIALOG_H
