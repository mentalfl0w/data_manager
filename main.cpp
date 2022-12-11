#include <QCoreApplication>
#include "protocol.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Protocol *p = Protocol::getInstance();
    QList<QString> users{QString("123321123"),QString("123321123"),QString("123321123"),QString("123321123")};
    qDebug()<<p->data_parser(p->data_maker(Protocol::ONLINEUSERS,4,users));
    qDebug()<<p->data_encrypt(p->data_maker(Protocol::ONLINEUSERS,4,users));
    qDebug()<<p->data_decrypt(p->data_encrypt(p->data_maker(Protocol::ONLINEUSERS,4,users)));
    return a.exec();
}
