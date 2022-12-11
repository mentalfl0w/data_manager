#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class TCPServer : public QObject
{
    Q_OBJECT
public:
    static TCPServer* getInstance();
    void configAndrun();
    void setServer(QString IP, qint16 port);
    void setCallBack(void (*callback)(QString &ip, quint16 port, QByteArray &data));//Qt的信号-槽机制实际就是回调函数，
    void setCallBack(void (*callback)(QString &ip, quint16 port));                  //此处为练手，不使用该机制
    void sendToclient(QString ip, quint16 port, QByteArray data);
    void stopRun();
private:
    explicit TCPServer(QObject *parent = nullptr);
    ~TCPServer();
    QString listen_addr = "0.0.0.0";
    qint16 listen_port = 7890;
    QTcpServer tcp_server;
    QList <QTcpSocket*> tcp_socket_list;
    void (*recvcallback) (QString &ip, quint16 port, QByteArray &data)=nullptr;
    void (*disccallback) (QString &ip, quint16 port)=nullptr;
    static TCPServer tcpserver_instance;
private slots:
    void acceptTCPConnection();
    void TCPReadPeer();
    void discTCPConnection();
signals:
};

#endif // TCPSERVER_H
