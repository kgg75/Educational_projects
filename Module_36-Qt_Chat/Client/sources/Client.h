#pragma once
#include "StdAfx.h"

#include "clientwindow.h"
#include "../../_common_files/Constants.h"
#include "../../_common_files/SHA1.h"


class Client : public QObject {
        Q_OBJECT
    public:
        /*explicit*/ Client(/*QObject* parent*/);
        ~Client();

    public slots:
        bool ConnectToServer();
        bool IsConnected() { return _isConnected; };
        bool IsLogged() { return _isLogged; };
        int receiveData();
        int sendData();
        void RequestHandler();
        int LoginRegister(const bool registration, const QString& login, const QString& name, const QString& surname, const QString& email, const QString& password);
        int Logout();
        int GetUsersList(QStringList& users_list);
        int GetMessagesList(QStringList& messages_list);
        int SendMessage(const uint id, const QString& message);

    signals:
        void StatusChanged(const QString& qtext);
        void WindowTextChanged(const QString& qtext);
        void MessageReceived(const QString& sender, const QString& message);

    private:
        int MessagesExchange(/*const uchar command_code*/);
        QTcpSocket      _socket;
        QString         _login = "";

        bool            _isConnected = false,
                        _isLogged = false;

        QByteArray		_request_msg,
                        _response_msg;

};
