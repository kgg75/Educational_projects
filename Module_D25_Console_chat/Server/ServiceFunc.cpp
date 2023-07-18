#include "StdAfx.h"

#include "ServiceFunc.h"


#if defined(max)
	#undef max
#endif


using namespace std;


uchar get_command_code(const CommandSpace* commandSpace, string& command) {	// выполняет анализ команды и возвращает соответствующий код действия
	lowercase(command);	//	преобразование текста команды в нижний регистр
	for (auto i = 0; commandSpace[i].command[0] != 0; i++)	// поиск кода команды
		if (command == commandSpace[i].command || command == commandSpace[i].command2)
			return commandSpace[i].code;
	return 0;
}


uchar get_command(const CommandSpace* commandSpace, const string console_prompt) {	// выполняет получение команды с консоли, возвращает соответствующий код действия
	cout << console_prompt;	// вывод обращения
	string command = get_value_from_console();

	return get_command_code(commandSpace, command);
}


void lowercase(string& str) {	// преобразует строку в нижний регистр
	for (auto i = 0; str[i] != 0; i++)
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] += 'a' - 'A';
}


string lowercase_s(const string& str) {	// преобразует строку в нижний регистр
	string output = str;
	for (auto i = 0; output[i] != 0; i++)
		if (output[i] >= 'A' && output[i] <= 'Z')
			output[i] += 'a' - 'A';
	return output;
}


bool check_empty_name(const string& str, const string& subject) {	// проверяет строку на пустоту
	if (str.empty()) {
		cout << "Ошибка - " << subject << " не может быть пустым.\n";
		return false;
	}
	return true;
}


bool check_spaces(const string& str, const string& subject) {	// проверяет строку на пробелы
	if (str.find_first_of(' ') != string::npos) {	// найдены пробелы
		cout << "Ошибка - " << subject << " не может содержать пробелы.\n";
		return false;
	}
	return true;
}


bool check_name(const string& str, const string& subject, const size_t minLength, const size_t maxLength) {	// проверяет строку на пустоту, минимальную/максимальную длину
	if (!check_empty_name(str, subject))
		return false;
	if ((str.size() < minLength) || (str.size() > maxLength)) {
		cout << "Ошибка - длина значения вне допустимого диапазона [" << minLength << "-" <<maxLength << "].\n";
		return false;
	}
	return true;
}


string get_value_from_console() {	// получает значение с консоли
	string value;
	while (!(cin >> value));	// noskipws?
	cin.clear(),
	cin.ignore(numeric_limits<streamsize>::max(), '\n');	// очистка буфера консоли
	return value;
}


string get_string_from_console() {	// получает строку с консоли и очищает её от начальных и конечных пробелов
	string str;
	while (str.empty())
		getline(cin, str);
	str.erase(0, str.find_first_not_of(' '));
	str.erase(str.find_last_not_of(' ') + 1, str.length());
	return str;
}


string GetTime() {	// возвращает текущее время
	const time_t t_c = chrono::system_clock::to_time_t(chrono::system_clock::now());
	stringstream ss;
	tm tmDest;
	localtime_s(&tmDest, &t_c);
	ss << put_time(&tmDest, DATE_TIME_FORMAT);	// "[%Y-%m-%d %H:%M:%S].\n";
	return ss.str();
}


bool QuestionYN(const string& message) {	// задаёт вопрос с двумя вариантами ответа y/n и возвращает ответ
	cout << message << " (y/n): ";
	string command = "";
	while ((command != "y") && (command != "n")) {
		command = get_value_from_console();
		lowercase(command);
	}
	return (command == "y") ? true : false;
}


string BoolToStrR(bool value) {
	return value ? "да" : "нет";
}


string BoolToStrE(bool value) {
	return value ? "yes" : "no";
}


void PrintErrorMessage(const char error_code) {
	auto i = 0;
	while (ERROR_SPACE[i].code != error_code && ERROR_SPACE[i].code != 0)
		i++;
	cout << ERROR_SPACE[i].text << '\n';
}


