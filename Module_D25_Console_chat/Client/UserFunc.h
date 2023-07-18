#pragma once
#include "../Server/Constants.h"


bool SetName(std::string& name);	// выполняет ввод и проверку имени
bool SetSurname(std::string& surname);	// выполняет ввод и проверку фамилии
bool SetLogin(std::string& login);	// выполняет ввод и проверку login
bool SetEmail(std::string& login);	// выполняет ввод и проверку email
bool SetPassword(std::string& password);	// выполняет ввод и проверку пароля
bool NewUser(std::string& name, std::string& surname, std::string& login, std::string& email, std::string& password);	// собирает сведения для регистрации нового пользователя
bool SignIn(std::string& login, std::string& password);	// собирает сведения для входа пользователя в чат
bool ChooseUser(std::string& receiver);	// выбирает получателя для отправки сообщения
bool AddMessage(std::string& text);	// добавляет сообщение
bool SetRegData(std::string& changed_data, const uchar regDataType);	// выполняет запрос новых регистрационных данных