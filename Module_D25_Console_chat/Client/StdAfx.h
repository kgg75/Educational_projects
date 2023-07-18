#pragma once

#if defined(_WIN32)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <sqlext.h>
	#include <sqltypes.h>
	#include <sql.h>
	
	#pragma comment(lib, "Ws2_32.lib")
	#pragma comment(lib, "Mswsock.lib")
	#pragma comment(lib, "AdvApi32.lib")

	typedef unsigned __int64    ssize_t;

	#if defined(max)
	#undef max
	#endif

	// Макросы для выражений зависимых от OS    
	#define WIN(exp) exp
	#define NIX(exp)
#elif defined(__linux__)
	#include <unistd.h>
	#include <sys/socket.h> // Библиотека для работы с сокетами
	//#include <sys/types.h>
	#include <netinet/in.h>
	// Макросы для выражений зависимых от OS    
	#define WIN(exp)
	#define NIX(exp) exp
#endif


#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

