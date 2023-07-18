#include "clientwindow.h"
#include "logindialog.h"
#include "aboutdialog.h"
#include "ui_clientwindow.h"


//ClientWindow public:
ClientWindow::ClientWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::ClientWindow) {
    ui->setupUi(this);
    ui->comboBoxUsers->addItem(QString::fromLocal8Bit("Все пользователи"));
    ui->comboBoxUsers->setCurrentIndex(-1);

}


ClientWindow::~ClientWindow() {
    delete ui;
}


void ClientWindow::SetStatusText(const char* cchr) {
    statusBar()->showMessage(QString::fromLocal8Bit(cchr));
}


void ClientWindow::SetStatusQText(const QString& qtext) {
    statusBar()->showMessage(qtext);
}


void ClientWindow::SetWindowQText(const QString& qtext) {
    setWindowTitle("Chat" + qtext);
}


void ClientWindow::MessageReceived(const QString& sender, const QString& message) {
    ui->incomingMsgs->append("<" + sender + "> " + message);
}


void ClientWindow::BindServices(const Client* client) {
    connect(client, &Client::StatusChanged, this, &ClientWindow::SetStatusQText);
    connect(client, &Client::WindowTextChanged, this, &ClientWindow::SetWindowQText);
    connect(client, &Client::MessageReceived, this, &ClientWindow::MessageReceived);
    connect(this, &ClientWindow::IsConnected, client, &Client::IsConnected);
    connect(this, &ClientWindow::IsLogged, client, &Client::IsLogged);
    connect(this, &ClientWindow::LoginRegister, client, &Client::LoginRegister);
    connect(this, &ClientWindow::Logout, client, &Client::Logout);
    connect(this, &ClientWindow::GetUsersList, client, &Client::GetUsersList);
    connect(this, &ClientWindow::GetMessagesList, client, &Client::GetMessagesList);
    connect(this, &ClientWindow::SendMessage, client, &Client::SendMessage);
}


//ClientWindow private slots:
void ClientWindow::on_menuLogin_triggered() {
    ShowLoginDialog(0);
    setWidgetsState(emit IsLogged());
}


void ClientWindow::on_menuRegistration_triggered() {
    ShowLoginDialog(1);
}


void ClientWindow::on_menuLogout_triggered() {
    if (emit IsLogged()) {
        if (emit Logout() == MSG_SUCCESS_RESULT)
            setWidgetsState(false);
    }
}


void ClientWindow::on_menuFont_triggered() {
    bool bResult;
    _font = QFontDialog::getFont(&bResult, QFont("Arial", 10), this);
    if (bResult) {
        ui->lineEdit->setFont(_font);
        ui->incomingMsgs->setFont(_font);
        ui->outcomingMsgs->setFont(_font);
        this->update();
    }
}


void ClientWindow::on_menuWindowColor_triggered() {
    _color = QColorDialog::getColor(_color, this, "Выбор цвета окна");
    if (_color.isValid()) {
        QPalette p;
        p = ui->lineEdit->palette();
        p.setColor(QPalette::Base, _color);
        ui->lineEdit->setPalette(p);
        p = ui->incomingMsgs->palette();
        p.setColor(QPalette::Base, _color);
        ui->incomingMsgs->setPalette(p);
        p = ui->outcomingMsgs->palette();
        p.setColor(QPalette::Base, _color);
        ui->outcomingMsgs->setPalette(p);
        this->update();
    }
}


void ClientWindow::on_menuFontColor_triggered() {
    _fontColor = QColorDialog::getColor(_fontColor, this, "Выбор цвета шрифта");
    if (_fontColor.isValid()) {
        QPalette p;
        p = ui->lineEdit->palette();
        p.setColor(QPalette::Text, _fontColor);
        ui->lineEdit->setPalette(p);
        p = ui->incomingMsgs->palette();
        p.setColor(QPalette::Text, _fontColor);
        ui->incomingMsgs->setPalette(p);
        p = ui->outcomingMsgs->palette();
        p.setColor(QPalette::Text, _fontColor);
        ui->outcomingMsgs->setPalette(p);
        this->update();
    }
}


void ClientWindow::on_menuClose_triggered() {
    QMessageBox qmBox(QMessageBox::Warning,
                      QString::fromLocal8Bit("Предупреждение"),
                      QString::fromLocal8Bit("Вы действительно хотите закрыть чат?"),
                      QMessageBox::Ok | QMessageBox::Cancel,
                      this,
                      Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    if (qmBox.exec() == QMessageBox::Ok)
        close();
}


void ClientWindow::on_pushButton_clicked() {
    if (ui->comboBoxUsers->currentIndex() == -1) {
        QMessageBox qmBox(QMessageBox::Warning,
                          QString::fromLocal8Bit("Предупреждение"),
                          QString::fromLocal8Bit("Выберите получателя сообщения!"),
                          QMessageBox::Ok,
                          this,
                          Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
        qmBox.exec();
    }
    else {
        ui->outcomingMsgs->append("<" + ui->comboBoxUsers->currentText() + "> " + ui->lineEdit->text());

        int iResult;
        emit iResult = SendMessage(ui->comboBoxUsers->currentIndex(), ui->lineEdit->text());
        if (iResult == MSG_SUCCESS_RESULT)
            statusBar()->showMessage(QString::fromLocal8Bit("Сообщение отправлено"));
    }
}


void ClientWindow::on_menuAbout_triggered() {
    AboutDialog aboutDialog;
    aboutDialog.exec();
}


bool ClientWindow::ShowLoginDialog(const int tab) {
    LoginDialog loginDialog(tab);
    bool bResult = (loginDialog.exec() == QDialog::Accepted);

    if (bResult) {
        if ((loginDialog.GetIndex()) == 0 && (emit IsLogged())) {
            QMessageBox qmBox(QMessageBox::Warning,
                              QString::fromLocal8Bit("Предупреждение"),
                              QString::fromLocal8Bit("Вы действительно хотите войти под другим именем?"),
                              QMessageBox::Cancel | QMessageBox::Ok,
                              this,
                              Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

            if (qmBox.exec() == QMessageBox::Cancel)
                return false;

            Logout();
            setWidgetsState(emit IsLogged());
        }

        int iResult;
        emit iResult = LoginRegister(loginDialog.GetIndex() == 1,
                                       loginDialog.GetLogin(),
                                       loginDialog.GetName(),
                                       loginDialog.GetSurname(),
                                       loginDialog.GetEmail(),
                                       loginDialog.GetPassword());

        if (iResult == MSG_SUCCESS_RESULT) {
            SetStatusQText(QString::fromLocal8Bit("Выполнил вход пользователь '") +
                          loginDialog.GetLogin() + "'");
            SetWindowQText(" @" + loginDialog.GetLogin());

            QStringList users_list;
            if (emit GetUsersList(users_list) == MSG_SUCCESS_RESULT) {
                ui->comboBoxUsers->clear();
                ui->comboBoxUsers->addItem(QString::fromLocal8Bit(SERVICE_NAME));
                ui->comboBoxUsers->addItems(users_list);
            }

            setWidgetsState(true);

            QStringList messages_list;
            if (emit GetMessagesList(messages_list) == MSG_SUCCESS_RESULT) {
                for (auto it : messages_list)
                    ui->incomingMsgs->append(it);
            }
        }
        else if (iResult == ERR_USER_ALREADY_LOGGED) {
            QMessageBox qmBox(QMessageBox::Critical,
                              QString::fromLocal8Bit("Ошибка"),
                              QString::fromLocal8Bit("Пользователь ") +
                              loginDialog.GetLogin() +
                              QString::fromLocal8Bit(" уже вошёл в чат!"),
                              QMessageBox::Ok, 0, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);    //QMessageBox::AcceptRole | QMessageBox::RejectRole, QMessageBox::Warning, QMessageBox::Ok | QMessageBox::Cancel, 0);
            qmBox.exec();
        }
    }

    return bResult;
}


void ClientWindow::setWidgetsState(const bool state) {
    ui->comboBoxUsers->setEnabled(state);
    ui->lineEdit->setEnabled(state);
    ui->incomingMsgs->setEnabled(state);
    ui->outcomingMsgs->setEnabled(state);
    //ui->pushButton->setEnabled(state);
    if (!state) {
        ui->comboBoxUsers->setCurrentIndex(-1);
        ui->lineEdit->clear();
        ui->incomingMsgs->clear();
        ui->outcomingMsgs->clear();
    }
}


void ClientWindow::on_lineEdit_textChanged(const QString &arg1) {
    ui->pushButton->setEnabled(!ui->lineEdit->text().isEmpty());
}
