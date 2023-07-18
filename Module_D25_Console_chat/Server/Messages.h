#pragma once
#include "Constants.h"


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


extern Message message;


int AddMessageToDB(const uint senderId, const uint receiverId, const std::string& text);	// добавляет сообщение в базу данных и возвращает его id или код ошибки
int SetReadedState(const uint receiverId, const uint messageId = 0, const bool MarkAsReaded = true);	// устанавливает состояние флага "прочитан" для всех или одного сообщений указанного пользователя
uint GetMessagesCount(const uint receiverId, const bool showUnReadedOnly = true);	// подсчитывает число сообщений для указанного получателя
int PrepareListByOneM(const uint receiverId, const bool showUnReadedOnly = true);	// подготавливает выполнение функции ListByOneM; возвращает количество сообщений или код ошибки
int ListByOneM(const uint user_index, std::string& output);	// возвращает длину строки output или код ошибки
