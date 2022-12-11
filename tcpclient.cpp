#include "tcpclient.h"
#include <QHostAddress>
#include <QTime>
#include <QCoreApplication>

using namespace std;
TCPClient TCPClient::tcpclient_instance;
QTcpSocket TCPClient::socket;

TCPClient::TCPClient(QObject *parent)
    : QObject{parent}
{

}

TCPClient::~TCPClient()
{

}

TCPClient* TCPClient::getInstance()
{
    return &tcpclient_instance;
}

void TCPClient::configAndrun()
{
    socket.setSocketOption(QAbstractSocket::KeepAliveOption,1);
    socket.connectToHost(server_ip, server_port);
    connect(&socket,&QTcpSocket::connected,this,&TCPClient::connected);
    connect(&socket,&QTcpSocket::disconnected,this,&TCPClient::disconnected);
    connect(&socket,&QTcpSocket::readyRead,this,&TCPClient::readSocket);
}

void TCPClient::sendToserver(QByteArray &data)
{
    if (socket.isOpen())
    {
        socket.write(data);
        socket.waitForBytesWritten(2000);
    }
}

void TCPClient::setServer(QString IP, qint16 port)
{
    server_ip = IP;
    server_port = port;
}

void TCPClient::connected()
{
    emit TCPClientConnected();
    QString addr_t = socket.peerAddress().toString();
    QByteArray addr = addr_t.toLocal8Bit();
    printf("Connected to server at %.*s:%d.\n",addr.size(),addr.data(),socket.peerPort());
}

void TCPClient::disconnected()
{
    emit TCPClientDisconnected();
    printf("Disconnected from server.\n");
}

void TCPClient::readSocket()
{
    QByteArray recv;
    while(socket.bytesAvailable())//循环接收
    {
        recv += socket.readAll();
        if (!socket.waitForReadyRead(20))//超时停止接收
        {
            /*QString error = sock->errorString();
                string error_t =  error.toStdString();
                log_w("error:%d: %.*s", sock->error(),error_t.length(), error_t.data());*/
            break;
        }
    }
    QString ip = socket.peerAddress().toString();
    string ip_t = ip.toStdString();
    printf("TCPC: Recvd from %.*s:%d\n", ip_t.length(), ip_t.data(), socket.peerPort());
    printf("TCPC Message:%.*s\n",recv.size(),(char *)recv.data());
    if (_callback != nullptr)
        _callback(ip, socket.peerPort(),recv);
}

void TCPClient::setCallBack(void (*callback)(QString &mac, quint16 port, QByteArray &data))
{
    _callback = callback;
}

void TCPClient::_sleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
