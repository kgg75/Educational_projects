#include "StdAfx.h"

#include "Client.h"


using namespace std;


int main() {
	system("chcp 1251");

	// инициализация клиента чата
	cout << "Подключение к серверу...";

	if (client_start() == 0)
		cout << "выполнено!\n";
	else {
		cout << "Ошибка подключения к серверу чата; программа будет закрыта.\n";
		return 1;
	}

	thread chatCommandsThread(ConsoleCommandsHandler);
	int iResult = ChatClient();	// запуск работы чата; можно получить результат (зарезервировано)

	if (chatCommandsThread.joinable())
		chatCommandsThread.join();

	//iResult = Chat();	// запуск работы чата; можно получить результат (зарезервировано)
	
	return iResult;
}