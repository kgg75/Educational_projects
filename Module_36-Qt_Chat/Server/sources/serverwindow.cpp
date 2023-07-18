#include "StdAfx.h"

#include "serverwindow.h"
#include "aboutdialog.h"
#include "userslist.h"
#include "ui_serverwindow.h"

//#include "Server.h"
#include "../../_common_files/ServiceFunc.h"
#include "../../_common_files/Constants.h"


ServerWindow::ServerWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::ServerWindow) {
    ui->setupUi(this);
    ui->textEdit->document()->setMetaInformation(QTextDocument::MetaInformation(0), "UTF-8");
}


ServerWindow::~ServerWindow() {
    delete ui;
}


void ServerWindow::atInitBegin() {
    ui->textEdit->insertPlainText(QString::fromLocal8Bit("---=== Чат-сервер версия 3.0b ===---\n"));
    ui->textEdit->insertPlainText(SEPARATOR);
}


void ServerWindow::atInitEnd() {
    ui->textEdit->insertPlainText(SEPARATOR);
    ui->textEdit->insertPlainText(QString::fromLocal8Bit("Сервер запущен ") + qGetTime() + '\n');
    ui->textEdit->insertPlainText(SEPARATOR);
}


void ServerWindow::qtext_out(const QString& qstr) {
    ui->textEdit->insertPlainText(qstr);
}


void ServerWindow::text_out(const char* str) {
    ui->textEdit->insertPlainText(QString::fromLocal8Bit(str));
}


void ServerWindow::BindServices(const Server* server) {
    connect(server, &Server::atNetStart, this, &ServerWindow::qtext_out);  // для получения результата подключения к сети
    connect(server, &Server::NewMessage, this, &ServerWindow::qtext_out);  // для выводы сообщений сервера
    connect(this, &ServerWindow::GetConnections, server, &Server::PutConnections); // для запроса списка подключения
    connect(server, &Server::StatusChanged, this, &ServerWindow::SetStatusText);    // для изменения строки статуса
}


void ServerWindow::BindServices(const Database* database) {
    connect(database, &Database::atDBStart, this, &ServerWindow::qtext_out);  // для получения результата подключения к БД
    connect(this, &ServerWindow::GetUsersList_fromDB, database, &Database::GetUsersList);
    connect(this, &ServerWindow::GetUsersDetailList_fromDB, database, &Database::GetUsersDetailList);
    connect(this, &ServerWindow::GetMessagesFullList_fromDB, database, &Database::GetMessagesFullList);
    connect(this, &ServerWindow::SetIsLocked_inDB, database, &Database::SetIsLocked);
}


void ServerWindow::BindServices(const Server* server, const Database* database) {
    connect(server, &Server::AddUser_toDB, database, &Database::UserRegister);  // для получения результата подключения к БД
    connect(server, &Server::GetUserId_fromDB, database, &Database::GetUserId);  // для получения id пользователя
    connect(server, &Server::CheckHash_withDB, database, &Database::CheckHash);  // для проверки пароля
    connect(server, &Server::SetLastVisit_toDB, database, &Database::SetLastVisit);  // для установки времени последнего посещения
    connect(server, &Server::SetReadedState_toDB, database, &Database::SetReadedState);  // для установки времени последнего посещения
    connect(server, &Server::GetLogin_fromDB, database, &Database::GetLogin);  // для возврата login
    connect(server, &Server::GetUsersList_fromDB, database, &Database::GetUsersList);
    connect(server, &Server::GetMessagesList_fromDB, database, &Database::GetMessagesList);
    connect(server, &Server::AddMessage_toDB, database, &Database::AddMessage);
}


void ServerWindow::SetStatusText(const QString& qstr) {
    statusBar()->showMessage(qstr);
}


// private slots:
void ServerWindow::on_pushBtnConnections_clicked() {
    ui->textEdit->insertPlainText(SEPARATOR);

    QStringList connections_list;
    int iResult;
    emit iResult = GetConnections(connections_list);
    if (iResult > 0) {
        ui->textEdit->insertPlainText(QString::fromLocal8Bit("Список подключений:\nсокет - учётное имя\n"));
        for (auto it : connections_list)
            ui->textEdit->insertPlainText(it);
    }
    else
        ui->textEdit->insertPlainText(QString::fromLocal8Bit("Нет подключений.\n"));

    ui->textEdit->insertPlainText(SEPARATOR);

    statusBar()->showMessage(QString::fromLocal8Bit("Подключений: ") + QString::number(iResult));
}


void ServerWindow::on_pushBtnUsers_clicked() {
    ui->textEdit->insertPlainText(SEPARATOR);

    QStringList users_list;
    int iResult;
    emit iResult = GetUsersDetailList_fromDB(users_list);
    if (iResult > 0) {
        ui->textEdit->insertPlainText(QString::fromLocal8Bit("Список пользователей:\nid - login - имя, фамилия - e-mail - [дата рег.] - [посл. посещение] - заблокирован? - удалён?\n"));
        for (auto it : users_list)
            ui->textEdit->insertPlainText(it);
    }
    else
        ui->textEdit->insertPlainText(QString::fromLocal8Bit("Нет зарегистрированных пользователей."));

    ui->textEdit->insertPlainText(SEPARATOR);

}

void ServerWindow::on_pushBtnMessages_clicked() {
    ui->textEdit->insertPlainText(SEPARATOR);

    QStringList messages_list;
    int iResult;
    emit iResult = GetMessagesFullList_fromDB(messages_list);
    if (iResult > 0) {
        ui->textEdit->insertPlainText(QString::fromLocal8Bit("Список сообщений:\nid [дата] <отправитель> текст\n"));
        for (auto it : messages_list)
            ui->textEdit->insertPlainText(it);
    }
    else
        ui->textEdit->insertPlainText(QString::fromLocal8Bit("Список сообщений пуст."));

    ui->textEdit->insertPlainText(SEPARATOR);

}


void ServerWindow::on_menuFont_triggered() {
    bool bResult;
    font = QFontDialog::getFont(&bResult, QFont("Arial", 10), this);
    if (bResult) {
        ui->textEdit->setFont(font);
        //ui->textEdit->append(font.family());
        ui->textEdit->update();
    }
}


void ServerWindow::on_menuWindowColor_triggered() {
    color = QColorDialog::getColor(color, this, "Выбор цвета окна");
    if (color.isValid()) {
        QPalette p = ui->textEdit->palette();
        p.setColor(QPalette::Base, color);
        ui->textEdit->setPalette(p);
        ui->textEdit->update();
    }
}


void ServerWindow::on_menuFontColor_triggered() {
    fontColor = QColorDialog::getColor(fontColor, this, "Выбор цвета шрифта");
    if (fontColor.isValid()) {
        QPalette p = ui->textEdit->palette();
        p.setColor(QPalette::Text, fontColor);
        ui->textEdit->setPalette(p);
        ui->textEdit->update();
    }
}


void ServerWindow::on_menuExit_triggered() {
    QMessageBox qmBox(QMessageBox::Warning,
                      QString::fromLocal8Bit("Предупреждение"),
                      QString::fromLocal8Bit("Вы действительно хотите закрыть сервер чата?"),
                      QMessageBox::Ok | QMessageBox::Cancel,
                      this,
                      Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);    //QMessageBox::AcceptRole | QMessageBox::RejectRole, QMessageBox::Warning, QMessageBox::Ok | QMessageBox::Cancel, 0);

    if (qmBox.exec() == QMessageBox::Ok)
        close();
}


void ServerWindow::on_menuAbout_triggered() {
    AboutDialog aboutDialog;
    aboutDialog.exec();
}


void ServerWindow::on_menuClear_triggered() {
    ui->textEdit->clear();
}


void ServerWindow::on_menuConnections_triggered() {
    on_pushBtnConnections_clicked();
}


void ServerWindow::on_menuUsers_triggered() {
    on_pushBtnUsers_clicked();
}


void ServerWindow::on_menuMessages_triggered() {
    on_pushBtnMessages_clicked();
}

void ServerWindow::on_menuLock_triggered() {
    QStringList users_list;
    emit GetUsersList_fromDB(users_list);
    UsersList userlist(users_list);

    bool bResult = (userlist.exec() == QDialog::Accepted);
    if (bResult) {
        uint id = userlist.GetUserId();
        int iResult;
        emit iResult = SetIsLocked_inDB(id);
        if (iResult == MSG_SUCCESS_RESULT)
            ui->textEdit->insertPlainText(QString::fromLocal8Bit("Заблокирован пользователь #") + QString::number(id) +'\n');
    }
}
