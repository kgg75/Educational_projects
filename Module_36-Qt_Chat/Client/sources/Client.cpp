//#include "StdAfx.h"

#include "Client.h"
#include "../../_common_files/ServiceFunc.h"
#include "../../_common_files/SHA1.h"


using namespace std;


//Client public:
Client::Client(/*QObject* parent*/)/* : QObject(parent)*/ {
    _isConnected = ConnectToServer();
    connect(&_socket, SIGNAL(readyRead()), SLOT(receiveData()));
}


Client::~Client() {
    _socket.close();
}


//Client public slots:
bool Client::ConnectToServer() {
    _socket.connectToHost(SERVER_ADDRESS, PORT_NUMBER);
    return _socket.waitForConnected(5000);
}


int Client::receiveData() {
    int iResult = _socket.bytesAvailable();

    if (iResult > 0 && iResult <= (int)BUFFER_LENGTH) {
        _response_msg.clear();
        _response_msg.append(_socket.readAll());
        RequestHandler();
    }
    else
        return ERR_CONNECTION_ERROR;  //??
}


int Client::sendData() {
    if(_socket.state() == QAbstractSocket::ConnectedState) {
        _socket.write(_request_msg);
        return static_cast<int>(_socket.waitForBytesWritten());
    }
    else
        return ERR_CONNECTION_ERROR;  //??
}


// �������� ����� �������� ����������� (��������/����)
int Client::MessagesExchange(/*const uchar command_code*/) {
    if (sendData() > 0)
        _socket.waitForReadyRead();

    return _response_msg.size();
}


void Client::RequestHandler() {
    QString sender, message;
    int iResult;

    switch (_response_msg[0]) {
        case LCMD_RECEIVE:
            sender = QString(&(_response_msg.data()[1]));
            message = QString(&(_response_msg.data()[1 + full_length(sender.toStdString())]));

            emit MessageReceived(sender, message);
            _request_msg.clear();
            _request_msg[0] = MSG_SUCCESS_RESULT;
            iResult = sendData();

            break;
        default:
            break;
    }
}


int Client::LoginRegister(const bool registration, const QString& login, const QString& name, const QString& surname, const QString& email, const QString& password) {
    _request_msg.clear();
    _request_msg[0] = registration ? GCMD_REG : GCMD_LOGIN;	// ��������� ��� ������� � ������ [0];
    _request_msg += login + '\0';	// �������� login

    if (registration) {   // registration
        _request_msg += name + '\0';	// �������� name
        _request_msg += surname + '\0';	// �������� surname
        _request_msg += email + '\0';	// �������� email
    }
    SHA1PwdArray pass_sha1_hash;	// ��� ������
    CalculateSHA1(pass_sha1_hash, password.toLocal8Bit());
    _request_msg.append(pass_sha1_hash.asChar(), SHA1_HASH_LENGTH_BYTES);	// �������� ���

    MessagesExchange();

    if (!registration && _response_msg[0] == MSG_SUCCESS_RESULT) {	// ������������ ������� ����� (����������� �� �������� ����!)
        _login = login;
        _isLogged = true;
    }

    return _response_msg[0];
}


int Client::Logout() {
    _request_msg.clear();
    _request_msg[0] = LCMD_LOGOUT;
    MessagesExchange();

    if (_response_msg[0] == MSG_SUCCESS_RESULT) {	// ������������ ������� �����������������
        _isLogged = false;
        emit StatusChanged(QString::fromLocal8Bit("����� �� ���� ������������ '") + _login + "'");
        emit WindowTextChanged("");
    }
    else
        ;

    return MSG_SUCCESS_RESULT;  // !!
}


int Client::GetUsersList(QStringList& users_list) {
    _request_msg.clear();
    _request_msg[0] = LCMD_USERS_LIST;

    MessagesExchange();

    if (_response_msg[0] == MSG_SUCCESS_RESULT) {
        int index = 1;
        while (index < _response_msg.size()) {
            QString qstr = QString::fromStdString(&(_response_msg.data()[index]));
            users_list.append(qstr);
            index += full_length(qstr.toStdString());
        }
    }
    else
        ;

    return _response_msg[0];
}


int Client::GetMessagesList(QStringList& messages_list) {
    _request_msg.clear();
    _request_msg[0] = LCMD_READ;

    MessagesExchange();

    if (_response_msg[0] == MSG_SUCCESS_RESULT) {
        int index = 1;
        while (index < _response_msg.size()) {
            QString qstr = QString::fromStdString(&(_response_msg.data()[index]));
            messages_list.append(qstr);
            index += full_length(qstr.toStdString());
        }
    }
    else
        ;

    return _response_msg[0];
}


int Client::SendMessage(const uint id, const QString& message) {
    _request_msg.clear();
    _request_msg[0] = LCMD_SEND;

    _request_msg.append(static_cast<char*>((void*)&id), sizeof(id));	// �������� id ����������
    _request_msg += message + '\0';	// �������� ����� ���������

    MessagesExchange();

    return _response_msg[0];  // !!
}

