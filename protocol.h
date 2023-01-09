#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QObject>
#include <QJsonObject>

class Protocol : public QObject
{
    Q_OBJECT
public:
    static Protocol* getInstance();
    template<typename  Type, typename ...Args>
    QByteArray data_maker(Type type, Args ...args)
    {
        QList<any_types> args_list;
        args_list.append(any_types(type));
        (void)std::initializer_list <int> {(args_list.append(any_types(args)),0)...};
        return data_builder(args_list);
    };
    enum data_types {MESSAGE,FILE,REQUEST,ONLINEUSERS,RESPONSE};
    QPair<data_types,QJsonObject> data_parser(QByteArray data);
    QByteArray data_encrypt(QByteArray src);
    void set_data_cipher_word(char word);
    QByteArray data_decrypt(QByteArray src);
private:
    char cipher_word = 'G';
    struct any_types {
      enum type {String, Bool, Int, Data_types, List};
      any_types(int   e) { m_data.INT    = e; m_type = Int;}
      any_types(QString e) { m_data.STRING=e; m_type = String;}
      any_types(QList<QString> e) { m_data.LIST=e; m_type = List;}
      any_types(bool e) { m_data.BOOL = e; m_type = Bool;}
      any_types(data_types e) { m_data.DATA_TYPES = e; m_type = Data_types;}
      type get_type() const { return m_type; }
      int get_int() const { return m_data.INT; }
      bool get_bool() const { return m_data.BOOL; }
      data_types get_data_types() const  { return m_data.DATA_TYPES; }
      QString get_string() const { return m_data.STRING; }
      QList<QString> get_list() const { return m_data.LIST; }
    private:
      type m_type;
      struct {
        int   INT;
        bool BOOL;
        QString STRING;
        QList<QString> LIST;
        data_types DATA_TYPES;
      } m_data;
    };
    static Protocol protocol_instance;
    explicit Protocol(QObject *parent = nullptr);
    ~Protocol();
    QByteArray data_builder(QList<any_types> &args);
    QByteArray XOR_En_Decrypt(QByteArray src);
    QByteArray Upper_Lower_En_Decrypt(QString src);
signals:

};
#endif // PROTOCOL_H
