#include "StdAfx.h"
#include "Server.h"


using namespace std;


int main() {
	system("chcp 1251");

	int iResult = InitializeServer();
	if (iResult != 0)
		return iResult;

	thread chatCommandsThread(ConsoleCommandsHandler);
	iResult = ChatServer();

	if (chatCommandsThread.joinable())
		chatCommandsThread.join();

	return iResult;
}