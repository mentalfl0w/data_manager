#include "protocol.h"
#include <QJsonDocument>
#include <QJsonArray>

Protocol Protocol::protocol_instance;
Protocol::Protocol(QObject *parent)
    : QObject{parent}
{

}

Protocol::~Protocol()
{

}

Protocol* Protocol::getInstance()
{
    return &protocol_instance;
}

QByteArray Protocol::data_builder(QList<any_types> &args)
{
    QJsonObject data;
    switch(args[0].get_data_types())
    {
    case MESSAGE:
    {
        if (args.length()==2)
        {
            data.insert("type","message");
            data.insert("data",args[1].get_string());
        }
        else if (args.length()==3)
        {
            data.insert("type","message");
            data.insert("to_user",args[1].get_string());
            data.insert("data",args[2].get_string());
        }
        break;
    }
    case FILE:
    {
        data.insert("type","file");
        data.insert("name",args[1].get_string());
        data.insert("data",args[2].get_string());
        break;
    }
    case REQUEST:
    {
        data.insert("type","request");
        data.insert("action",args[1].get_string());
        QList<QString> content = args[2].get_list();
        QJsonObject json_content;
        for (int i=0; i<content.length(); i+=2)
            json_content.insert(content[i],content[i+1]);
        data.insert("content",json_content);
        break;
    }
    case ONLINEUSERS:
    {
        data.insert("type","online_users");
        data.insert("count",args[1].get_int());
        QList<QString> users = args[2].get_list();
        QJsonArray json_users;
        for (auto &user:users)
            json_users.append(user);
        data.insert("users",json_users);
        break;
    }
    case RESPONSE:
    {
        data.insert("type","response");
        data.insert("action",args[1].get_string());
        QList<QString> content = args[2].get_list();
        QJsonObject json_content;
        for (int i=0; i<content.length(); i+=2)
            json_content.insert(content[i],content[i+1]);
        data.insert("content",json_content);
        break;
    }
    default:
        break;
    }
    QJsonDocument data_doc(data);
    return data_doc.toJson(QJsonDocument::Compact)+'\n';
}

QPair<Protocol::data_types,QJsonObject> Protocol::data_parser(QByteArray data)
{
    QJsonDocument json_data = QJsonDocument::fromJson(data);
    if (json_data["type"] == "message")
        return qMakePair(MESSAGE,json_data.object());
    else if (json_data["type"] == "file")
        return qMakePair(FILE,json_data.object());
    else if (json_data["type"] == "request")
            return qMakePair(REQUEST,json_data.object());
    else if (json_data["type"] == "online_users")
            return qMakePair(ONLINEUSERS,json_data.object());
    else if (json_data["type"] == "response")
            return qMakePair(RESPONSE,json_data.object());
    else
        return qMakePair(RESPONSE,json_data.object());
}

QByteArray Protocol::XOR_En_Decrypt(QByteArray src)
{
    QByteArray result;
    for(auto &i:src)
        result.append(i ^ cipher_word);
    return result;
}

QByteArray Protocol::Upper_Lower_En_Decrypt(QString src)
{
    QByteArray result;
    for(auto &i:src)
    {
        if(i.isUpper())
        {
            QChar word = i.toLower();
            result.append(word.toLatin1());
        }
        else if (i.isLower())
        {
            QChar word = i.toUpper();
            result.append(word.toLatin1());
        }
        else
            result.append(i.toLatin1());
    }
    return result;
}

QByteArray Protocol::data_encrypt(QByteArray src)
{
    QByteArray en_base64 = src.toBase64();
    QByteArray result = Upper_Lower_En_Decrypt(en_base64);
    return XOR_En_Decrypt(result);
}

QByteArray Protocol::data_decrypt(QByteArray src)
{
    QByteArray result = XOR_En_Decrypt(src);
    result = Upper_Lower_En_Decrypt(result);
    return QByteArray::fromBase64(result);
}

void Protocol::set_data_cipher_word(char word)
{
    cipher_word = word;
}
