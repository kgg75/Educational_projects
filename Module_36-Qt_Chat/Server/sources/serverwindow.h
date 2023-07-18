#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QTextEdit>
#include <QMainWindow>

#include "Server.h"
#include "DBFunctions.h"


QT_BEGIN_NAMESPACE
namespace Ui { class ServerWindow; }
QT_END_NAMESPACE

class Server;
class Database;

class ServerWindow : public QMainWindow {
    Q_OBJECT

    public:
        ServerWindow(QWidget *parent = nullptr);
        ~ServerWindow();
        void atInitBegin();
        void atInitEnd();
        void qtext_out(const QString& qstr);
        void text_out(const char* str);
        //void mtext_out(const QString& qstr);

        template <typename ... Args>
        void mtext_out(const Args* ...args) {
            static char buffer[1024];
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

            text_out(buffer);
        }
        void BindServices(const Server* server);
        void BindServices(const Database* database);
        void BindServices(const Server* server, const Database* database);
        void SetStatusText(const QString& qstr);

    private slots:
        void on_pushBtnConnections_clicked();
        void on_pushBtnUsers_clicked();
        void on_pushBtnMessages_clicked();
        void on_menuFont_triggered();
        void on_menuWindowColor_triggered();
        void on_menuFontColor_triggered();
        void on_menuExit_triggered();
        void on_menuAbout_triggered();
        void on_menuClear_triggered();
        void on_menuConnections_triggered();
        void on_menuUsers_triggered();
        void on_menuMessages_triggered();
        void on_menuLock_triggered();

    signals:
        int GetConnections(QStringList& connections_list);
        int GetUsersList_fromDB(QStringList& users_list);
        int GetUsersDetailList_fromDB(QStringList& users_list);
        int GetMessagesFullList_fromDB(QStringList& messages_list);
        int SetIsLocked_inDB(const uint id);

    private:
        Ui::ServerWindow* ui;
        QColor color, fontColor;
        QFont font;

};
#endif // SERVERWINDOW_H
