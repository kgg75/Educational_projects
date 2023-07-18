#pragma once


constexpr auto SQL_RESULT_LEN = 240;
constexpr auto SQL_RETURN_CODE_LEN = 1024;


extern SQLHSTMT	sqlStmtHandle;  // дескриптор для выполнения запросов к базе данных


// переменные для функции SQLGetDiagRecA - возвращает текущие значения нескольких полей диагностической записи,
// содержащей сведения об ошибке, предупреждении и состоянии.
#if _DEBUG 
	extern SQLCHAR		SQLState;
	extern SQLINTEGER	NativeError;
	extern SQLCHAR		ErrorMessageText[];
	extern SQLSMALLINT	ErrorTextLength;
#endif


int MySQLSuccess(SQLRETURN rc);
int Initialize_ODBC();
int Close_ODBC();