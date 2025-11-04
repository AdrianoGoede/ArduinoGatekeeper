#ifndef USEREDITOR_H
#define USEREDITOR_H

#include <QDialog>
#include <QAbstractButton>
#include "../Models/GatekeeperModel.h"

namespace Ui {
    class UserEditor;
}

class UserEditor : public QDialog
{
    Q_OBJECT

public:
    explicit UserEditor(QWidget *parent = nullptr);
    ~UserEditor();

public slots:
    int exec() override;
    int exec(UserEntry* entry);

private slots:
    void accept() override;

private:
    Ui::UserEditor* ui;
    UserEntry* _userEntry = nullptr;
};

#endif // USEREDITOR_H
