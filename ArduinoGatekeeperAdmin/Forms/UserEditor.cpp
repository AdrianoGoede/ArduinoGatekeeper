#include "UserEditor.h"
#include "Forms/ui_UserEditor.h"
#include <QMessageBox>

UserEditor::UserEditor(QWidget *parent) : QDialog(parent), ui(new Ui::UserEditor)
{
    ui->setupUi(this);
    ui->leUid->setMaxLength(USER_RFID_UID_LENGHT);
    ui->leUid->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[0-9A-Fa-f]*"),
        ui->leUid
    ));

    connect(ui->bbButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->bbButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

UserEditor::~UserEditor() { delete ui; }

int UserEditor::exec() { return QDialog::DialogCode::Rejected; }

int UserEditor::exec(UserEntry* entry)
{
    if (!entry) QDialog::reject();
    _userEntry = entry;

    ui->leUid->setText(entry->uid);
    ui->leUid->setReadOnly(!entry->uid.isEmpty());

    ui->sbKey0->setValue(entry->key[0]);
    ui->sbKey1->setValue(entry->key[1]);
    ui->sbKey2->setValue(entry->key[2]);
    ui->sbKey3->setValue(entry->key[3]);
    ui->sbKey4->setValue(entry->key[4]);
    ui->sbKey5->setValue(entry->key[5]);

    return QDialog::exec();
}

void UserEditor::accept()
{
    if (ui->leUid->text().size() != USER_RFID_UID_LENGHT) {
        QMessageBox::critical(this, "Error", "Enter a valid UID in hex format");
        return;
    }

    _userEntry->uid = ui->leUid->text();
    _userEntry->key[0] = (uint8_t)ui->sbKey0->value();
    _userEntry->key[1] = (uint8_t)ui->sbKey1->value();
    _userEntry->key[2] = (uint8_t)ui->sbKey2->value();
    _userEntry->key[3] = (uint8_t)ui->sbKey3->value();
    _userEntry->key[4] = (uint8_t)ui->sbKey4->value();
    _userEntry->key[5] = (uint8_t)ui->sbKey5->value();

    QDialog::accept();
}
