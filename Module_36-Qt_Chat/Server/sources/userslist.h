#ifndef USERSLIST_H
#define USERSLIST_H

#include "StdAfx.h"

namespace Ui { class UsersList; }

class UsersList : public QDialog {
    Q_OBJECT

    public:
        explicit UsersList(QStringList& users_list, QWidget *parent = nullptr);
        ~UsersList();
        int GetUserId();

    private slots:
        void on_pushButtonCancel_clicked();
        void on_pushButtonLock_clicked();
    private:
        Ui::UsersList *ui;
};

#endif // USERSLIST_H
