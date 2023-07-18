#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>

#include <QString>

#include "Constants.h"
//#include "_Server/StdAfx.h"
//#include <sql.h>


uchar get_command_code(const CommandSpace* commandSpace, std::string& command);	// выполняет анализ команды и возвращает соотвествующий код действия
uchar get_command(const CommandSpace* commandSpace, const std::string console_prompt = "Введите команду >> ");	// выполняет получение команды с консоли, возвращает соотвествующий код действия
void lowercase(std::string& str);	// преобразует строку в нижний регистр

// возвращает длину строки, включая символ '0' в конце
inline uint full_length(const std::string& str) {
	return str.length() + 1;
}

// копирует по адресу dest строку source, включая символ '0' в конце
// возвращает полную длину строки (включая символ '0' в конце)
inline uint strcopy(char* dest, const std::string& source) {
	uint len = full_length(source);
	memcpy(dest, source.c_str(), len);
	return len;
}

inline uint qstrcopy(char* dest, const QString& source) {
    uint len = source.length() + 1;
    memcpy(dest, source.toStdString().c_str(), len);
    return len;
}

std::string lowercase_s(const std::string& str);	// преобразует строку в нижний регистр
bool check_empty_name(const std::string& str, const std::string& subject);	// проверяет строку на пустоту
bool check_spaces(const std::string& str, const std::string& subject);	// проверяет строку на пробелы
bool check_name(const std::string& str, const std::string& subject, const size_t minLength, const size_t maxLength);	// проверяет строку на пустоту, минимальную/максимальную длину
std::string get_value_from_console();	// получает значение с консоли
std::string get_string_from_console();	// получает строку с консоли и очищает её от начальных и конечных пробелов
std::string GetTime();	// возвращает текущее время
QString qGetTime(); // возвращает текущее время
bool QuestionYN(const std::string& message);	// задаёт вопрос с двумя вариантами ответа y/n и возвращает ответ
std::string BoolToStrR(bool value);
std::string BoolToStrE(bool value);
void PrintErrorMessage(const char error_code);

// "Склеивание" строк const char[] для передачи в SQL-запрос
/*template <typename ... Args>
SQLCHAR* BuildStatement(const Args* ...args) {
	static char buffer[SQL_BUFFER_LENGTH];
	uint	len, index = 0;
	(
		(
			len = strlen(args),		// вычисляем длину очередного параметра args
			memcpy(&buffer[index], args, len),   // копируем в буфер каждую строку из пакета параметров args
			index += len	// устанавливаем индекс
		),
		...   // распаковка пакета параметров args  - C++17
	);

	buffer[index] = 0;	// устанавливаем 0 в конце строки

	return (SQLCHAR*)buffer;
}*/


