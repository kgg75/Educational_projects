// функции для подключения к базе данных, проверке таблиц, закрытии бвзы данных
#include "StdAfx.h"

#include "DBFunctions.h"
#include "Constants.h"
#include "Users.h"
#include "Messages.h"
#include "ServiceFunc.h"


using namespace std;


SQLHENV     sqlEnvHandle {nullptr}; // дескриптор окружения базы данных
SQLHDBC     sqlConnHandle {nullptr}; // дескриптор для соединения с базой данных
SQLHSTMT    sqlStmtHandle {nullptr};  // дескриптор для выполнения запросов к базе данных
SQLCHAR     retconstring[SQL_RETURN_CODE_LEN] {}; // строка для кода возврата из функций API ODBC
SQLRETURN   sqlResult;


// переменные для функции SQLGetDiagRecA - возвращает текущие значения нескольких полей диагностической записи,
// содержащей сведения об ошибке, предупреждении и состоянии.
#if _DEBUG 
    SQLCHAR		SQLState;
    SQLINTEGER	NativeError;
    SQLCHAR		ErrorMessageText[SQL_BUFFER_LENGTH];
    SQLSMALLINT	ErrorTextLength;
#endif


// simple helper functions  
inline int MySQLSuccess(SQLRETURN rc) { return (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO); }


// проверяет существование таблиц, в случае отсутствия создаёт их
int CheckTables() {
    cout << "Открытие таблицы пользователей...";

    if (MySQLSuccess(SQLExecDirectA(sqlStmtHandle, BuildStatement("SELECT * FROM ", USERS_TABLE_NAME, " LIMIT 1"), SQL_NTS))) {
        cout << " выполнено.\n";
    } else {
        cout << "\nПредупреждение: таблица пользователей отсутствует!\n";
        if (MySQLSuccess(sqlResult = SQLExecDirectA(sqlStmtHandle,
            BuildStatement(
                "CREATE TABLE IF NOT EXISTS ",
                    USERS_TABLE_NAME,
                "(",
                    user.Id.columnName, " SERIAL PRIMARY KEY,",
                    user.Name.columnName, " VARCHAR(", to_string(MAX_NAME_LENGTH).c_str() ,") NOT NULL,",
                    user.Surname.columnName, " VARCHAR(", to_string(MAX_SURNAME_LENGTH).c_str(), ") NOT NULL,",
                    user.Login.columnName, " VARCHAR(", to_string(MAX_LOGIN_LENGTH).c_str(), ") UNIQUE NOT NULL,",
                    user.Email.columnName, " VARCHAR(", to_string(MAX_EMAIL_LENGTH).c_str(), "),",
                    user.Hash[0].columnName, " INT UNSIGNED,",
                    user.Hash[1].columnName, " INT UNSIGNED,",
                    user.Hash[2].columnName, " INT UNSIGNED,",
                    user.Hash[3].columnName, " INT UNSIGNED,",
                    user.Hash[4].columnName, " INT UNSIGNED,",
                    user.RegDate.columnName, " DATE,",
                    user.LastVisit.columnName, " DATETIME,",
                    user.IsLocked.columnName, " BOOL DEFAULT false,",
                    user.IsDeleted.columnName, " BOOL DEFAULT false"
                ")"
            ), SQL_NTS))) {
            cout << "Создана новая таблица пользователей.\n";
        }
        else {
            cout << "Ошибка создания новой таблицы пользователей!\n";
            return sqlResult;
        }
    }

    cout << "Открытие таблицы сообщений...";
 
    if (MySQLSuccess(SQLExecDirectA(sqlStmtHandle, BuildStatement("SELECT * FROM ", MSGS_TABLE_NAME, " LIMIT 1"), SQL_NTS))) {
        cout << " выполнено.\n";
    } else {
        cout << "\nПредупреждение: таблица сообщений отсутствует!\n";
        if (MySQLSuccess(sqlResult = SQLExecDirectA(sqlStmtHandle,
            BuildStatement(
                "CREATE TABLE IF NOT EXISTS ",
                    MSGS_TABLE_NAME,
                "(",
                    message.Id.columnName, " SERIAL PRIMARY KEY, ",
                    message.SenderId.columnName, " INT UNSIGNED NOT null references ", USERS_TABLE_NAME, "(", user.Id.columnName, "),",
                    message.ReceiverId.columnName, " INT UNSIGNED NOT null references ", USERS_TABLE_NAME, "(", user.Id.columnName, "),",
                    message.Text.columnName, " VARCHAR(", to_string(MAX_MESSAGE_LENGTH).c_str(), ") NOT NULL,",
                    message.DateTime.columnName, " DATETIME,",
                    message.IsReaded.columnName, " BOOL DEFAULT false,",
                    message.IsDelivered.columnName, " BOOL DEFAULT false"
                ")"
            ), SQL_NTS))) {
            cout << "Создана новая таблица сообщений.\n";
        }
        else {
            cout << "Ошибка создания новой таблицы сообщений!\n";
            return sqlResult;
        }
    }

    return sqlResult;
}


// Устанавливает соединение с сервером
int Initialize_ODBC() {
    cout << "Подключение к SQL-серверу...";

    // Allocate environment handle  
    if (MySQLSuccess(SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnvHandle))) {   // выделяем дескриптор для базы данных
        // Set the ODBC version environment attribute
        if (MySQLSuccess(SQLSetEnvAttr(sqlEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0))) { // устанавливаем атрибут (??)
            // Allocate connection handle  
            if (MySQLSuccess(SQLAllocHandle(SQL_HANDLE_DBC, sqlEnvHandle, &sqlConnHandle))) {    // выделяем дескриптор для соединения с базой данных
                SQLSetConnectAttr(sqlConnHandle, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);  // Set login timeout to 5 seconds  
                
                if (MySQLSuccess(SQLDriverConnectA(sqlConnHandle, GetDesktopWindow(),   // подключаемся к базе данных
                    (SQLCHAR*)"DRIVER={MySQL ODBC 8.0 ANSI Driver};"
                    "SERVER=localhost;"
                    "PORT=3306;"
                    //"DATABASE=chatDB;"
                    "UID=root;"
                    "PWD=root;",
                    SQL_NTS,
                    retconstring,
                    1024,
                    NULL,
                    SQL_DRIVER_COMPLETE))) {
                    
                    if (MySQLSuccess(SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle))) { // Allocate statement handle  
                        cout << " выполнено.\n";
                        
                        sqlResult = SQLExecDirectA(sqlStmtHandle, BuildStatement("CREATE DATABASE IF NOT EXISTS ", DATABASE_NAME), SQL_NTS);
                        sqlResult = SQLExecDirectA(sqlStmtHandle, BuildStatement("USE ", DATABASE_NAME), SQL_NTS);
                        if (MySQLSuccess(CheckTables()))
                            return 0;   // инициализация сервера БД выполнена успешно  
                        else
                            return sqlResult;
                    }
                    else {
                        cout << "ошибка при выделении дескриптора для выполнения запросов к базе данных.\n";
                        SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
                        SQLDisconnect(sqlConnHandle);
                    }
                }
                else {
                    cout << "ошибка при подключении к базе данных.\n";
                    SQLFreeHandle(SQL_HANDLE_DBC, sqlConnHandle);
                }
            }
            else {
                cout << "ошибка при выделении дескриптора для соединения с базой данных.\n";
            }
        }
        else {
            cout << "ошибка при установке атрибута.\n";
            SQLFreeHandle(SQL_HANDLE_ENV, sqlEnvHandle);
        }
    }
    else {
        cout << "ошибка при выделении дескриптора для базы данных.\n";
    }
    return SQL_ERROR;
}


// Закрывает соединение и дескрипторы
int Close_ODBC() {
    SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
    SQLDisconnect(sqlConnHandle);
    SQLFreeHandle(SQL_HANDLE_DBC, sqlConnHandle);
    SQLFreeHandle(SQL_HANDLE_ENV, sqlEnvHandle);

    return 0;
}