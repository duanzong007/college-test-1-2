#ifndef IP_CHECK_H
#define IP_CHECK_H

#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QVector>
#include <QMap>
#include <QString>
#include <QDebug>
#include <QFile>


struct ip{
    int station_id;
    QString station_name;
    QString station_telecode;
    double jing;
    double wei;
    ip(){}
    ip(int a,QString b,QString c,double d,double e):
        station_id(a),station_name(b),station_telecode(c),
        jing(d),wei(e){}
};


class ip_check
{
public:
    QMap<int,ip> ips;//键为站点id
    bool inputIp();
    ip_check();

    void qdebugShow() const;




};

#endif // IP_CHECK_H
