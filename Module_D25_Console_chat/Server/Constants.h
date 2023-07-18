#pragma once


typedef unsigned int uint;
typedef unsigned char uchar;


// структура описывает связку команда - подсказка
struct CommandSpace {
	const uchar code;		// код команды
	const char* command;	// текст команды
	const char* command2;	// дублирующий вариант текста команды для отладки - недокументирован в справке!
	const char* help;		// текст справки по команде
};


// структура описывает связку код ошибки - сообщение
struct ErrorSpace {
	const char code;
	const char* text;
};


// структура описывает переменную и свойства связанных с ней столбцов базы данных
template <typename T> struct Var_with_AddParams {
	T value;						// хранимая переменная
	const char* columnName;			// имя соответствующего столбца в базе данных
	const SQLSMALLINT targetType;	// тип данных
};


// коды команд
const uchar
	// серверные
		//SCMD_SAVE =				1,
		SCMD_CON =				2,
		SCMD_USERS =			3,
		SCMD_HELP =				4,
		SCMD_EXIT =				5,
	// глобальные
		GCMD_REG =				20,
		GCMD_LOGIN =			21,
		GCMD_HELP =				22,
		GCMD_EXIT =				23,
	// локальные
		LCMD_SEND =				50,
		LCMD_READ =				51,
		LCMD_READ_ALL =			52,
		LCMD_READ_UNREADED =	53,
		LCMD_USER_INFO =		54,
		LCMD_USERS_LIST =		55,
		LCMD_USER_DELETE =		56,
		LCMD_USER_CHANGE =		57,
		LCMD_HELP =				58,
		LCMD_LOGOUT =			59,
		LCMD_IS_USER_EXIST =	70,
		LCMD_MESSAGES_COUNT =	71,
		LCMD_UNREADED_COUNT =	72,
		LCMD_RECEIVE =			80,
	// сублокальные
		SLCMD_CHANGE_NAME =		90,
		SLCMD_CHANGE_SURNAME =	91,
		SLCMD_CHANGE_LOGIN =	92,
		SLCMD_CHANGE_EMAIL =	93,
		SLCMD_CHANGE_PASSWORD =	94,
		SLCMD_HELP =			95,
		SLCMD_QUIT =			96;


// пространство серверных команд с подсказками
const CommandSpace SERVER_COMMANDS[]{
	//{SCMD_SAVE, "save", "s", "сохранить данные пользователей и сообщений"},
	{SCMD_CON,				"con",		"c",	"вывести список подключений"},
	{SCMD_USERS,			"users",	"u",	"вывести список пользователей"},
	{SCMD_HELP,				"help",		"?",	"вывести справку по командам"},
	{SCMD_EXIT,				"exit",		"e",	"закрыть сервер"},
	{0, "", "", ""}
};


// пространство глобальных команд с подсказками
const CommandSpace GLOBAL_COMMANDS[] {
	{GCMD_REG,				"reg",		"r",	"зарегистрироваться"},
	{GCMD_LOGIN,			"login",	"l+",	"войти в чат"},
	{GCMD_HELP,				"help",		"?",	"вывести справку по командам"},
	{GCMD_EXIT,				"exit",		"e",	"закрыть чат"},
	{0, "", "", ""}
};


// пространство локальных команд с подсказками
const CommandSpace LOCAL_COMMANDS[] {
	{LCMD_SEND,				"send",		"s",	"отправить сообщение"},
	{LCMD_READ,				"read",		"r",	"прочитать сообщения"},
	{LCMD_USER_INFO,		"info",		"i",	"вывести информацию о пользователе"},
	{LCMD_USERS_LIST,		"list",		"l",	"вывести список всех пользователей"},
	{LCMD_USER_DELETE,		"delete",	"del",	"удалить учётную запись пользователя"},
	{LCMD_USER_CHANGE,		"change",	"ch",	"изменить учётные данные пользователя"},
	{LCMD_HELP,				"help",		"?",	"вывести справку по командам"},
	{LCMD_LOGOUT,			"logout",	"l-",	"выйти из чата"},
	{0, "", "", ""}
};


// подпространство локальных команд для изменения регистрационных данных с подсказками
const CommandSpace SUBLOCAL_COMMANDS[] {
	{SLCMD_CHANGE_NAME,		"name",		"n",	"изменить имя пользователя"},
	{SLCMD_CHANGE_SURNAME,	"surname",	"sn",	"изменить фамилию пользователя"},
	{SLCMD_CHANGE_LOGIN,	"login",	"l",	"изменить учётное имя (login) пользователя"},
	{SLCMD_CHANGE_EMAIL,	"email",	"em",	"изменить e-mail пользователя"},
	{SLCMD_CHANGE_PASSWORD,	"pwd",		"p",	"изменить пароль пользователя"},
	{SLCMD_HELP,			"help",		"?",	"вывести справку по командам"},
	{SLCMD_QUIT,			"quit",		"q",	"выйти из этого меню"},
	{0, "", "", ""}
};


// коды ошибок и диагностических сообщений
const char
	MSG_SUCCESS_RESULT =		1,
	MSG_STOP_EXCHANGE =			0,
	ERR_NO_RESULT =				0,
	ERR_USER_NOT_FOUND =		-1,
	ERR_WRONG_PASSWORD =		-2,
	ERR_USER_ALREADY_LOGGED =	-3,
	ERR_LOGIN_ALREADY_EXIST =	-4,
	ERR_TRY_USE_SERVICE_NAME =	-5,
	ERR_WRONG_USER_ID =			-6,
	ERR_REQUEST_ERROR =			-50,	// для использования внутри сервера
	ERR_SERVER_ERROR =			-51;	// для отправки клиенту


const ErrorSpace ERROR_SPACE[]{
	{ERR_USER_NOT_FOUND,		"Пользователь с таким login не существует!"},
	{ERR_WRONG_PASSWORD,		"Неверный пароль!"},
	{ERR_USER_ALREADY_LOGGED,	"Пользователь с таким login уже выполнил вход в чат с другого адреса!"},
	{ERR_LOGIN_ALREADY_EXIST,	"Пользователь с таким login уже зарегистрирован!"},
	{ERR_TRY_USE_SERVICE_NAME,	"Регистрироваться под таким именем нельзя!"},
	{ERR_REQUEST_ERROR,			"Ошибка выполнения запроса к базе данных."},
	{ERR_SERVER_ERROR,			"Ошибка сервера."},
	{0, ""}
};


//определения констант
const int
	PORT_NUMBER =			7777,	// номер порта
	DELAY_MS =				400,	// задержка для сервера в мс
	DELAY_MCS =				DELAY_MS * 1'000;	// задержка для сервера в мкс


const size_t
	MIN_NAME_LENGTH =		2,		// минимальная длина имени пользователя
	MAX_NAME_LENGTH =		16,		// максимальная длина имени пользователя
	MIN_SURNAME_LENGTH =	2,		// минимальная длина имени пользователя
	MAX_SURNAME_LENGTH =	32,		// максимальная длина имени пользователя
	MIN_LOGIN_LENGTH =		2,		// минимальная длина login
	MAX_LOGIN_LENGTH =		32,		// максимальная длина login
	MIN_EMAIL_LENGTH =		5,		// минимальная длина email
	MAX_EMAIL_LENGTH =		32,		// максимальная длина email
	MIN_PASSWORD_LENGTH =	3,		// минимальная длина пароля
	MAX_PASSWORD_LENGTH =	32,		// максимальная длина пароля
	BUFFER_LENGTH =			1024,	// размер буфера для данных
	SQL_BUFFER_LENGTH =		1024,	// размер буфера для SQL-запроса
	MAX_MESSAGE_LENGTH =	512;	// Максимальный размер сообщения


const char
	SERVER_ADDRESS[]		{ "127.0.0.1" },
	CLIENT_STOP_MESSAGE[]	{ "stop" },
	SERVICE_NAME[]			{ "все пользователи" },	// имя служебного пользователя с индексом 0 (не используется)
	SERVICE_LOGIN[]			{ "all" },	// login служебного пользователя с индексом 0 (не используется)
	SEPARATOR[]				{ "-------------------------------------------------------------------------\n" },
	MSG_ERR_CMD[]			{ "Команда не распознана, повторите ввод.\n" },
	DATE_TIME_FORMAT[]		{ "[%Y-%m-%d %H:%M:%S]" },
	SQL_DATETIME_FORMAT[]	{ "[yyyy-MM-dd hh:mm:ss]" },
	USERS_TABLE_NAME[]		{ "users" },	// имя таблицы для хранения данных пользователей
	MSGS_TABLE_NAME[]		{ "messages" },	// имя таблицы для хранения сообщений
	DATABASE_NAME[]			{ "chatDB" };	// имя базы данных
