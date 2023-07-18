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

// �������������� ������
    Server      tcpServer(&server_window);

    server_window.text_out("������������� ������� ����... ");
    server_window.BindServices(&tcpServer);  // ����������� ������ ������ "������" ��� ��������� ���������� ����������� � ����
    int iResult = tcpServer.Initialize();
    if (iResult) {
        ;   // TODO Ok
    }
    else {
        ;   // TODO error
    }

// ������������ � ���� ������
    Database database(&server_window);

    server_window.text_out("����������� � SQL-�������... ");
    server_window.BindServices(&database);  // ����������� ������ ������ "���� ������" ��� ��������� ���������� ����������� � ��
    if (database.open()) {
        ;   // TODO Ok
    }
    else {
        ;   // TODO error
    }

    server_window.atInitEnd();
    server_window.BindServices(&tcpServer, &database);  // ����������� �� � �������

    return app.exec();
}
