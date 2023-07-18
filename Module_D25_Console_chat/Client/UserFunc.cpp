#include "StdAfx.h"

#include "UserFunc.h"
#include "../Server/ServiceFunc.h"


using namespace std;


bool SetName(string& name) {	// выполняет ввод и проверку имени
	cout << "Имя (длина " << MIN_NAME_LENGTH << "-" << MAX_NAME_LENGTH << " символов, пробелы не допускаются): ";
	name = get_string_from_console();
	return (check_name(name, "имя", MIN_NAME_LENGTH, MAX_NAME_LENGTH) && check_spaces(name, "имя"));
}


bool SetSurname(string& surname) {	// выполняет ввод и проверку фамилии
	cout << "Фамилия (длина " << MIN_SURNAME_LENGTH << "-" << MAX_SURNAME_LENGTH << " символов, пробелы не допускаются): ";
	surname = get_string_from_console();
	return (check_name(surname, "фамилия", MIN_SURNAME_LENGTH, MAX_SURNAME_LENGTH) && check_spaces(surname, "фамилия"));
}


bool SetLogin(string& login) {	// выполняет ввод и проверку login
	cout << "Учётное имя (login) (длина " << MIN_LOGIN_LENGTH << "-" << MAX_LOGIN_LENGTH << " символов, пробелы не допускаются): ";
	login = get_string_from_console();
	return (check_name(login, "login", MIN_LOGIN_LENGTH, MAX_LOGIN_LENGTH) && check_spaces(login, "login"));	// проверка синтаксиса
}


bool SetEmail(string& email) {	// выполняет ввод и проверку email
	cout << "E-mail (длина " << MIN_EMAIL_LENGTH << "-" << MAX_EMAIL_LENGTH << " символов, пробелы не допускаются): ";
	email = get_string_from_console();
	return (check_name(email, "e-mail", MIN_EMAIL_LENGTH, MAX_EMAIL_LENGTH) && check_spaces(email, "e-mail"));	// проверка синтаксиса
}


bool SetPassword(string& password) {	// выполняет ввод и проверку пароля
	cout << "Пароль: ";
	password = get_value_from_console();
	if (!check_name(password, "пароль", MIN_PASSWORD_LENGTH, MAX_PASSWORD_LENGTH) || !check_spaces(password, "пароль")) {
		password = "";
		return false;
	}
	cout << "Повторите пароль: ";
	string password2 = get_value_from_console();
	if (password != password2) {
		cout << "Пароли не совпадают!\n";
		password = "";
		return false;
	}

	return true;
}


bool NewUser(string& name, string& surname, string& login, string& email, string& password) {	// собирает сведения для регистрации нового пользователя
	cout << "Для регистрации введите следующие данные пользователя (пробелы не допускаются):\n";
	return (SetName(name) && SetSurname(surname) && SetLogin(login) && SetEmail(email) && SetPassword(password));
}


bool SignIn(string& login, string& password) {	// собирает сведения для входа пользователя в чат
	cout << "Укажите условное имя (login): ";
	cin >> login;
	if (!check_empty_name(login, "имя"))
		return false;

	cout << "Укажите пароль: ";
	cin >> password;

	return true;
}


bool ChooseUser(string& receiver) {	// выбирает получателя для отправки сообщения
	cout << "Укажите условное имя (login) получателя: ";
	cin >> receiver;
	return (check_empty_name(receiver, "имя"));
}


bool AddMessage(string& text) {	// добавляет сообщение
	cout << "Введите текст сообщения: ";
	text = get_string_from_console();
	return (check_name(text, "сообщение", 1, MAX_MESSAGE_LENGTH));
}

bool SetRegData(string& changed_data, const uchar regDataType) {	// выполняет запрос с консоли новых регистрационных данных
	bool bResult = false;

	switch (regDataType) {
		case SLCMD_CHANGE_NAME:	// локальная команда name - изменить имя пользователя
			bResult = SetName(changed_data);
			break;
		case SLCMD_CHANGE_SURNAME:	// локальная команда surname - изменить фамилию пользователя
			bResult = SetSurname(changed_data);
			break;
		case SLCMD_CHANGE_LOGIN:	// локальная команда log - изменить учётное имя (login) пользователя
			bResult = SetLogin(changed_data);
			break;
		case SLCMD_CHANGE_EMAIL:	// локальная команда email - изменить e-mail пользователя
			bResult = SetEmail(changed_data);
			break;
		case SLCMD_CHANGE_PASSWORD:	// локальная команда pwd - изменить пароль пользователя
			bResult = (SetPassword(changed_data));
			break;
	}

	return bResult;
}