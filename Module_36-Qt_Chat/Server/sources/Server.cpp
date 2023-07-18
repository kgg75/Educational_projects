#include "StdAfx.h"

#include "Users.h"
#include "Messages.h"
#include "Server.h"
#include "DBFunctions.h"
//#include "../../_common_files/Constants.h"
#include "../../_common_files/ServiceFunc.h"


using namespace std;


Server::Server(QObject* parent) {
    _request_msg.resize(BUFFER_LENGTH);
    _response_msg.resize(BUFFER_LENGTH);
}


Server::~Server() {
    _server.close();
}


int Server::Initialize() {
    bool bResult = _server.listen(QHostAddress(SERVER_ADDRESS), PORT_NUMBER);

    connect(&_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    connect(&_server, SIGNAL(disconnected()), this, SLOT(disconnected()));

    emit atNetStart(bResult ? QString::fromLocal8Bit("���������.\n") : _server.errorString());
    emit StatusChanged(QString::fromLocal8Bit("�����������: 0"));

    return bResult;
}


void Server::newConnection() {
    while (_server.hasPendingConnections()) {
        QTcpSocket* socket = _server.nextPendingConnection();
        connect(socket, SIGNAL(readyRead()), SLOT(ReadRequest()));
        _users_map.emplace(socket, 0);

        emit NewMessage(qGetTime() + QString::fromLocal8Bit(" ����������� ������ #") + QString::number(socket->socketDescriptor()) + "\n");
        emit StatusChanged(QString::fromLocal8Bit("�����������: ") + QString::number(_users_map.size()));
    }
}


void Server::disconnected() {
    QTcpSocket* socket = static_cast<QTcpSocket*>(sender());
    QMessageBox qmBox(QMessageBox::Warning,
                      "disconnected",
                      QString::number(socket->socketDescriptor()),
                      QMessageBox::Ok, 0, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    qmBox.exec();
}


int Server::PutConnections(QStringList& connections_list) {
    for (map <QTcpSocket*, uint>::iterator it = _users_map.begin(); it != _users_map.end(); ) {
        if (it->first->socketDescriptor() == -1)    // ������ ���������
            it = _users_map.erase(it);
        else
            ++it;
    }

    if (!_users_map.empty()) {
        QString qtext;
        string login;
        for (map <QTcpSocket*, uint>::iterator it = _users_map.begin(); it != _users_map.end(); ++it) {
            qtext = QString::number(it->first->socketDescriptor()) + " - ";
            if (it->second != 0) {
                int iResult;
                emit iResult = GetLogin_fromDB(_users_map.at(it->first), login);
                qtext += QString::fromStdString(login) + '\n';
            }
            else {
                qtext += QString::fromLocal8Bit("<���� �� ��������>\n");
            }
            connections_list.append(qtext);
        }

    }

    return _users_map.size();
}


// ��������� ������� ��������� - ������
int Server::ReadRequest() {
    QTcpSocket* socket = static_cast<QTcpSocket*>(sender());

    int iResult = socket->bytesAvailable();

    if (iResult <= 0)
        return -1;
    else if (iResult > (int)BUFFER_LENGTH)
        return -2;
    else {
        _request_msg.clear();
        _request_msg.append(socket->readAll());
        RequestHandler(socket);
    }
    return iResult;
}


int Server::SendResponse(QTcpSocket* socket/*, const char* local_response_msg, const uint local_response_msg_length*/) {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        int iResult = socket->write(_response_msg.data(), _response_msg.size());
        socket->waitForBytesWritten();
        if (iResult)
            return MSG_SUCCESS_RESULT;
    }

    return ERR_SERVER_ERROR;
}


void Server::RequestHandler(QTcpSocket* socket) {
    std::string	login,
                name,
                surname,
                email,
                //receiver,
                message;
    int         iResult = 0;
    uint        id,
                receiverId,
                index = 1;

    _response_msg.clear();
    _response_msg[0] = MSG_SUCCESS_RESULT;	// ������������� �������� ������ (1-� ����) �� ���������

    switch (_request_msg[0]) {
        case GCMD_REG:	{// ������� reg - ������������������
                login = string(&(_request_msg.data()[index]));

                index += full_length(login);
                name = string(&(_request_msg.data()[index]));

                index += full_length(name);
                surname = string(&(_request_msg.data()[index]));

                index += full_length(surname);
                email = string(&(_request_msg.data()[index]));

                index += full_length(email);

                emit iResult = AddUser_toDB(login, name, surname, email, (SHA1PwdArray&)_request_msg.data()[index]/*, &iResult*/);

                if (iResult > 0) {	// ����������� ������ �������
                    emit NewMessage(qGetTime() + QString::fromLocal8Bit(" �� ������ #") + QString::number(socket->socketDescriptor()) + QString::fromLocal8Bit(" ����������������� ������������ '") + QString::fromStdString(login) + "'\n");
                }
                else
                    _response_msg[0] = ERR_REQUEST_ERROR;	// ����������� �����
            }
            break;
        case GCMD_LOGIN:	// ������� login - ����
            login = string(&(_request_msg.data()[index]));
            emit iResult = GetUserId_fromDB(login);

            if (iResult > 0) {
                id = iResult;
                for (map <QTcpSocket*, uint>::iterator it = _users_map.begin(); it != _users_map.end(); it++)
                    if (it->second == id) {	// ������������ � ����� id ��� �����
                        _response_msg[0] = ERR_USER_ALREADY_LOGGED;
                        break;
                    }

                if (_response_msg[0] != ERR_USER_ALREADY_LOGGED) {
                    index += full_length(login);
                    emit iResult = CheckHash_withDB(id, (SHA1PwdArray&)_request_msg.data()[index]/*, &iResult*/);	// ������� ������ �� �������� ���e�� � ������� ����
                    if (iResult > 0) {	// login ������ �������
                        _users_map.find(socket)->second = iResult;	// ��������� id ������������ � �������
                        emit SetLastVisit_toDB(_users_map.at(socket));	// �� ������ ���� ������������� ������� ����/����� ��� ����/����� ���������� ���������
                        emit NewMessage(qGetTime() + QString::fromLocal8Bit(" �� ������ #") + QString::number(socket->socketDescriptor()) + QString::fromLocal8Bit(" ����� � ��� ������������ '") + QString::fromStdString(login) + "'\n");
                    }
                }
            }
            else    // ������ ������
                _response_msg += (char)iResult;	// ����������� �����
            break;
        case LCMD_LOGOUT:	// ��������� ������� logout - �����
            emit SetLastVisit_toDB(_users_map.at(socket));
            emit iResult = GetLogin_fromDB(_users_map.at(socket), login);
            emit NewMessage(qGetTime() + QString::fromLocal8Bit(" �� ������ #") + QString::number(socket->socketDescriptor()) + QString::fromLocal8Bit(" ����� �� ���� ������������ '") + QString::fromStdString(login) + "'\n");
            _users_map.find(socket)->second = 0;
            break;
        case LCMD_USERS_LIST: {
                QStringList users_list;
                emit iResult = GetUsersList_fromDB(users_list);
                if (iResult > 0) {
                    for (auto it : users_list) {
                        _response_msg.append(it + '\0');
                    }
                }
            }
            break;
        case LCMD_SEND:
            receiverId = static_cast<uint>(_request_msg.data()[index]); // ������������� id ����������

            message = string(&(_request_msg.data()[index + sizeof(uint)]));

            //emit NewMessage("<" + QString::number(receiverId) + "> " + QString::fromStdString(message) + '\n');
            emit iResult = AddMessage_toDB(_users_map.find(socket)->second, receiverId, message);

            if (iResult > 0) {	// ��� ������
                id = iResult;	// ��������� id ���������
                emit iResult = GetLogin_fromDB(_users_map.at(socket), login);  // �������� login ����������
                if (iResult == MSG_SUCCESS_RESULT) {
                    for (map <QTcpSocket*, uint>::iterator it = _users_map.begin(); it != _users_map.end(); it++) {
                        if (it->second == receiverId) {	// ���������� ��������� ��������� � ����
                            _response_msg[0] = LCMD_RECEIVE;
                            _response_msg.append(QString::fromStdString(login) + '\0');
                            _response_msg.append(QString::fromStdString(message) + '\0');

                            SendResponse(it->first);	// ���������� ��������� ���������� �� ���������� it->first
                            ReadRequest();	// ������ �������� ����� MSG_SUCCESS_RESULT

                            if (_request_msg[0] == MSG_SUCCESS_RESULT)
                                emit SetReadedState_toDB(receiverId, id);	// �������� �����������
                            break;
                        }
                    }
                }
            }

            _response_msg += (char)iResult;	// ����������� �����
            break;
        case LCMD_READ: {
                QStringList messages_list;
                receiverId = _users_map.find(socket)->second;
                emit iResult = GetMessagesList_fromDB(messages_list, receiverId, false);
                if (iResult > 0) {
                    for (auto it : messages_list) {
                        _response_msg.append(it + '\0');
                    }
                }
                if (iResult == ERR_REQUEST_ERROR)
                    iResult = ERR_SERVER_ERROR;
            }
            _response_msg += (char)iResult;	// ����������� �����
            break;
        default:
            break;
    }

    iResult = SendResponse(socket);
}

