#pragma once

#include "StdAfx.h"
#include "SHA1.h"


// структура хранит связанные с пользователем переменные, а также соответсвующие им параметры для базы данных
struct User {
	Var_with_AddParams <uint>									Id			{ 0,		"id",			SQL_INTEGER };
	Var_with_AddParams <char[MAX_NAME_LENGTH + 1]>				Name		{ "",		"first_name",	SQL_CHAR };	// +1 символ на 0 в конце
	Var_with_AddParams <char[MAX_SURNAME_LENGTH + 1]>			Surname		{ "",		"surname",		SQL_CHAR };	// +1 символ на 0 в конце
	Var_with_AddParams <char[MAX_LOGIN_LENGTH + 1]>				Login		{ "",		"login",		SQL_CHAR };	// +1 символ на 0 в конце
	Var_with_AddParams <char[MAX_EMAIL_LENGTH + 1]>				Email		{ "",		"email",		SQL_CHAR };	// +1 символ на 0 в конце
	Var_with_AddParams <uint>									Hash[SHA1_HASH_LENGTH_UINTS]{
																			{ 0,		"hash1",		SQL_INTEGER },
																			{ 0,		"hash2",		SQL_INTEGER },
																			{ 0,		"hash3",		SQL_INTEGER },
																			{ 0,		"hash4",		SQL_INTEGER },
																			{ 0,		"hash5",		SQL_INTEGER } };
	Var_with_AddParams <char[sizeof(SQL_DATETIME_FORMAT) + 1]>	RegDate		{ "",		"reg_date",		SQL_CHAR };	// +1 символ на 0 в конце
	Var_with_AddParams <char[sizeof(SQL_DATETIME_FORMAT) + 1]>	LastVisit	{ "",		"last_visit",	SQL_CHAR };	// +1 символ на 0 в конце
	Var_with_AddParams <bool>									IsLocked	{ false,	"is_locked",	SQL_BIT };
	Var_with_AddParams <bool>									IsDeleted	{ false,	"is_deleted",	SQL_BIT };
};


//extern User user;


int GetUserId(const std::string& login);	// проверяет соответствие введённого login существующим; возвращает id или код ошибки
int GetLogin(const uint id, std::string& login);	// получает login для указанного по id пользователя; возвращает MSG_SUCCESS_RESULT или код ошибки
int CheckLogin(const std::string& login);	// проверка login на несовпадение с существующими
int CheckHash(const uint id, const SHA1PwdArray& pass_sha1_hash);	// выполняет вход пользователя в чат; возвращает его id или код ошибки
int NewUser(const std::string& name, const std::string& surname, const std::string& login, const std::string& email, SHA1PwdArray& pass_sha1_hash);	// регистрирует нового пользователя; возвращает его id или код ошибки
int ChangeRegData(const uint id, const char* changed_data, const uchar regDataType);
int SetPassword(const uint id, SHA1PwdArray& pass_sha1_hash);	// заменяет хэш пароля пользователя
int SetLastVisit(const uint id);	// обновляет дату и время последнего посещения
int SetIsDeleted(const uint id);	// помечает пользователя как удалённого
int UserInfo(const uint id, std::string& output);	// отображает данных указанного пользователя, возвращает длину строки output
int PrepareListByOneU();	// выводит краткие сведения по указанному пользователю; возвращает длину списка или код ошибки
int ListByOneU(const uint user_index, std::string& output);	// выводит краткие сведения по указанному пользователю; заполняет строку output и возвращает её длину или код ошибки
