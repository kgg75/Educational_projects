#include "clientwindow.h"

#include "StdAfx.h"

#include "Client.h"
#include "../../_common_files/Constants.h"


int main(int argc, char *argv[]) {
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1251"));

    QApplication app(argc, argv);

    Client client;

    ClientWindow client_window;
    client_window.BindServices(&client);    // привязываем объект класса "Клиент" для получения результата подключения к сети
    client_window.show();

    if (emit client_window.IsConnected()) {
        client_window.SetStatusText("Подключение к серверу чата выполнено.");
    }
    else {
        QMessageBox qmBox(QMessageBox::Critical,
                          QString::fromLocal8Bit("Ошибка!"),
                          QString::fromLocal8Bit("Ошибка подключения к серверу чата.\nПрограмма будет закрыта."),
                          QMessageBox::Ok,
                          &client_window,
                          Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
        qmBox.exec();
        ;   //TODO on exit
    }

    return app.exec();
}
