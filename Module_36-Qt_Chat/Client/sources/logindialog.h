#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "StdAfx.h"


namespace Ui { class LoginDialog; }

class LoginDialog : public QDialog {
    Q_OBJECT

    public:
        explicit LoginDialog(const int tab, QWidget *parent = nullptr);
        ~LoginDialog();
        inline const QString& GetLogin() const { return _login; };
        //inline const char* GetLogin() const { return _login.toLocal8Bit(); };
        inline const QString& GetName() const { return _name; };
        //inline const char* GetName() const { return _name.toStdString().c_str(); };
        inline const QString& GetSurname() const { return _surname; };
        inline const QString& GetEmail() const { return _email; };
        inline const QString& GetPassword() const { return _password; };
        inline int GetIndex() const { return _currentIndex; };

    private slots:
        void on_pushButtonCancel_clicked();
        void on_pushButtonOK_clicked();

    private:
        Ui::LoginDialog *ui;
        QMessageBox::StandardButtons SetLogin();
        QMessageBox::StandardButtons SetName();
        QMessageBox::StandardButtons SetSurname();
        QMessageBox::StandardButtons SetEmail();
        QMessageBox::StandardButtons SetPassword();
        QString _login, _name, _surname, _email, _password;
        int _currentIndex;
};

#endif // LOGINDIALOG_H
