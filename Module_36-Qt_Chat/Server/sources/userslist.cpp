#include "userslist.h"
#include "ui_userslist.h"

UsersList::UsersList(QStringList& users_list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UsersList)
{
    ui->setupUi(this);
    ui->listWidget->addItems(users_list);
}

UsersList::~UsersList()
{
    delete ui;
}


int UsersList::GetUserId() {
    return ui->listWidget->currentRow() + 1;
}


void UsersList::on_pushButtonCancel_clicked() {
    reject();
}


void UsersList::on_pushButtonLock_clicked() {
    accept();
}
