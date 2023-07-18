#include "StdAfx.h"

#include "Client.h"
#include "UserFunc.h"
#include "../Server/Constants.h"
#include "../Server/ServiceFunc.h"
#include "../Server/SHA1.h"


using namespace std;


char			request_msg[BUFFER_LENGTH],
				response_msg[BUFFER_LENGTH];

uint			request_msg_length,
				response_msg_length;

SOCKET			server_descriptor;

fd_set			descriptors_set {0};

const timeval	timeout = {0, 0};

string			console_prompt;

bool			loop = true;	// флаг продолжения/остановки ChatClient()


// подключает клиента к серверу
int client_start() {
	int iResult;

	WIN(
		WSADATA wsaData = {0};
		WSAStartup(MAKEWORD(2, 2), &wsaData);
	)

	server_descriptor = socket(AF_INET, SOCK_STREAM, 0);	// Создадим сокет
	if (server_descriptor WIN(== INVALID_SOCKET)NIX(< 0)) {
		cout << "Сокет не может быть создан!\n";
		WIN(cout << "Код ошибки: " << WSAGetLastError() << '\n';)
		return -1;
	}

	struct sockaddr_in server_address;
	inet_pton(AF_INET, SERVER_ADDRESS, &server_address.sin_addr.s_addr);    // Установим адрес сервера
	server_address.sin_port = htons(PORT_NUMBER);    // Зададим номер порта
	server_address.sin_family = AF_INET;    // Используем IPv4

	// Установим соединение с сервером
	iResult = connect(server_descriptor, (struct sockaddr*)&server_address, sizeof(server_address));
	if (iResult WIN(== SOCKET_ERROR)NIX(!= 0)) {
		cout << "Соединение с сервером не установлено!\n";
		WIN(cout << "Код ошибки: " << WSAGetLastError() << '\n';)
		return -1;
	}
	return 0;
}


// отключает клиента от сервера
void client_stop() {	// закрываем сокет, завершаем соединение
	WIN(send(server_descriptor, CLIENT_STOP_MESSAGE, sizeof(CLIENT_STOP_MESSAGE), 0))	// информируем сервер о своём завершении
	NIX(write(server_descriptor, CLIENT_STOP_MESSAGE, sizeof(CLIENT_STOP_MESSAGE)));

	WIN(closesocket)NIX(close)(server_descriptor);

	WIN(WSACleanup());
}


// очищает консоль и выводит приглашение
void ConsoleWaitMessage() {
	cin.clear();
	//cin.ignore(numeric_limits<streamsize>::max(), '\n');	// очистка буфера консоли
	cout << console_prompt;
}


// печатает справку по командам
void print_help(const CommandSpace* commandSpace) {
	for (int i = 0; commandSpace[i].command[0] != 0; i++)
		cout << "\t" << commandSpace[i].command << "\t- " << commandSpace[i].help << '\n';
}


// печатает начальные сообщения
void AtStart() {	
	cout.clear();
	cout << SEPARATOR;
	cout << "* * *  Чат-клиент версия 2.0  * * *\n";
	cout << SEPARATOR;
	cout << "Чат запущен " << GetTime() << '\n';
	cout << SEPARATOR;
	cout << "Выберите действие:\n";
	print_help(GLOBAL_COMMANDS);
	cout << SEPARATOR;
}


// выполяет обмен сетевыми сообщениями (отправка/приём)
int MessagesExchange(const uchar command_code) {
	request_msg[0] = command_code;	// размещаем код команды в ячейку [0];

	int iResult =	WIN(send(server_descriptor, request_msg, request_msg_length, 0))
					NIX(write(server_descriptor, request_msg, request_msg_length));

	if (iResult WIN(== SOCKET_ERROR)NIX(!= 0)) {	// Если передали >= 0  байт, значит пересылка прошла успешно
		cout << "Ошибка отправки данных на сервер!\n";
		WIN(cout << "Код ошибки: " << WSAGetLastError() << '\n';)
	}

	iResult =	WIN(recv(server_descriptor, response_msg, BUFFER_LENGTH, 0))	// получаем ответ
				NIX(read(server_descriptor, response_msg, BUFFER_LENGTH));

	if (iResult WIN(== SOCKET_ERROR)NIX(!= 0)) {	// Если получили >= 0  байт, значит приём прошёл успешно
		cout << "Ошибка получения данных с сервера!\n";
		WIN(cout << "Код ошибки: " << WSAGetLastError() << '\n';)
	}

	return iResult;
}


// выполняет запрос числа сообщений
uint RequestMessagesQty(const string& receiver, const uchar flag_UnReadedOrAll) {	
	strcopy(&request_msg[1], receiver);	// копируем login в буфер для отправки
	request_msg_length = 1 + full_length(receiver);
	MessagesExchange(flag_UnReadedOrAll);	// получаем количество сообщений

	uint unReadedMsgs;
	memcpy(&unReadedMsgs, &response_msg[1], sizeof(unReadedMsgs));
	return unReadedMsgs;
}


// выполняет циклический запрос и печать набора сообщений
void PrintMessages(const uchar command_code, const string& login) {
	strcopy(&request_msg[1], login);	// копируем login в буфер для отправки
	request_msg_length = 1 + full_length(login);

	do {
		MessagesExchange(command_code);	// инициализируем передачу и подтверждаем продолжение передачи сообщений
		if (response_msg[0] == MSG_SUCCESS_RESULT)
			cout << &response_msg[1];
	} while (response_msg[0] == MSG_SUCCESS_RESULT);

	if (response_msg[0] < 0)	// < 0 - возвращён код ошибки
		PrintErrorMessage(response_msg[0]);
}


void RequestHandler() {
	string sender, text;
	int iResult, response_msg_length = 1;
	//uint count, index = 1, receiverId;
	uchar command_code = request_msg[0];

	WIN(memset(response_msg, 0, BUFFER_LENGTH))NIX(bzero(response_msg, BUFFER_LENGTH));
	response_msg[0] = MSG_SUCCESS_RESULT;	// устанавливаем значение ответа (1-й байт) по умолчанию

	switch (command_code) {
		case LCMD_RECEIVE:
			sender = &request_msg[1];
			text = &request_msg[1 + sender.length() + 1];
			cout << '\n' << GetTime() << " получено сообщение от '" << sender << "': " << text << '\n';
			ConsoleWaitMessage();
			
			response_msg[0] = MSG_SUCCESS_RESULT;
			response_msg_length = 1;
			iResult = WIN(send(server_descriptor, response_msg, response_msg_length, 0))	// TODO!! вынести в функцию
				NIX(write(server_descriptor, response_msg, response_msg_length));

			if (iResult WIN(== SOCKET_ERROR)NIX(!= 0)) {	// Если передали >= 0  байт, значит пересылка прошла успешно
				cout << "Ошибка отправки данных на сервер!\n";
				WIN(cout << "Код ошибки: " << WSAGetLastError() << '\n';)
			}
			;
			break;
		default:
			;
			break;
	}
}


// обработчик консольных команд клиента чата
int ConsoleCommandsHandler() {	
	string login, new_login, name, surname, email, password, receiver, text, any_text;
	SHA1PwdArray pass_sha1_hash;	// хэш пароля
	uint index;

	AtStart();

	while (loop) {
		WIN(memset(request_msg, 0, BUFFER_LENGTH))NIX(bzero(request_msg, BUFFER_LENGTH));

		switch (auto g_command_code = get_command(GLOBAL_COMMANDS)) {	// получаем глобальную команду с консоли
			case GCMD_REG:	// команда reg - зарегистрироваться
				if (NewUser(name, surname, login, email, password)) {
					index = 1;
					index += strcopy(&request_msg[index], name);	// копируем name
					index += strcopy(&request_msg[index], surname);	// копируем surname
					index += strcopy(&request_msg[index], login);	// копируем login
					index += strcopy(&request_msg[index], email);	// копируем email

					CalculateSHA1(pass_sha1_hash, password.c_str());

					memcpy(&request_msg[index], &pass_sha1_hash, SHA1_HASH_LENGTH_BYTES);	// копируем хэш
					request_msg_length = index + SHA1_HASH_LENGTH_BYTES;
					MessagesExchange(g_command_code);

					if (response_msg[0] == MSG_SUCCESS_RESULT)	// пользователь успешно зарегистрировался
						cout << "Вы успешно зарегистрированы!\n";
					else
						PrintErrorMessage(response_msg[0]);
				}
				break;
			case GCMD_LOGIN:	// команда login - вход
				if (SignIn(login, password)) {
					index = 1;
					index += strcopy(&request_msg[index], login);	// копируем login

					CalculateSHA1(pass_sha1_hash, password.c_str());

					memcpy(&request_msg[index], &pass_sha1_hash, SHA1_HASH_LENGTH_BYTES);	// копируем хэш
					request_msg_length = index + SHA1_HASH_LENGTH_BYTES;
					MessagesExchange(g_command_code);

					if (response_msg[0] == MSG_SUCCESS_RESULT) {	// пользователь успешно авторизовался
						cout << "Пользователь '" << login << "' вошёл в чат.\n";

						uint unReadedMsgs = RequestMessagesQty(login, LCMD_UNREADED_COUNT);
						if (unReadedMsgs > 0) {
							cout << "Есть непрочитанные сообщения (" << unReadedMsgs << "). ";
							if (QuestionYN("Показать?"))	// вывод непрочитанных сообщений
								PrintMessages(LCMD_READ_UNREADED, login);
						}
						
						console_prompt = login + ", введите команду >> ";
						while (!login.empty()) {	// пока пользователь не вышел из чата
							WIN(memset(request_msg, 0, BUFFER_LENGTH))NIX(bzero(request_msg, BUFFER_LENGTH));
							request_msg_length = 1;	// устанавливаем минимальную длину сообщения

							// начало локального пространства команд
							switch (auto l_command_code = get_command(LOCAL_COMMANDS, console_prompt)) {	// получаем локальную команду с консоли
								case LCMD_SEND:	// локальная команда send - отправить сообщение
									if (!ChooseUser(receiver))	// выбор получателя
										break;
										
									request_msg_length += strcopy(&request_msg[1], receiver);	// копируем receiver
									MessagesExchange(LCMD_IS_USER_EXIST);	// проверяем существование получателя

									if (response_msg[0] == MSG_SUCCESS_RESULT) {
										if (receiver != login) {
											if (AddMessage(text)) {
												index = 1;
												index += strcopy(&request_msg[index], receiver);	// копируем имя получателя
												request_msg_length = index + strcopy(&request_msg[index], text);	// копируем text, устанавливаем длину сообщения
												
												MessagesExchange(l_command_code);

												if (response_msg[0] != 0)
													cout << "Сообщение для '" << receiver << "' отправлено.\n";
											}
										} else
											cout << "Нельзя отправлять сообщения самому себе.\n";
									}
									else
										PrintErrorMessage(response_msg[0]);
									break;
								case LCMD_READ: {	// локальная команда read - прочитать сообщения
										bool answer = QuestionYN("Показать только непрочитанные (y) или все сообщения (n)?");
										uint messsagesQty = RequestMessagesQty(login, answer ? LCMD_UNREADED_COUNT : LCMD_MESSAGES_COUNT);	// получаем количество сообщений
										
										if (messsagesQty > 0)
											PrintMessages(answer ? LCMD_READ_UNREADED : LCMD_READ_ALL, login);
										else
											cout << "Нет " << (answer ? "непрочитанных " : "") << "сообщений.\n";
									}
									break;
								case LCMD_USER_INFO:	// локальная команда info - вывести информацию о пользователе
									MessagesExchange(l_command_code);
									if (response_msg[0] == MSG_SUCCESS_RESULT)
										cout << "Информация о пользователе:\n" << &response_msg[1] << '\n';
									else
										PrintErrorMessage(response_msg[0]);
									break;
								case LCMD_USERS_LIST:	// локальная команда list - вывести список всех пользователей
									cout << "Список пользователей:\n   № #id имя\tфамилия\tlogin\te-mail\tдата регистр.\tпоследнее посещение\n";
									PrintMessages(l_command_code, login);
									break;
								case LCMD_USER_DELETE:	// локальная команда delete - удалить учётную запись пользователя
									if (QuestionYN("Информация о пользователе будет навсегда удалена. Вы уверены?") && 
										QuestionYN("Это действие нельзя будет отменить. Подтвердите удаление ещё раз."))
										MessagesExchange(l_command_code);
										login.clear();
									break;
								case LCMD_USER_CHANGE: {	// локальная команда change - изменить учётные данные пользователя
										bool lcmd_loop = true;
										cout << "Вы находитесь в меню изменения учётных данных пользователя '" << login << "'\n";
										print_help(SUBLOCAL_COMMANDS);

										while (lcmd_loop) {
											WIN(memset(request_msg, 0, BUFFER_LENGTH))NIX(bzero(request_msg, BUFFER_LENGTH));
											console_prompt = login + ", выберите действие или команду >> ";
											request_msg_length = 1;	// устанавливаем минимальную длину сообщения

											switch (auto sl_command_code = get_command(SUBLOCAL_COMMANDS, console_prompt)) {	// получаем локальную команду с консоли
												case SLCMD_CHANGE_NAME:		// локальная команда name		- изменить имя пользователя
												case SLCMD_CHANGE_SURNAME:	// локальная команда surname	- изменить фамилию пользователя
												case SLCMD_CHANGE_LOGIN:	// локальная команда log		- изменить учётное имя (login) пользователя
												case SLCMD_CHANGE_EMAIL:	// локальная команда email		- изменить e-mail пользователя
												case SLCMD_CHANGE_PASSWORD:	// локальная команда pwd		- изменить пароль пользователя
													SetRegData(any_text, sl_command_code);
														
													if (sl_command_code == SLCMD_CHANGE_PASSWORD) {
														CalculateSHA1(pass_sha1_hash, any_text.c_str());
														
														memcpy(&request_msg[1], &pass_sha1_hash, SHA1_HASH_LENGTH_BYTES);	// копируем новый хэш
														request_msg_length += SHA1_HASH_LENGTH_BYTES;
													}
													else
														request_msg_length += strcopy(&request_msg[1], any_text); // копируем параметр, устанавливаем длину сообщения

													MessagesExchange(sl_command_code);

													if (response_msg[0] == MSG_SUCCESS_RESULT) {
														switch (sl_command_code) {
															case SLCMD_CHANGE_NAME:	// локальная команда name - "изменить имя пользователя"
																cout << "Имя пользователя '" << login << "' было изменено.\n\n";
																break;
															case SLCMD_CHANGE_SURNAME:	// локальная команда surname - "изменить фамилию пользователя"
																cout << "Фамилия пользователя '" << login << "' была изменена.\n\n";
																break;
															case SLCMD_CHANGE_LOGIN:	// локальная команда log - "изменить учётное имя (login) пользователя"
																login = any_text;
																cout << "Учётное имя (login) пользователя было изменено на '" << login << "'.\n\n";
																break;
															case SLCMD_CHANGE_EMAIL:	// локальная команда email - "изменить e-mail пользователя"
																cout << "Адрес эл. почты пользователя '" << login << "' был изменён.\n\n";
																break;
															case SLCMD_CHANGE_PASSWORD:	// локальная команда pwd - "изменить пароль пользователя"
																cout << "Пароль пользователя '" << login << "' был изменён.\n\n";
																break;
														}
													}
													else
														PrintErrorMessage(response_msg[0]);

													break;
												case SLCMD_HELP:	// локальная команда help - вывести справку по командам
													print_help(SUBLOCAL_COMMANDS);
													break;
												case SLCMD_QUIT:	// локальная команда quit - выйти из этого меню
													lcmd_loop = false;
													cout << "Вы вышли из меню изменения учётных данных.\n\n";
													break;
												default:
													cout << MSG_ERR_CMD;
											}
										}
										console_prompt = login + ", введите команду >> ";	// восстанавливаем приглашение консоли

									}	// конец локального пространства команды change - "изменить учётные данные пользователя"
									break;
								case LCMD_HELP:	// локальная команда help - вывести справку по командам
									print_help(LOCAL_COMMANDS);
									break;
								case LCMD_LOGOUT:	// локальная команда logout - выход
									request_msg_length += strcopy(&request_msg[1], login);	// копируем login, устанавливаем длину сообщения
									MessagesExchange(l_command_code);
									login.clear();
									break;
								default:
									cout << MSG_ERR_CMD;
							}
						}	// конец локального пространства команд
					}
					else
						PrintErrorMessage(response_msg[0]);	// ошибки авторизации
				}
				break;	// продолжаем запрос команд
			case GCMD_HELP:	// команда help - вывести справку по командам
				print_help(GLOBAL_COMMANDS);
				break;
			case GCMD_EXIT:	// команда exit - закрыть чат
				loop = false;	// инициализируем остановку клиента
				break;
			default:
				cout << MSG_ERR_CMD;
		}
	}
	return 0;
}


int ChatClient() {	// главный обработчик чата
	while (loop) {
		FD_ZERO(&descriptors_set);
		FD_SET(server_descriptor, &descriptors_set);

		int iResult = select(0, nullptr, nullptr, &descriptors_set, &timeout);	// проверяем сокет на ошибки
		if (iResult > 0) {
			WIN(cout << "\nЕсть ошибки в подключении к серверу\nКод ошибки: " << WSAGetLastError() << '\n';);
			ConsoleWaitMessage();
		}

		descriptors_set.fd_count = 1;
		iResult = select(0, &descriptors_set, nullptr, nullptr, &timeout);	// проверяем готовность сокета на чтение
		if (iResult > 0) {
			iResult = recv(server_descriptor, request_msg, BUFFER_LENGTH, 0);	// принимаем запрос
			if (iResult > 0)
				RequestHandler();	// обрабатывает входящие данные из буфера и отправляет ответы
			else if (iResult WIN(== SOCKET_ERROR)NIX(!= 0)) {
				WIN(cout << "Код ошибки: " << WSAGetLastError() << '\n';)
				cout << "Подключение к серверу потеряно, клиент будет закрыт.\n";
				loop = false;	// инициализируем остановку клиента
				break;
			}
		}
		WIN(Sleep(DELAY_MS))NIX(usleep(DELAY_MCS));
	}

	client_stop();	// остановка клиента и выход

	return 0;
}
