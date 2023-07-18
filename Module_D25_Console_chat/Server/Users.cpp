// функции для отправки в базу данных запросов по пользователям
#include "StdAfx.h"

#include "Users.h"
#include "Constants.h"
#include "DBFunctions.h"
#include "ServiceFunc.h"


using namespace std;


// Переменная для оперативного хранения данных, связанных с пользователем
User user;


/**************************************
  GetUserId проверяет соответствие введённого учётного имеми (login) существующим в базе данных
  -------------------------------------
  const string& login	- учётное имя (login) пользователя, подлежащее проверке
  -------------------------------------
  Возвращает идентификатор пользователя или код ошибки
  *************************************/

int GetUserId(const string& login) {
	SQLCloseCursor(sqlStmtHandle);
	SQLRETURN sqlResult = SQLExecDirectA(sqlStmtHandle,
		BuildStatement(
		"SELECT ",
			user.Id.columnName,
		" FROM ",
			USERS_TABLE_NAME,
		" WHERE ",
			user.Login.columnName,"='", login.c_str(), "'"
		), SQL_NTS);

	if (sqlResult != SQL_SUCCESS)
		return ERR_REQUEST_ERROR;

	sqlResult = SQLBindCol(sqlStmtHandle, 1, user.Id.targetType, &user.Id.value, sizeof(user.Id.value), nullptr);
	sqlResult = SQLFetch(sqlStmtHandle);

	return (sqlResult == SQL_SUCCESS) ? user.Id.value : ERR_USER_NOT_FOUND;
}


/**************************************
  GetLogin получает учётное имя (login) для указанного по идентификатору пользователя
  -------------------------------------
  const uint id	- идентификатор пользователя
  string& login	- ссылка для записи полученного login
  -------------------------------------
  возвращает MSG_SUCCESS_RESULT или код ошибки
  *************************************/

int GetLogin(const uint id, string& login) {
	SQLCloseCursor(sqlStmtHandle);
	SQLRETURN sqlResult = SQLExecDirectA(sqlStmtHandle,
		BuildStatement(
		"SELECT ",
			user.Login.columnName,
		" FROM ",
			USERS_TABLE_NAME,
		" WHERE ",
			user.Id.columnName, "=", to_string(id).c_str()
		), SQL_NTS);

	if (sqlResult != SQL_SUCCESS)
		return ERR_USER_NOT_FOUND;

	sqlResult = SQLBindCol(sqlStmtHandle, 1, user.Login.targetType, user.Login.value, sizeof(user.Login.value), nullptr);
	sqlResult = SQLFetch(sqlStmtHandle);

	if (sqlResult == SQL_SUCCESS) {
		login = user.Login.value;
		return MSG_SUCCESS_RESULT;
	}
	return 
		ERR_REQUEST_ERROR;
}


/**************************************
  GetHash получает из быза данных hash пароля для указанного по идентификатору пользователя
  -------------------------------------
  const uint id					- идентификатор пользователя
  SHA1PwdArray& hash_from_db	- ссылка на структуру для записи hash
  -------------------------------------
  возвращает MSG_SUCCESS_RESULT или код ошибки
  *************************************/

int GetHash(const uint id, SHA1PwdArray& hash_from_db) {
	SQLRETURN sqlResult;

	sqlResult = SQLCloseCursor(sqlStmtHandle);
	sqlResult = SQLExecDirectA(sqlStmtHandle, BuildStatement(
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
	), SQL_NTS);

	#if _DEBUG
		if (sqlResult == SQL_ERROR) {
			if (SQLGetDiagRecA(SQL_HANDLE_STMT, sqlStmtHandle, 1, &SQLState, &NativeError, ErrorMessageText, SQL_BUFFER_LENGTH, &ErrorTextLength) != SQL_SUCCESS)
				cout << ErrorMessageText << '\n';
		}
	#endif

	if (sqlResult == SQL_SUCCESS) {
		for (auto i = 0; i < SHA1_HASH_LENGTH_UINTS; i++) {
			sqlResult = SQLBindCol(sqlStmtHandle, i + 1, user.Hash[i].targetType, &hash_from_db[i], sizeof(uint), nullptr);
			#if _DEBUG 
			if (sqlResult == SQL_ERROR) {
				if (SQLGetDiagRecA(SQL_HANDLE_STMT, sqlStmtHandle, 1, &SQLState, &NativeError, ErrorMessageText, SQL_BUFFER_LENGTH, &ErrorTextLength) != SQL_SUCCESS)
					cout << ErrorMessageText << '\n';
			}
			#endif
		}
		sqlResult = SQLFetch(sqlStmtHandle);
	}

	return (sqlResult == SQL_SUCCESS) ? MSG_SUCCESS_RESULT : ERR_REQUEST_ERROR;
}


/**************************************
  CheckLogin проверяет указанное учётное имя (login) на отсутствие совпадения с недостимыми
  и уже имеющимися в базе данных именами
  -------------------------------------
  const string& login	- учётное имя (login) пользователя, подлежащее проверке
  -------------------------------------
  возвращает MSG_SUCCESS_RESULT или код ошибки
  *************************************/

int CheckLogin(const string& login) {
	if (lowercase_s(login) == SERVICE_LOGIN)	// попытка регистрации под служебным login
		return ERR_TRY_USE_SERVICE_NAME;

	if (GetUserId(login) > 0)
		return ERR_LOGIN_ALREADY_EXIST;

	return MSG_SUCCESS_RESULT;
}


/**************************************
  CheckHash выполняет проверку указанного hash пароля пользователя на соответствие его хранимому в базе данных
  -------------------------------------
  const uint id					- идентификатор пользователя
  SHA1PwdArray& hash_from_db	- ссылка на структуру для записи hash
  -------------------------------------
  возвращает его id или код ошибки
  *************************************/

int CheckHash(const uint id, const SHA1PwdArray& pass_sha1_hash) {
	if (id == 0)	//  0 - пользователь не найден
		return ERR_WRONG_USER_ID;

	SHA1PwdArray hash_from_db;
	if (GetHash(id, hash_from_db) == MSG_SUCCESS_RESULT) {
		if (pass_sha1_hash == hash_from_db)
			return id;
	}

	return ERR_WRONG_PASSWORD;
}


/**************************************
  NewUser создаёт в базе данных нового пользователя
  -------------------------------------
  const string& name		- имя пользователя
  const string& surname		- фамилия пользователя
  const string& login		- учётное имя (login) пользователя
  const string& email		- e-mail пользователя
  SHA1PwdArray& pass_sha1_hash	- ссылка на структуру для записи hash
  -------------------------------------
  Возвращает id нового пользователя или код ошибки
  *************************************/

int NewUser(const string& name, const string& surname, const string& login, const string& email, SHA1PwdArray& pass_sha1_hash) {
	int iResult = CheckLogin(login);

	if (iResult == MSG_SUCCESS_RESULT) {
		SQLRETURN sqlResult;

		sqlResult = SQLCloseCursor(sqlStmtHandle);
		sqlResult = SQLExecDirectA(sqlStmtHandle, BuildStatement(
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
		), SQL_NTS);

		if (sqlResult == SQL_SUCCESS) {	// получаем id пользователя
			sqlResult = SQLCloseCursor(sqlStmtHandle);
			sqlResult = SQLExecDirectA(sqlStmtHandle, BuildStatement(
				"SELECT ",
					"MAX(", user.Id.columnName, ")"
				" FROM ",
					USERS_TABLE_NAME
			), SQL_NTS);

			SQLLEN		row_count;
			SQLSMALLINT	col_count;

			SQLNumResultCols(sqlStmtHandle, &col_count);
			SQLRowCount(sqlStmtHandle, &row_count);

			if (sqlResult == SQL_SUCCESS && col_count == 1 && row_count == 1) {	// 1 - число столбцов в выполненном выше запросе
				sqlResult = SQLBindCol(sqlStmtHandle, 1, user.Id.targetType, &user.Id.value, sizeof(user.Id.value), nullptr);
				if (sqlResult == SQL_SUCCESS && SQLFetch(sqlStmtHandle) == SQL_SUCCESS)
					return user.Id.value;	// возвращаем id пользователя
			}
		}
	}
	return ERR_REQUEST_ERROR;
}


/**************************************
  ChangeRegData изменяет в базе данных данные указанного по идентификатору пользователю
  -------------------------------------
  const uint id				- идентификатор пользователя
  const char* changed_data	- указатель на стоку с новыми данными
  const uchar regDataType	- код типа изменяемых данных
  -------------------------------------
  Возвращает MSG_SUCCESS_RESULT или код ошибки
  *************************************/

int ChangeRegData(const uint id, const char* changed_data, const uchar regDataType) {	// изменение учётной записи пользователя
	int iResult;
	const char* ptrColumnName;

	switch (regDataType) {
		case SLCMD_CHANGE_NAME:
			ptrColumnName = user.Name.columnName;
			break;
		case SLCMD_CHANGE_SURNAME:
			ptrColumnName = user.Surname.columnName;
			break;
		case SLCMD_CHANGE_LOGIN:
			iResult = CheckLogin(changed_data);

			if (iResult != MSG_SUCCESS_RESULT)
				return iResult;
			
			ptrColumnName = user.Login.columnName;
			break;
		case SLCMD_CHANGE_EMAIL:
			ptrColumnName = user.Email.columnName;
			break;
		default:
			return ERR_REQUEST_ERROR;
			break;
	}

	SQLRETURN sqlResult;
	sqlResult = SQLExecDirectA(sqlStmtHandle, BuildStatement(
		"UPDATE ",
			USERS_TABLE_NAME,
		" SET ",
			ptrColumnName, "='", changed_data, "'"
		" WHERE ",
			user.Id.columnName, "=", to_string(id).c_str()
	), SQL_NTS);

	return (sqlResult == SQL_SUCCESS) ? MSG_SUCCESS_RESULT : ERR_REQUEST_ERROR;
}


/**************************************
  SetPassword заменяет в базе данных hash пароля пользователя
  -------------------------------------
  const uint id					- идентификатор пользователя
  SHA1PwdArray& pass_sha1_hash	- ссылка на структуру для записи hash
  -------------------------------------
  Возвращает MSG_SUCCESS_RESULT или код ошибки
  *************************************/

int SetPassword(const uint id, SHA1PwdArray& pass_sha1_hash) {
	SQLRETURN sqlResult;
	sqlResult = SQLCloseCursor(sqlStmtHandle);
	sqlResult = SQLExecDirectA(sqlStmtHandle, BuildStatement(
		"UPDATE ",
			USERS_TABLE_NAME,
		" SET ",
			user.Hash[0].columnName, "=", to_string(pass_sha1_hash[0]).c_str(), ",",
			user.Hash[1].columnName, "=", to_string(pass_sha1_hash[1]).c_str(), ",",
			user.Hash[2].columnName, "=", to_string(pass_sha1_hash[2]).c_str(), ",",
			user.Hash[3].columnName, "=", to_string(pass_sha1_hash[3]).c_str(), ",",
			user.Hash[4].columnName, "=", to_string(pass_sha1_hash[4]).c_str(),
		" WHERE ",
			user.Id.columnName, "=", to_string(id).c_str()
	), SQL_NTS);

#if _DEBUG
	if (sqlResult == SQL_ERROR) {
		if (SQLGetDiagRecA(SQL_HANDLE_STMT, sqlStmtHandle, 1, &SQLState, &NativeError, ErrorMessageText, SQL_BUFFER_LENGTH, &ErrorTextLength) != SQL_SUCCESS)
			cout << ErrorMessageText << '\n';
	}
#endif

	return (sqlResult == SQL_SUCCESS) ? MSG_SUCCESS_RESULT : ERR_SERVER_ERROR;
}


/**************************************
  SetLastVisit обновляет в базе данных дату и время последнего посещения
  -------------------------------------
  const uint id				- идентификатор пользователя
  -------------------------------------
  Возвращает MSG_SUCCESS_RESULT или код ошибки
  *************************************/

int SetLastVisit(const uint id) {
	SQLRETURN sqlResult;
	sqlResult = SQLExecDirectA(sqlStmtHandle, BuildStatement(
		"UPDATE ",
			USERS_TABLE_NAME,
		" SET ",
			user.LastVisit.columnName, "=CURRENT_TIMESTAMP"
		" WHERE ",
			user.Id.columnName, "=", to_string(id).c_str()
	), SQL_NTS);

	return (sqlResult == SQL_SUCCESS) ? MSG_SUCCESS_RESULT : ERR_REQUEST_ERROR;
}


/**************************************
  SetIsDeleted помечает в базе данных пользователя как удалённого
  -------------------------------------
  const uint id			- идентификатор пользователя
  -------------------------------------
  Возвращает MSG_SUCCESS_RESULT или код ошибки
  *************************************/

int SetIsDeleted(const uint id) {
	SQLRETURN sqlResult;
	sqlResult = SQLExecDirectA(sqlStmtHandle, BuildStatement(
		"UPDATE ",
			USERS_TABLE_NAME,
		" SET ",
			user.IsDeleted.columnName, "='1'"
		" WHERE ",
			user.Id.columnName, "=", to_string(id).c_str()
	), SQL_NTS);

	return (sqlResult == SQL_SUCCESS) ? MSG_SUCCESS_RESULT : ERR_REQUEST_ERROR;
}


/**************************************
  UserInfo отображает информацию по указанному по id пользователю
  -------------------------------------
  const uint id			- идентификатор пользователя
  string& output		- строка для записи информации
  -------------------------------------
  Возвращает длину строки output или код ошибки
  *************************************/

int UserInfo(const uint id, string& output) {
	SQLRETURN sqlResult;

	sqlResult = SQLCloseCursor(sqlStmtHandle);
	sqlResult = SQLExecDirectA(sqlStmtHandle, BuildStatement(
		"SELECT ",
		user.Name.columnName, ",",
		user.Surname.columnName, ",",
		user.Login.columnName, ",",
		user.Email.columnName, ",",
		user.RegDate.columnName,
		" FROM ",
		USERS_TABLE_NAME,
		" WHERE ",
		user.Id.columnName, "=", to_string(id).c_str()
	), SQL_NTS);

	SQLLEN		row_count;
	SQLSMALLINT	col_count;

	SQLNumResultCols(sqlStmtHandle, &col_count);
	SQLRowCount(sqlStmtHandle, &row_count);

	if (sqlResult == SQL_SUCCESS && col_count == 5 && row_count == 1) {	// 5 - число столбцов в выполненном выше запросе
		sqlResult = SQLBindCol(sqlStmtHandle, 1, user.Name.targetType, user.Name.value, MAX_NAME_LENGTH, nullptr);
		sqlResult = SQLBindCol(sqlStmtHandle, 2, user.Surname.targetType, user.Surname.value, MAX_SURNAME_LENGTH, nullptr);
		sqlResult = SQLBindCol(sqlStmtHandle, 3, user.Login.targetType, user.Login.value, MAX_LOGIN_LENGTH, nullptr);
		sqlResult = SQLBindCol(sqlStmtHandle, 4, user.Email.targetType, user.Email.value, MAX_EMAIL_LENGTH, nullptr);
		sqlResult = SQLBindCol(sqlStmtHandle, 5, user.RegDate.targetType, user.RegDate.value, sizeof(SQL_DATETIME_FORMAT), nullptr);
		sqlResult = SQLFetch(sqlStmtHandle);

		output = "             id: " + to_string(id) +
			"\n            имя: " + (string)user.Name.value +
			"\n        фамилия: " + (string)user.Surname.value +
			"\n          login: " + (string)user.Login.value +
			"\n         e-mail: " + (string)user.Email.value +
			"\n  дата регистр.: " + (string)user.RegDate.value + '\n';
		return output.length();
	}
	else
		return ERR_REQUEST_ERROR;
}


/**************************************
  PrepareListByOneU подготавливает выполнение функции ListByOneU
  -------------------------------------
  -------------------------------------
  Возвращает количество неудалённых пользователей или код ошибки
  *************************************/

int PrepareListByOneU() {
	SQLRETURN sqlResult;

	sqlResult = SQLCloseCursor(sqlStmtHandle);
	sqlResult = SQLExecDirectA(sqlStmtHandle, BuildStatement(
		"SELECT ",
			user.Id.columnName, ",",
			user.Name.columnName, ",",
			user.Surname.columnName, ",",
			user.Login.columnName, ",",
			user.Email.columnName, ",",
			user.RegDate.columnName, ",",
			user.LastVisit.columnName,
		" FROM ",
			USERS_TABLE_NAME,
		" WHERE ",
			user.IsDeleted.columnName, "=0"
	), SQL_NTS);

	SQLLEN		row_count;
	SQLSMALLINT	col_count;

	SQLNumResultCols(sqlStmtHandle, &col_count);
	SQLRowCount(sqlStmtHandle, &row_count);

	if (sqlResult == SQL_SUCCESS && col_count == 7 && row_count > 0) {	// 7 - число столбцов в выполненном выше запросе
		sqlResult = SQLBindCol(sqlStmtHandle, 1, user.Id.targetType, &user.Id.value, sizeof(user.Id.value), nullptr);
		sqlResult = SQLBindCol(sqlStmtHandle, 2, user.Name.targetType, user.Name.value, MAX_NAME_LENGTH, nullptr);
		sqlResult = SQLBindCol(sqlStmtHandle, 3, user.Surname.targetType, user.Surname.value, MAX_SURNAME_LENGTH, nullptr);
		sqlResult = SQLBindCol(sqlStmtHandle, 4, user.Login.targetType, user.Login.value, MAX_LOGIN_LENGTH, nullptr);
		sqlResult = SQLBindCol(sqlStmtHandle, 5, user.Email.targetType, user.Email.value, MAX_EMAIL_LENGTH, nullptr);
		sqlResult = SQLBindCol(sqlStmtHandle, 6, user.RegDate.targetType, user.RegDate.value, sizeof(SQL_DATETIME_FORMAT), nullptr);
		sqlResult = SQLBindCol(sqlStmtHandle, 7, user.LastVisit.targetType, user.LastVisit.value, sizeof(SQL_DATETIME_FORMAT), nullptr);

		return row_count;
	}
	else
		return ERR_REQUEST_ERROR;
}


/**************************************
  ListByOneU заполняет строку output краткими сведениями по
  неудалённым пользователям, которые были получены в предшествующем вызове PrepareListByOneU(),
  -------------------------------------
  const uint user_index		- порядковый номер возвращаемой строки
  string& output			- строка для записи информации
  -------------------------------------
  Возвращает длину строку output или ERR_NO_RESULT
  *************************************/

int ListByOneU(const uint user_index, string& output) {
	while (SQLFetch(sqlStmtHandle) != SQL_NO_DATA) {
		output = "   " + to_string(user_index) +
			") #" + to_string(user.Id.value) +
			" " + (string)user.Name.value +
			"\t" + (string)user.Surname.value +
			"\t" + (string)user.Login.value +
			"\t" + (string)user.Email.value +
			"\t" + (string)user.RegDate.value + 
			"\t" + (string)user.LastVisit.value + '\n';
		return output.length();
	}

	return ERR_NO_RESULT;
}
