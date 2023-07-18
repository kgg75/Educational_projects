// ������� ��� ����������� � ���� ������, �������� ������, �������� ���� ������

#include "DBFunctions.h"
#include "../../_common_files/Constants.h"
#include "../../_common_files/ServiceFunc.h"


using namespace std;


Database::Database(QObject* parent) {
    _qdb = QSqlDatabase::addDatabase(DATABASE_TYPE);
    _qdb.setDatabaseName(DATABASE_NAME);
    _qdb.setHostName(SERVER_ADDRESS);
    //_qdb.setUserName(DB_USER);
    //_qdb.setPassword(DB_PASSWORD);
}


Database::~Database() {
    _qdb.close();
    _qdb.removeDatabase(DATABASE_NAME);
}


bool Database::open() {
    bool bResult = _qdb.open();
    if (!bResult) {
        emit atDBStart(_qdb.lastError().text() + '\n');
        return false;
    }

    QSqlQuery query(_qdb);
    bResult = query.exec(QBuildStatement(
                    "CREATE TABLE IF NOT EXISTS ",
                        USERS_TABLE_NAME,
                    "(",
                        user.Id.columnName, " INTEGER PRIMARY KEY,",
                        user.Name.columnName, " VARCHAR(", to_string(MAX_NAME_LENGTH).c_str() ,") NOT NULL,",
                        user.Surname.columnName, " VARCHAR(", to_string(MAX_SURNAME_LENGTH).c_str(), ") NOT NULL,",
                        user.Login.columnName, " VARCHAR(", to_string(MAX_LOGIN_LENGTH).c_str(), ") UNIQUE NOT NULL,",
                        user.Email.columnName, " VARCHAR(", to_string(MAX_EMAIL_LENGTH).c_str(), "),",
                        user.Hash[0].columnName, " UNSIGNED INTEGER,",
                        user.Hash[1].columnName, " UNSIGNED INTEGER,",
                        user.Hash[2].columnName, " UNSIGNED INTEGER,",
                        user.Hash[3].columnName, " UNSIGNED INTEGER,",
                        user.Hash[4].columnName, " UNSIGNED INTEGER,",
                        user.RegDate.columnName, " DATE,",
                        user.LastVisit.columnName, " DATETIME,",
                        user.IsLocked.columnName, " BOOL DEFAULT false,",
                        user.IsDeleted.columnName, " BOOL DEFAULT false"
                    ")"
                ));
    if (!bResult) {
        QSqlError error = query.lastError();
        emit atDBStart(error.text() + '\n');
        return false;
    }

    bResult = query.exec(QBuildStatement(
                    "CREATE TABLE IF NOT EXISTS ",
                        MSGS_TABLE_NAME,
                    "(",
                        message.Id.columnName, " INTEGER PRIMARY KEY, ",
                        message.SenderId.columnName, " UNSIGNED INTEGER NOT null references ", USERS_TABLE_NAME, "(", user.Id.columnName, "),",
                        message.ReceiverId.columnName, " UNSIGNED INTEGER references ", USERS_TABLE_NAME, "(", user.Id.columnName, "),",    // NOT null
                        message.Text.columnName, " VARCHAR(", to_string(MAX_MESSAGE_LENGTH).c_str(), ") NOT NULL,",
                        message.DateTime.columnName, " DATETIME,",
                        message.IsReaded.columnName, " BOOL DEFAULT false,",
                        message.IsDelivered.columnName, " BOOL DEFAULT false"
                    ")"
                ));
    if (!bResult) {
        QSqlError error = query.lastError();
        emit atDBStart(error.text() + '\n');
        return false;
    }

    emit atDBStart(QString::fromLocal8Bit("���������.\n"));
    return true;
}


/**************************************
  GetUserId ��������� ������������ ��������� �������� ����� (login) ������������ � ���� ������
  -------------------------------------
  const string& login	- ������� ��� (login) ������������, ���������� ��������
  -------------------------------------
  ���������� ������������� ������������ ��� ��� ������
  *************************************/

int Database::GetUserId(const string& login/*, int* pResult*/) {
    QSqlQuery query(_qdb);
    int iResult;

    bool bResult = query.exec(QBuildStatement(
        "SELECT ",
            user.Id.columnName,
        " FROM ",
            USERS_TABLE_NAME,
        " WHERE ",
            user.Login.columnName,"='", login.c_str(), "'"
        ));

    if (!bResult)
        iResult = ERR_USER_NOT_FOUND;
    else {
        if (query.next())
            iResult = query.value(0).toInt();
        else
            iResult = ERR_REQUEST_ERROR;
    }

    return iResult;
}


/**************************************
  CheckLogin ��������� ��������� ������� ��� (login) �� ���������� ���������� � �����������
  � ��� ���������� � ���� ������ �������
  -------------------------------------
  const string& login	- ������� ��� (login) ������������, ���������� ��������
  -------------------------------------
  ���������� MSG_SUCCESS_RESULT ��� ��� ������
  *************************************/

int Database::CheckLogin(const string& login) {
    if (lowercase_s(login) == SERVICE_NAME)	// ������� ����������� ��� ��������� login
        return ERR_TRY_USE_SERVICE_NAME;

    if (GetUserId(login) > 0)
        return ERR_LOGIN_ALREADY_EXIST;

    return MSG_SUCCESS_RESULT;
}


/**************************************
  UserRegister ������ � ���� ������ ������ ������������
  -------------------------------------
  const string& login		- ������� ��� (login) ������������
  const string& name		- ��� ������������
  const string& surname		- ������� ������������
  const string& email		- e-mail ������������
  const SHA1PwdArray& pass_sha1_hash	- ������ �� ��������� ��� ������ hash
  -------------------------------------
  ���������� id ������ ������������ ��� ��� ������
  *************************************/

int Database::UserRegister(const string& login, const string& name, const string& surname, const string& email, SHA1PwdArray& pass_sha1_hash/*, int* pResult*/) {
    int iResult = CheckLogin(login);

    if (iResult == MSG_SUCCESS_RESULT) {
        QSqlQuery query(_qdb);
        bool bResult = query.exec(QBuildStatement(
            "INSERT INTO ",
                USERS_TABLE_NAME,
            "(",
                user.Name.columnName, ",",
                user.Surname.columnName, ",",
                user.Login.columnName, ",",
                user.Email.columnName, ",",
                user.Hash[0].columnName, ",",
                user.Hash[1].columnName, ",",
                user.Hash[2].columnName, ",",
                user.Hash[3].columnName, ",",
                user.Hash[4].columnName, ",",
                user.RegDate.columnName, ",",
                user.LastVisit.columnName,
            ") "
            "VALUES ('",
                name.c_str(), "','",
                surname.c_str(), "','",
                login.c_str(), "','",
                email.c_str(), "',",
                to_string(pass_sha1_hash[0]).c_str(), ",",
                to_string(pass_sha1_hash[1]).c_str(), ",",
                to_string(pass_sha1_hash[2]).c_str(), ",",
                to_string(pass_sha1_hash[3]).c_str(), ",",
                to_string(pass_sha1_hash[4]).c_str(), ",",
                "CURRENT_DATE,"
                "CURRENT_TIMESTAMP"
            ")"
        ));

        if (bResult) {	// �������� id ������������
            bResult = query.exec(QBuildStatement(
                "SELECT ",
                    "MAX(", user.Id.columnName, ")"
                " FROM ",
                    USERS_TABLE_NAME
            ));

            if (bResult && query.next())
                iResult = query.value(0).toInt();	// ���������� id ������������
            else
                iResult = ERR_REQUEST_ERROR;
        }
    }
    else
        iResult = ERR_REQUEST_ERROR;

    return iResult;
}


/**************************************
  GetHash �������� �� ���� ������ hash ������ ��� ���������� �� �������������� ������������
  -------------------------------------
  const uint id					- ������������� ������������
  SHA1PwdArray& hash_from_db	- ������ �� ��������� ��� ������ hash
  -------------------------------------
  ���������� MSG_SUCCESS_RESULT ��� ��� ������
  *************************************/

int Database::GetHash(const uint id, SHA1PwdArray& hash_from_db) {
    QSqlQuery query(_qdb);

    bool bResult = query.exec(QBuildStatement(
        "SELECT ",
            user.Hash[0].columnName, ",",
            user.Hash[1].columnName, ",",
            user.Hash[2].columnName, ",",
            user.Hash[3].columnName, ",",
            user.Hash[4].columnName,
        " FROM ",
            USERS_TABLE_NAME,
        " WHERE ",
            user.Id.columnName, "=", to_string(id).c_str()
    ));

    if (bResult && query.next()) {
        for (auto i = 0; i < SHA1_HASH_LENGTH_UINTS; i++) {
            hash_from_db[i] = query.value(i).toUInt();
        }
    }

    return bResult ? MSG_SUCCESS_RESULT : ERR_REQUEST_ERROR;
}


/**************************************
  CheckHash ��������� �������� ���������� hash ������ ������������ �� ������������ ��� ��������� � ���� ������
  -------------------------------------
  const uint id					- ������������� ������������
  SHA1PwdArray& hash_from_db	- ������ �� ��������� ��� ������ hash
  -------------------------------------
  ���������� ��� id ��� ��� ������
  *************************************/

int Database::CheckHash(const uint id, /*const */SHA1PwdArray& pass_sha1_hash/*, int* pResult*/) {
    int iResult;

    if (id == 0)	//  0 - ������������ �� ������
        iResult = ERR_WRONG_USER_ID;
    else {
        SHA1PwdArray hash_from_db;
        if (GetHash(id, hash_from_db) == MSG_SUCCESS_RESULT) {
            iResult = (pass_sha1_hash == hash_from_db) ? id : ERR_WRONG_PASSWORD;
        }
        else
            iResult = ERR_REQUEST_ERROR;
    }

    return iResult;
}


/**************************************
  SetLastVisit ��������� � ���� ������ ���� � ����� ���������� ���������
  -------------------------------------
  const uint id				- ������������� ������������
  -------------------------------------
  ���������� MSG_SUCCESS_RESULT ��� ��� ������
  *************************************/

int Database::SetLastVisit(const uint id) {
    QSqlQuery query(_qdb);

    bool bResult = query.exec(QBuildStatement(
        "UPDATE ",
            USERS_TABLE_NAME,
        " SET ",
            user.LastVisit.columnName, "=CURRENT_TIMESTAMP"
        " WHERE ",
            user.Id.columnName, "=", to_string(id).c_str()
    ));

    return bResult ? MSG_SUCCESS_RESULT : ERR_REQUEST_ERROR;
}


/**************************************
  SetReadedState ������������� ��������� ����� "��������" ��� ���� ��� ������ ��������� ���������� ������������
  -------------------------------------
  const uint senderId		- ������������� �����������
  const uint messageId		- ������������� ���������; �� ��������� 0
  const bool MarkAsReaded	- ��������� ����� "��������" ��� ���������; �� ��������� false
  -------------------------------------
  ���� ������������� ��������� �� ������ (�� ��������� ��������������� �������� 0),
  �� ������� ������������� ���� MarkAsReaded ��� ���� ��������� ������������.
  -------------------------------------
  ���������� MSG_SUCCESS_RESULT ��� ��� ������
  *************************************/

int Database::SetReadedState(const uint receiverId, const uint messageId, const bool MarkAsReaded) {
    QSqlQuery query(_qdb);

    bool bResult = query.exec(QBuildStatement(
    "UPDATE ",
        MSGS_TABLE_NAME,
    " SET ",
        message.IsReaded.columnName, (MarkAsReaded ? "='1'" : "='0'"),
    " WHERE ",
        (messageId == 0 ? message.ReceiverId.columnName : message.Id.columnName),
        "=",
        to_string((messageId == 0 ? receiverId : messageId)).c_str()
    ));

    return bResult ? MSG_SUCCESS_RESULT : ERR_REQUEST_ERROR;
}


/**************************************
  GetLogin �������� ������� ��� (login) ��� ���������� �� �������������� ������������
  -------------------------------------
  const uint id - ������������� ������������
  string& login	- ������ ��� ������ ����������� login
  -------------------------------------
  ���������� MSG_SUCCESS_RESULT ��� ��� ������
  *************************************/

int Database::GetLogin(const uint id, std::string& login) {
    QSqlQuery query(_qdb);
    int iResult = MSG_SUCCESS_RESULT;

    bool bResult = query.exec(QBuildStatement(
          "SELECT ",
              user.Login.columnName,
          " FROM ",
              USERS_TABLE_NAME,
          " WHERE ",
              user.Id.columnName, "=", to_string(id).c_str()
    ));

    if (!bResult)
        iResult = ERR_USER_NOT_FOUND;
    else {
        if (query.next())
            login = query.value(0).toString().toStdString();
        else
            iResult = ERR_REQUEST_ERROR;
    }

    return iResult;
}


/**************************************
  GetUsersList ���������� ������ �������������
  -------------------------------------
  QStringList& users_list - ������ �� ����������� ������ �������������
  -------------------------------------
  ���������� MSG_SUCCESS_RESULT ��� ��� ������
  *************************************/

int Database::GetUsersList(QStringList& users_list) {
    QSqlQuery query(_qdb);
    int iResult = MSG_SUCCESS_RESULT;

    bool bResult = query.exec(QBuildStatement(
        "SELECT ",
            user.Login.columnName,
        " FROM ",
            USERS_TABLE_NAME,
        " ORDER BY ",
          user.Id.columnName
    ));

    if (!bResult)
        iResult = ERR_REQUEST_ERROR;
    else {
        while (query.next()) {
            users_list.append(query.value(0).toString());
        }
    }

    return iResult;
}


int Database::GetUsersDetailList(QStringList& users_list) {
    QSqlQuery query(_qdb);
    int iResult = MSG_SUCCESS_RESULT;

    bool bResult = query.exec(QBuildStatement(
        "SELECT ",
            user.Id.columnName, ",",
            user.Login.columnName, ",",
            user.Name.columnName, ",",
            user.Surname.columnName, ",",
            user.Email.columnName, ",",
            user.RegDate.columnName, ",",
            user.LastVisit.columnName, ",",
            user.IsLocked.columnName, ",",
            user.IsDeleted.columnName,
        " FROM ",
            USERS_TABLE_NAME,
        " ORDER BY ",
            user.Id.columnName
    ));

    if (!bResult)
        iResult = ERR_REQUEST_ERROR;
    else {
        while (query.next()) {
            users_list.append(query.value(0).toString() + " - " +
                            query.value(1).toString() + " - " +
                            query.value(2).toString() + " " +
                            query.value(3).toString() + " - " +
                            query.value(4).toString() + " - [" +
                            query.value(5).toString() + "] - [" +
                            query.value(6).toString() + "] - " +
                            query.value(7).toString() + " - " +
                            query.value(8).toString() + '\n');
        }
    }

    return iResult;
}


int Database::GetMessagesList(QStringList& messages_list, const uint receiverId, const bool showUnReadedOnly) {
    QSqlQuery query(_qdb);
    int iResult = MSG_SUCCESS_RESULT;

    bool bResult = query.exec(QBuildStatement(
        "SELECT ",
            MSGS_TABLE_NAME, ".", message.Id.columnName, ",",
            message.DateTime.columnName, ",",
            user.Login.columnName, ",",
            message.Text.columnName,
        " FROM ",
            MSGS_TABLE_NAME, " JOIN ", USERS_TABLE_NAME,
        " ON ",
            USERS_TABLE_NAME, ".", user.Id.columnName, "=", MSGS_TABLE_NAME, ".", message.SenderId.columnName,
        " WHERE ",
            message.ReceiverId.columnName, "=", to_string(receiverId).c_str(),
        " AND ",
            message.IsReaded.columnName, (showUnReadedOnly ? "=0" : "<=1"),
        " ORDER BY ",
            message.DateTime.columnName
    )); // �������� ����� ����������

    if (!bResult)
        iResult = ERR_REQUEST_ERROR;
    else {
        while (query.next()) {
            messages_list.append("#" +
                                query.value(0).toString() + "[" +
                                query.value(1).toString() +
                                QString::fromLocal8Bit("] �� <") +
                                query.value(2).toString() + ">: " +
                                query.value(3).toString());
        }
    }

    return iResult;
}


int Database::GetMessagesFullList(QStringList& messages_list) {
    QSqlQuery query(_qdb);
    int iResult = MSG_SUCCESS_RESULT;

    bool bResult = query.exec(QBuildStatement(
        "SELECT ",
            MSGS_TABLE_NAME, ".", message.Id.columnName, ",",
            message.DateTime.columnName, ",",
            user.Login.columnName, ",",
            message.Text.columnName,
        " FROM ",
            MSGS_TABLE_NAME, " JOIN ", USERS_TABLE_NAME,
        " ON ",
            USERS_TABLE_NAME, ".", user.Id.columnName, "=", MSGS_TABLE_NAME, ".", message.SenderId.columnName,
        " ORDER BY ",
            message.DateTime.columnName
    )); // �������� ����� ����������

    if (!bResult)
        iResult = ERR_USER_NOT_FOUND;
    else {
        while (query.next()) {
            messages_list.append(query.value(0).toString() + " [" +
                                 query.value(1).toString() + "] <" +
                                 query.value(2).toString() + "> " +
                                 query.value(3).toString() + '\n');
        }
    }

    return iResult;
}


int Database::AddMessage(const uint senderId, const uint receiverId, const std::string& text) {
    QSqlQuery query(_qdb);
    int iResult = MSG_SUCCESS_RESULT;

    bool bResult = query.exec(QBuildStatement(
        "INSERT INTO ",
            MSGS_TABLE_NAME,
            "(",
            message.SenderId.columnName, ",",
            message.ReceiverId.columnName, ",",
            message.Text.columnName, ",",
            message.DateTime.columnName,
            ") "
        "VALUES (",
            to_string(senderId).c_str(), ",",
            to_string(receiverId).c_str(), ",'",
            text.c_str(),
            "', CURRENT_TIMESTAMP"
        ")"
    ));

    if (!bResult)
        iResult = ERR_REQUEST_ERROR;
    else {
        while (query.next()) {
            }
    }

    return iResult;
}



/**************************************
  SetIsLocked �������� � ���� ������ ������������ ��� ����������������
  -------------------------------------
  const uint id			- ������������� ������������
  -------------------------------------
  ���������� MSG_SUCCESS_RESULT ��� ��� ������
  *************************************/

int Database::SetIsLocked(const uint id) {
    QSqlQuery query(_qdb);

    bool bResult = query.exec(QBuildStatement(
        "UPDATE ",
            USERS_TABLE_NAME,
        " SET ",
            user.IsLocked.columnName, "='1'"
        " WHERE ",
            user.Id.columnName, "=", to_string(id).c_str()
    ));

    return bResult ? MSG_SUCCESS_RESULT : ERR_REQUEST_ERROR;
}
