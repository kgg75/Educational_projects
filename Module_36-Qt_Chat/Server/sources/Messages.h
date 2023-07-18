#pragma once
#include "../../_common_files/Constants.h"


// структура хранит связанные с сообщением переменные, а также соответсвующие им параметры для базы данных
struct Message {
	Var_with_AddParams <uint>									Id				{ 0,		"id",				SQL_INTEGER };
	Var_with_AddParams <uint>									SenderId		{ 0,		"sender_id",		SQL_INTEGER };
	Var_with_AddParams <char[MAX_LOGIN_LENGTH + 1]>				SenderLogin		{ "",		"",					SQL_CHAR };	// не используется в БД
	Var_with_AddParams <uint>									ReceiverId		{ 0,		"receiver_id",		SQL_INTEGER };
	Var_with_AddParams <char[MAX_LOGIN_LENGTH + 1]>				ReceiverLogin	{ "",		"",					SQL_CHAR };	// не используется в БД
	Var_with_AddParams <char[MAX_MESSAGE_LENGTH + 1]>			Text			{ "",		"message",			SQL_CHAR };	// +1 символ на 0 в конце
	Var_with_AddParams <char[sizeof(SQL_DATETIME_FORMAT) + 1]>	DateTime		{ "",		"send_datetime",	SQL_CHAR };	// +1 символ на 0 в конце
	Var_with_AddParams <bool>									IsReaded		{ false,	"is_readed",		SQL_BIT };
	Var_with_AddParams <bool>									IsDelivered		{ false,	"is_delivered",		SQL_BIT };	// не используется
};

