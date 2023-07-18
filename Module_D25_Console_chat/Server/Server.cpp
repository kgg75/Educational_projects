#include "StdAfx.h"

#include "Constants.h"
#include "Users.h"
#include "Messages.h"
#include "Server.h"
#include "DBFunctions.h"
#include "ServiceFunc.h"


using namespace std;

//Users users;
//Messages messages;
map <SOCKET, uint> users_map;	// связка сокет - идентификатор (id) пользователя

char		request_msg[BUFFER_LENGTH],
			response_msg[BUFFER_LENGTH];

SOCKET		server_descriptor;

fd_set		descriptors_set {0};

const		timeval timeout = {0, 0};

string		console_prompt = "Введите команду: ";

bool		loop = true;	// флаг продолжения/остановки ChatServer()


// печатает справку по командам
void print_help(const CommandSpace* _commandSpace) {	// печать справки по командам
	for (int i = 0; _commandSpace[i].command[0] != 0; i++)
		cout << "\t" << _commandSpace[i].command << "\t- " << _commandSpace[i].help << '\n';
}


// печатает начальные сообщения
void AtStart() {	
	cout.clear();
	cout << SEPARATOR;
	cout << "---=== Чат-сервер версия 2.0 ===---\n";
	cout << SEPARATOR;
	cout << "Сервер запущен " << GetTime() << '\n';
	cout << SEPARATOR;
	cout << "Справка по командам:\n";
	print_help(SERVER_COMMANDS);
}


// инициализирует сервер
int server_start() {	
	struct sockaddr_in server_address;
	int bind_status, connection_status;

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

	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(PORT_NUMBER);  // Зададим номер порта для связи
	server_address.sin_family = AF_INET;    // Используем IPv4

	BOOL bOptVal = TRUE;
	int bOptLen = sizeof(BOOL);
	int iResult = setsockopt(server_descriptor, SOL_SOCKET, SO_REUSEADDR, (char*)&bOptVal, bOptLen);
	iResult = setsockopt(server_descriptor, SOL_SOCKET, SO_BROADCAST, (char*)&bOptVal, bOptLen);

	bind_status = bind(server_descriptor, (struct sockaddr*)&server_address, sizeof(server_address));	// Привяжем сокет
	if (bind_status WIN(== SOCKET_ERROR)NIX(!= 0)) {
		cout << "Сокет не может быть привязан!\n";
		WIN(cout << "Код ошибки: " << WSAGetLastError() << '\n';)
		return -1;
	}
	
	connection_status = listen(server_descriptor, 5);	// Поставим сервер на прием данных 
	if (connection_status WIN(== SOCKET_ERROR)NIX(!= 0)) {
		cout << "Сервер не может установить новое соединение!\n";
		WIN(cout << "Код ошибки: " << WSAGetLastError() << '\n';)
		return -1;
	}

	return 0;
}


// закрывает сокет, завершает соединение с базой данных
void server_stop() {	
	WIN(closesocket)NIX(close)(server_descriptor);
	WIN(WSACleanup());

	Close_ODBC();
}


// очищает консоль и выводит приглашение
void ConsoleWaitMessage() {
	cin.clear();
	//cin.ignore(numeric_limits<streamsize>::max(), '\n');	// очистка буфера консоли
	cout << console_prompt;
}


// выполняет сетевое подключение и подключение базы данных
int InitializeServer() {
	cout << "Инициализация сервера чата... ";
	if (server_start() == 0)
		cout << "выполнена.\n";
	else {
		cout << "Ошибка инициализации сервера чата; программа будет закрыта.\n";
		loop = false;
		return 1;
	}

	int iResult = Initialize_ODBC();
	if (iResult != 0)
		return iResult;

	AtStart();
	return 0;
}

// отправляет сетевое сообщение - ответ
int SendResponse(const SOCKET connection_descriptor, const char* local_response_msg, const uint local_response_msg_length) {
	int iResult =	WIN(send(connection_descriptor, local_response_msg, local_response_msg_length, 0))	// отправляем ответ
						NIX(write(connection_descriptor, local_response_msg, local_response_msg_length));

	if (iResult WIN(== SOCKET_ERROR)NIX(!= 0)) {	// Если передали > 0  байт, значит пересылка прошла успешно
		cout << "Ошибка отправки данных клиенту!\n";
		WIN(cout << "Код ошибки: " << WSAGetLastError() << '\n';)
	}

	return iResult;
}


// принимает сетевое сообщение - запрос
int ReadRequest(const SOCKET connection_descriptor) {
	int iResult =	WIN(recv(connection_descriptor, request_msg, BUFFER_LENGTH, 0))	// получаем ответ
					NIX(read(connection_descriptor, request_msg, BUFFER_LENGTH));

	if (iResult WIN(== SOCKET_ERROR)NIX(!= 0)) {	// Если получили > 0  байт, значит приём прошёл успешно
		cout << "Ошибка получения данных от клиента!\n";
		WIN(cout << "Код ошибки: " << WSAGetLastError() << '\n';)
	}

	return iResult;
}


// обрабатывает запросы клиентов и отправляет ответы
void RequestHandler(const SOCKET connection_descriptor) {
	string	login,
			name,
			surname,
			email,
			receiver,
			text;

	int		iResult,
			response_msg_length = 1;

	uint	id,
			count,
			index = 1,
			receiverId;

	uchar	command_code = request_msg[0];	// передаём код команды

	WIN(memset(response_msg, 0, BUFFER_LENGTH))NIX(bzero(response_msg, BUFFER_LENGTH));
	response_msg[0] = MSG_SUCCESS_RESULT;	// устанавливаем значение ответа (1-й байт) по умолчанию

	switch (command_code) {
		case GCMD_REG:	// команда reg - зарегистрироваться
			name = &request_msg[1];
			index += full_length(name);	// создаём нового пользователя
			surname = &request_msg[index];
			index += full_length(surname);
			login = &request_msg[index];
			index += full_length(login);
			email = &request_msg[index];
			index += full_length(email);
			iResult = NewUser(name, surname, login, email, (SHA1PwdArray&)request_msg[index]);

			if (iResult > 0) {	// регистрация прошла успешно
				users_map.find(connection_descriptor)->second = iResult;	// добавляем в словарь id пользователя
				response_msg[0] = MSG_SUCCESS_RESULT;
			}
			else
				response_msg[0] = ERR_SERVER_ERROR;	// подготовили ответ

			break;
		case GCMD_LOGIN:	// команда login - вход
			login = &request_msg[1];
			iResult = GetUserId(login);

			if (iResult > 0) {
				id = iResult;
				for (map <SOCKET, uint>::iterator it = users_map.begin(); it != users_map.end(); it++)
					if (it->second == id) {	// пользователь с таким id уже вошёл
						response_msg[0] = ERR_USER_ALREADY_LOGGED;
						break;
					}

				if (response_msg[0] != ERR_USER_ALREADY_LOGGED) {
					iResult = CheckHash(id, (SHA1PwdArray&)request_msg[1 + full_length(login)]);	// передаём ссылку на фрагмент буфeра с данными хэша
					if (iResult > 0) {	// login прошёл успешно
						users_map.find(connection_descriptor)->second = iResult;	// добавляем id пользователя в словарь
						iResult = 1;
						SetLastVisit(users_map.at(connection_descriptor));	// на случай сбоя устанавливаем текущие дату/время как дату/время последнего посещения
						cout << '\n' << GetTime() << " на сокете #" << connection_descriptor << " вошёл в чат пользователь '" << login << "'\n";
						ConsoleWaitMessage();
					}
				}
			}
			response_msg[0] = (char)iResult;	// подготовили ответ
			break;
		case LCMD_SEND:
			receiver = &request_msg[1];
			text = &request_msg[1 + full_length(receiver)];
			receiverId = GetUserId(receiver);

			iResult = AddMessageToDB(users_map.at(connection_descriptor), receiverId, text);
			if (iResult > 0) {	// нет ошибок
				id = iResult;	// сохраняем id сообщения

				if (GetLogin(users_map.at(connection_descriptor), login) == MSG_SUCCESS_RESULT) {
					for (map <SOCKET, uint>::iterator it = users_map.begin(); it != users_map.end(); it++) {
						if (it->second == receiverId) {	// получатель сообщения находится в чате
							response_msg[0] = LCMD_RECEIVE;
							response_msg_length += strcopy(&response_msg[1], login);
							response_msg_length += strcopy(&response_msg[response_msg_length], text);

							SendResponse(it->first, response_msg, response_msg_length);	// отправляем сообщение получателю на дескриптор it->first
							ReadRequest(it->first);	// должны получить ответ MSG_SUCCESS_RESULT

							if (request_msg[0] == MSG_SUCCESS_RESULT)
								SetReadedState(receiverId, id);	// помечаем прочитанным
							break;
						}
					}
				}
			}
					
			response_msg[0] = (char)iResult;	// подготовили ответ
			break;
		case LCMD_READ_ALL:
		case LCMD_READ_UNREADED:
			receiver = &request_msg[1];
			receiverId = GetUserId(receiver);
			iResult = PrepareListByOneM(receiverId, (command_code == LCMD_READ_UNREADED ? true : false));

			if (iResult > 0) {
				auto i = 1;
				while (ListByOneM(i++, text) != ERR_NO_RESULT) {
					strcopy(&response_msg[1], text);
					SendResponse(connection_descriptor, response_msg, 1 + full_length(text));	// отправляем ответы
					ReadRequest(connection_descriptor);
				}
				SetReadedState(receiverId);
				response_msg[0] = MSG_STOP_EXCHANGE;
			}
			else if (iResult == 0)	// список сообщений пуст
				response_msg[0] = ERR_NO_RESULT;
			else if (iResult == ERR_REQUEST_ERROR)
				response_msg[0] = ERR_SERVER_ERROR;

			break;
		case LCMD_USER_INFO:
			iResult = UserInfo(users_map.at(connection_descriptor), text);
			if (iResult != ERR_REQUEST_ERROR) {
				strcopy(&response_msg[1], text);
				response_msg_length += iResult + 1;
			}
			else
				response_msg[0] = ERR_NO_RESULT;
			break;
		case LCMD_USERS_LIST:
			iResult = PrepareListByOneU();

			if (iResult > 0) {
				auto i = 1;
				while (ListByOneU(i++, text) != ERR_NO_RESULT) {
					strcopy(&response_msg[1], text);
					SendResponse(connection_descriptor, response_msg, 1 + full_length(text));	// отправляем ответы
					ReadRequest(connection_descriptor);
				}
				response_msg[0] = MSG_STOP_EXCHANGE;
			}
			else if (iResult == 0)	// список пользователей пуст
				response_msg[0] = ERR_NO_RESULT;
			else if (iResult == ERR_REQUEST_ERROR)
				response_msg[0] = ERR_SERVER_ERROR;

			break;
		case LCMD_USER_DELETE:
			iResult = SetIsDeleted(users_map.at(connection_descriptor));	// аргумент - id пользователя
			if (iResult == MSG_SUCCESS_RESULT)
				users_map.find(connection_descriptor)->second = 0;
			response_msg[0] = (char)iResult;
			break;
		case SLCMD_CHANGE_NAME:
		case SLCMD_CHANGE_SURNAME:
		case SLCMD_CHANGE_LOGIN:
		case SLCMD_CHANGE_EMAIL:
			iResult = ChangeRegData(users_map.at(connection_descriptor), &request_msg[1], command_code);
			response_msg[0] = (char)iResult;
			break;
		case SLCMD_CHANGE_PASSWORD:
			response_msg[0] = (char)SetPassword(users_map.at(connection_descriptor), (SHA1PwdArray&)request_msg[1]);
			break; 
		case LCMD_LOGOUT:	// локальная команда logout - выход
			SetLastVisit(users_map.at(connection_descriptor));
			users_map.find(connection_descriptor)->second = 0;
			cout << '\n' << GetTime() << " на сокете #" << connection_descriptor << " вышел из чата пользователь '" << &request_msg[1] << "'\n";
			ConsoleWaitMessage();
			break;
		case LCMD_IS_USER_EXIST:
			response_msg[0] = (char)(GetUserId(&request_msg[1]) > 0 ? MSG_SUCCESS_RESULT : ERR_NO_RESULT);
			break;
		case LCMD_MESSAGES_COUNT:	// все сообщения
		case LCMD_UNREADED_COUNT:	// только непрочитанные
			count = GetMessagesCount(users_map.at(connection_descriptor), command_code == LCMD_UNREADED_COUNT);
			response_msg[0] = (char)(count > 0 ? MSG_SUCCESS_RESULT : ERR_NO_RESULT);
			memcpy(&response_msg[1], &count, sizeof(count));	// помещаем в буфер кол-во непрочитанных сообщений
			response_msg_length += sizeof(count);
			break;
		default:
			;
			break;
	}

	iResult = SendResponse(connection_descriptor, response_msg, response_msg_length);	// отправляем ответ
}


// обработчик консольных команд сервера чата
int ConsoleCommandsHandler() {
	int iResult;
	string command, text;

	while (loop) {
		ConsoleWaitMessage();
		getline(cin, command);

		if (!command.empty()) {
			switch (get_command_code(SERVER_COMMANDS, command)) {	// анализируем серверную команду
				//case SCMD_SAVE:
					//break;
				case SCMD_CON:
					if (!users_map.empty()) {
						cout << "Список подключений:\n   сокет : учётное имя\n";
						for (map <SOCKET, uint>::iterator it = users_map.begin(); it != users_map.end(); it++) {
							if (it->second != 0)
								iResult = GetLogin(users_map.at(it->first), text);
							else
								text = "<вход не выполнен>";
							cout << "     " << it->first << " : " << text << "\n";
						}
					}
					else
						cout << "Нет подключений.\n";
					break;
				case SCMD_USERS:
					iResult = PrepareListByOneU();
					
					if (iResult > 0) {
						cout << "Список пользователей:\n   № #id имя\tфамилия\tlogin\te-mail\tдата регистр.\tпоследнее посещение\n";
						auto i = 1;
						while (ListByOneU(i++, text) != ERR_NO_RESULT)
							cout << text;
					}
					else if (iResult == ERR_REQUEST_ERROR)
						PrintErrorMessage(iResult);
					else
						cout << "Нет пользователей.\n";
					break;
				case SCMD_HELP:
					print_help(SERVER_COMMANDS);
					break;
				case SCMD_EXIT:
					if (!users_map.empty()) {
						if (QuestionYN("Есть действующие подключения. Всё равно закрыть сервер?"))
							loop = false;	// инициализируем остановку сервера
					}
					else if (QuestionYN("Подтвердите остановку сервера")) {
						loop = false;	// инициализируем остановку сервера
					}
					break;
				default:
					cout << MSG_ERR_CMD;
			}
		}
		WIN(Sleep(DELAY_MS))NIX(usleep(DELAY_MCS));
	}

	return 0;
}


// главный обработчик чата - принимает подключения клиентов и запросы
int ChatServer() {
	while (loop) {	// Communication Establishment
		FD_ZERO(&descriptors_set);
		FD_SET(server_descriptor, &descriptors_set);

		int iResult = select(0, &descriptors_set, nullptr, nullptr, &timeout);	// проверяем готовность сокета на новые подключения

		if (iResult > 0 && users_map.size() < FD_SETSIZE) {
			struct sockaddr_in client_address;
			socklen_t length = sizeof(client_address);
			SOCKET socket = accept(server_descriptor, (struct sockaddr*)&client_address, &length);
			if (socket WIN(== INVALID_SOCKET)NIX(< 0)) {
				cout << "\nСервер не может принимать данные от клиента!\n";
				WIN(cout << "Код ошибки: " << WSAGetLastError() << '\n';)
			}
			else {
				users_map.emplace(socket, 0);
				cout << '\n' << GetTime() << " подключился клиент #" << socket << '\n';
			}
			ConsoleWaitMessage();
		}
		else {
			if (iResult WIN(== SOCKET_ERROR)NIX(!= 0)) {
				WIN(cout << "Код ошибки: " << WSAGetLastError() << '\n';)
				if (QuestionYN("Внутренняя ошибка. Закрыть сервер?")) {
					loop = false;
					break;	// инициализируем остановку сервера
				}

				ConsoleWaitMessage();
			}


			WIN(Sleep(DELAY_MS))NIX(usleep(DELAY_MCS));

			if (users_map.size() == 0)
				continue;
		}

		for (map <SOCKET, uint>::iterator it = users_map.begin(); it != users_map.end(); ) {
			FD_ZERO(&descriptors_set);
			FD_SET(it->first, &descriptors_set);

			iResult = select(0, nullptr, nullptr, &descriptors_set, &timeout);	// проверяем сокет на ошибки
			if (iResult > 0) {
				WIN(cout << "\nЕсть ошибки в подключении клиента\t" << it->first <<
					"\nКод ошибки : " << WSAGetLastError() << '\n';);
				ConsoleWaitMessage();
			}

			descriptors_set.fd_count = 1;
			iResult = select(0, &descriptors_set, nullptr, nullptr, &timeout);	// проверяем готовность сокета на чтение
			if (iResult > 0) {
				WIN(memset(request_msg, 0, BUFFER_LENGTH))NIX(bzero(request_msg, BUFFER_LENGTH));
				iResult = recv(it->first, request_msg, BUFFER_LENGTH, 0);	// принимаем запрос
				
				if (iResult > 0) {
					if (strcmp(request_msg, CLIENT_STOP_MESSAGE) == 0) {
						cout << '\n' << GetTime() << " отключился клиент #" << it->first << '\n';
						ConsoleWaitMessage();
						it = users_map.erase(it);
						continue;
					}

					RequestHandler(it->first);	// обрабатывает входящие данные из буфера и отправляет ответы
				}
				else if (iResult WIN(== SOCKET_ERROR)NIX(!= 0)) {	// клиент "отвалился"
					cout << '\n' << GetTime() << " связь с клиентом #" << it->first << " потеряна\n";
					ConsoleWaitMessage();
					it = users_map.erase(it);
					continue;
				}
			}
			it++;
		}
		WIN(Sleep(DELAY_MS))NIX(usleep(DELAY_MCS));
	}

	// остановка сервера и выход
	server_stop();

	return 0;
}

