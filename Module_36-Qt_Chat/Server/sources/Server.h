#pragma once

//#include <map>

#include "serverwindow.h"
#include "../../_common_files/Constants.h"
#include "../../_common_files/SHA1.h"


class Server : public QObject {
    Q_OBJECT

    public:
        Server(QObject* parent);
        ~Server();
        int Initialize(/*ServerWindow* pServerWindow*/);

    public slots:
        int PutConnections(QStringList& connections_list);

    private slots:
        void newConnection();
        void disconnected();
        int ReadRequest();  //QTcpSocket* socket = nullptr);
        int SendResponse(QTcpSocket* socket);

    signals:
        void atNetStart(const QString& qstr);
        void NewMessage(const QString& qstr);
        void StatusChanged(const QString& qstr);
        int AddUser_toDB(const std::string& login, const std::string& name, const std::string& surname, const std::string& email, SHA1PwdArray& pass_sha1_hash);
        int GetUserId_fromDB(const std::string& qstr);
        int CheckHash_withDB(const uint id, /*const */SHA1PwdArray& pass_sha1_hash);
        int SetLastVisit_toDB(const uint id);
        int SetReadedState_toDB(const uint receiverId, const uint messageId = 0, const bool MarkAsReaded = true);
        int GetLogin_fromDB(const uint id, std::string& login);
        int GetUsersList_fromDB(QStringList& users_list);
        int GetMessagesList_fromDB(QStringList& messages_list, const uint receiverId, const bool showUnReadedOnly);
        int AddMessage_toDB(const uint senderId, const uint receiverId, const std::string& message);

    private:
        void RequestHandler(QTcpSocket* socket);
        QTcpServer      _server;
        std::map <QTcpSocket*, uint> _users_map;	// связка сокет - идентификатор (id) пользователя

        QByteArray  _request_msg,
                    _response_msg;

        uint        _request_msg_length,
                    _response_msg_length;
};

