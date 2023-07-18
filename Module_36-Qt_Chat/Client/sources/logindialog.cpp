#include "logindialog.h"
#include "ui_logindialog.h"


LoginDialog::LoginDialog(const int tab, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(tab);
}


LoginDialog::~LoginDialog() {
    delete ui;
}


QMessageBox::StandardButtons LoginDialog::SetLogin() {
    if (ui->tabWidget->currentIndex() == 0)   // login
        _login = ui->lineEditLogin->text();
    else    // registration
        _login = ui->lineEditRegLogin->text();
    return QMessageBox::Ok;
}


QMessageBox::StandardButtons LoginDialog::SetName() {
    _name = ui->lineEditName->text();
    return QMessageBox::Ok;
}


QMessageBox::StandardButtons LoginDialog::SetSurname() {
    _surname = ui->lineEditSurname->text();
    return QMessageBox::Ok;
}


QMessageBox::StandardButtons LoginDialog::SetEmail() {
    _email = ui->lineEditEmail->text();
    return QMessageBox::Ok;
}


QMessageBox::StandardButtons LoginDialog::SetPassword() {
    if (ui->tabWidget->currentIndex() == 0)   // login
        _password = ui->lineEditPassword->text();
    else {    // registration
        if (ui->lineEditRegPassword->text() == ui->lineEditConfirmRegPassword->text())
            _password = ui->lineEditRegPassword->text();
        else {
            QMessageBox qmBox(QMessageBox::Critical,
                              QString::fromLocal8Bit("ќшибка!"),
                              QString::fromLocal8Bit("ѕароли не совпадают!"),
                              QMessageBox::Cancel | QMessageBox::Retry,
                              0,
                              Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

            return (QMessageBox::StandardButtons)qmBox.exec();    //  QMessageBox::Cancel или QMessageBox::Retry
        }
    }
    return QMessageBox::Ok;
}


void LoginDialog::on_pushButtonCancel_clicked() {
    reject();
}


void LoginDialog::on_pushButtonOK_clicked() {
    _currentIndex = ui->tabWidget->currentIndex();
    if ((SetLogin() && SetPassword())
        &&
        (   (_currentIndex == 0)   // при login проверки далее не выполн€ютс€
            ||
            (
                (SetName() && SetSurname() && SetEmail())   // дополнительные проверки при регистрации
            )
        )) {
            accept();
    }
}
