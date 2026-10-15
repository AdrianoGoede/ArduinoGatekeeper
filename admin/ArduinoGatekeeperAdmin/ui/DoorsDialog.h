#ifndef DOORSDIALOG_H
#define DOORSDIALOG_H

#include <QDialog>
#include <QSslConfiguration>
#include "../core/ODataClient.h"

namespace Ui { class DoorsDialog; }

class DoorsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DoorsDialog(const QString* baseUrl, const QSslConfiguration* sslConfig, QWidget* parent = nullptr);
    ~DoorsDialog();

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void fetchData();
    void createDoor();
    void editDoor();
    void deleteDoor();
    void handleCollectionRequestResult(const QString& entity, const QJsonArray& results);
    void handleDoorCreationResult(const QString& entity, const QJsonObject& result);
    void handleDoorEditResult(const QString& entity, const QJsonObject& result);
    void handleDoorDeleteResult(const QString& entity);
    void requestFailed(const QString& entity, const QString& message);
    void handleRowSelectionChange();

private:
    Ui::DoorsDialog* ui;
    ODataClient* _odataClient = nullptr;
    void prepareTable();
    void toggleDialogEnabled();
};

#endif // DOORSDIALOG_H
