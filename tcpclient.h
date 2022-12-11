#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class TCPClient : public QObject
{
    Q_OBJECT
public:
    static TCPClient* getInstance();
    static void sendToserver(QByteArray &data);
    void configAndrun();
    void setCallBack(void (*callback)(QString &ip, quint16 port, QByteArray &data));
    void setServer(QString IP, qint16 port);
private slots:
    void connected();
    void readSocket();
    void disconnected();
private:
    explicit TCPClient(QObject *parent = nullptr);
    ~TCPClient();
    static void _sleep(int msec);
    static QTcpSocket socket;
    static TCPClient tcpclient_instance;
    QString server_ip = "127.0.0.1";
    qint16 server_port = 7890;
    bool has_found = false;
    void (*_callback) (QString &ip, quint16 port, QByteArray &data)=nullptr;
signals:
    void TCPClientConnected();
    void TCPClientDisconnected();

signals:

};

#endif // TCPCLIENT_H
