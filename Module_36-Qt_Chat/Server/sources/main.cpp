#include "StdAfx.h"

#include <QApplication>
#include <QTextCodec>

#include "serverwindow.h"


int main(int argc, char *argv[]) {
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1251"));

    QApplication app(argc, argv);

    ServerWindow server_window;
    server_window.show();
    server_window.atInitBegin();

// инициализируем сервер
    Server      tcpServer(&server_window);

    server_window.text_out("Инициализация сервера чата... ");
    server_window.BindServices(&tcpServer);  // привязываем объект класса "Сервер" для получения результата подключения к сети
    int iResult = tcpServer.Initialize();
    if (iResult) {
        ;   // TODO Ok
    }
    else {
        ;   // TODO error
    }

// подключаемся в базе данных
    Database database(&server_window);

    server_window.text_out("Подключение к SQL-серверу... ");
    server_window.BindServices(&database);  // привязываем объект класса "База данных" для получения результата подключения к БД
    if (database.open()) {
        ;   // TODO Ok
    }
    else {
        ;   // TODO error
    }

    server_window.atInitEnd();
    server_window.BindServices(&tcpServer, &database);  // привязываем БД к серверу

    return app.exec();
}
