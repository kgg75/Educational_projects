#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include "StdAfx.h"

#include "Client.h"
#include "../../_common_files/Constants.h"


QT_BEGIN_NAMESPACE
namespace Ui { class ClientWindow; }
QT_END_NAMESPACE

class Client;

class ClientWindow : public QMainWindow {
    Q_OBJECT

    public:
        ClientWindow(QWidget *parent = nullptr);
        ~ClientWindow();
        void SetStatusText(const char* cchr);
        void SetStatusQText(const QString& qtext);
        void SetWindowQText(const QString& qtext);
        void MessageReceived(const QString& sender, const QString& message);
        void BindServices(const Client* client);

    signals:
        bool IsConnected();
        bool IsLogged();
        int LoginRegister(const bool registration, const QString& login, const QString& name, const QString& surname, const QString& email, const QString& password);
        int Logout();
        int GetUsersList(QStringList& users_list);
        int GetMessagesList(QStringList& messages_list);
        int SendMessage(const uint id, const QString& message);

    private slots:
        void on_menuLogin_triggered();
        void on_menuRegistration_triggered();
        void on_menuLogout_triggered();
        void on_menuClose_triggered();
        void on_menuFont_triggered();
        void on_menuWindowColor_triggered();
        void on_menuFontColor_triggered();
        void on_pushButton_clicked();
        void on_menuAbout_triggered();
        void on_lineEdit_textChanged(const QString &arg1);

private:
        bool ShowLoginDialog(const int tab);
        void setWidgetsState(const bool state);
        Ui::ClientWindow *ui;
        QColor      _color,
                    _fontColor;
        QFont       _font;
};
#endif // CLIENTWINDOW_H
