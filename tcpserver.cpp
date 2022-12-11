#include "tcpserver.h"

using namespace std;

TCPServer TCPServer::tcpserver_instance;

TCPServer::TCPServer(QObject *parent)
    : QObject{parent}
{

}

TCPServer::~TCPServer()
{

}

TCPServer* TCPServer::getInstance()
{
    return &tcpserver_instance;
}

void TCPServer::configAndrun()
{
    tcp_server.listen(QHostAddress(listen_addr), listen_port);
    connect(&tcp_server, &QTcpServer::newConnection, this, &TCPServer::acceptTCPConnection);
    QByteArray addr = listen_addr.toLocal8Bit();
    printf("TCP Server Started at %.*s:%d.\n", addr.length(), addr.data(),listen_port);
}

void TCPServer::setServer(QString IP, qint16 port)
{
    listen_addr = IP;
    listen_port = port;
}

void TCPServer::stopRun()
{
    tcp_server.close();
    QByteArray addr = listen_addr.toLocal8Bit();
   printf("TCP Server Closed at %.*s:%d.\n",addr.length(), addr.data(),listen_port);
}

void TCPServer::acceptTCPConnection()
{
    QTcpSocket* clientConnection = tcp_server.nextPendingConnection();
    if(clientConnection!=nullptr)
    {
        clientConnection->setParent(this);
        tcp_socket_list.append(clientConnection);
        connect(clientConnection, &QTcpSocket::readyRead, this, &TCPServer::TCPReadPeer);
        connect(clientConnection,&QTcpSocket::disconnected,this,&TCPServer::discTCPConnection);
        QString ip = clientConnection->peerAddress().toString();
        string ip_t = ip.toStdString();
        printf("TCP: %.*s:%d connected.\n",ip_t.length(), ip_t.data(), clientConnection->peerPort());
    }
}

void TCPServer::discTCPConnection()
{
    for (int i=0;i<tcp_socket_list.length();i++)
    {
        if (tcp_socket_list[i] == dynamic_cast<QTcpSocket*>(sender()))//可以直接用sender()解决，此处为稳健起见还是采取遍历
        {
            QString ip = tcp_socket_list[i]->peerAddress().toString();
            string ip_t = ip.toStdString();
            printf("TCP: %.*s:%d Disconnected.\n",ip_t.length(), ip_t.data(), tcp_socket_list[i]->peerPort());
            if (disccallback != nullptr)
                disccallback(ip, tcp_socket_list[i]->peerPort());
            tcp_socket_list.removeAt(i);
            dynamic_cast<QTcpSocket*>(sender())->deleteLater();
            break;
        }
    }
}

void TCPServer::TCPReadPeer()
{
    QTcpSocket* sock =(QTcpSocket *)sender();
    QByteArray recv;
    while(sock->bytesAvailable())//循环接收
    {
        recv += sock->readAll();
        if (!sock->waitForReadyRead(20))//超时停止接收
        {
            /*QString error = sock->errorString();
                string error_t =  error.toStdString();
                log_w("error:%d: %.*s", sock->error(),error_t.length(), error_t.data());*/
            break;
        }
    }
    QString ip = sock->peerAddress().toString();
    string ip_t = ip.toStdString();
    printf("TCPC: Recvd from %.*s:%d\n", ip_t.length(), ip_t.data(), sock->peerPort());
    printf("TCP Message:%.*s\n",recv.size(),(char *)recv.data());
    if (recvcallback != nullptr)
        recvcallback(ip, sock->peerPort(),recv);
}

void TCPServer::setCallBack(void (*callback)(QString &ip, quint16 port, QByteArray &data))
{
    recvcallback = callback;
}

void TCPServer::setCallBack(void (*callback)(QString &ip, quint16 port))
{
    disccallback = callback;
}

void TCPServer::sendToclient(QString ip, quint16 port, QByteArray data)
{
    QHostAddress addr(ip);
    for (int i=0;i<tcp_socket_list.length();i++)
    {
        if (addr == tcp_socket_list[i]->peerAddress()&&tcp_socket_list[i]->peerPort()==port)
        {
            tcp_socket_list[i]->write(data);
            break;
        }
    }
}
