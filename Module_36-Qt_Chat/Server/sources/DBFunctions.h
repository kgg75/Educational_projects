#pragma once

#include "StdAfx.h"
#include "serverwindow.h"

#include "Users.h"
#include "Messages.h"
#include "../../_common_files/Constants.h"

//constexpr auto SQL_RESULT_LEN = 240;
//constexpr auto SQL_RETURN_CODE_LEN = 1024;

//class ServerWindow;

// "����������" ����� const char[] ��� �������� � SQL-������
template <typename ... Args>
QString QBuildStatement(const Args* ...args) {
    static char buffer[SQL_BUFFER_LENGTH];
    uint	len, index = 0;
    (
        (
            len = strlen(args),		// ��������� ����� ���������� ��������� args
            memcpy(&buffer[index], args, len),   // �������� � ����� ������ ������ �� ������ ���������� args
            index += len	// ������������� ������
        ),
        ...   // ���������� ������ ���������� args  - C++17
    );

    buffer[index] = 0;	// ������������� 0 � ����� ������

    return (QString)buffer;
}


class Database : public QObject {
    Q_OBJECT

    public:
        Database(QObject* parent);
        ~Database();
        bool open();

        User user;          // ��� ���������� �������� � ������ �������
        Message message;    // ��� ���������� �������� � ������ �������

    public slots:
        int GetLogin(const uint id, std::string& qtext);
        int UserRegister(const std::string& login, const std::string& name, const std::string& surname, const std::string& email, SHA1PwdArray& pass_sha1_hash/*, int* pResult = nullptr*/);
        int GetUserId(const std::string& qstr/*, int* pResult = nullptr*/);
        int CheckHash(const uint id, /*const */SHA1PwdArray& pass_sha1_hash/*, int* pResult = nullptr*/);
        int SetLastVisit(const uint id);
        int SetReadedState(const uint receiverId, const uint messageId = 0, const bool MarkAsReaded = true);
        int GetUsersList(QStringList& users_list);
        int GetUsersDetailList(QStringList& users_list);
        int GetMessagesList(QStringList& messages_list, const uint receiverId, const bool showUnReadedOnly = true);
        int GetMessagesFullList(QStringList& messages_list);
        int AddMessage(const uint senderId, const uint receiverId, const std::string& text);
        int SetIsLocked(const uint id);

    private slots:


    signals:
        void atDBStart(const QString& qstr);

    private:
        int GetHash(const uint id, SHA1PwdArray& hash_from_db);
        int CheckLogin(const std::string& login);
        QSqlDatabase    _qdb;
};
