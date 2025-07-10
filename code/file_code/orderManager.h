#ifndef ORDERMANAGER_H
#define ORDERMANAGER_H

#include <QString>
#include <QVector>
#include <QDebug>
#include <QDateTime>
#include <QtConcurrent/QtConcurrent>  // 引入QtConcurrent
#include <QFuture>  // 引入QFuture
#include "filemanager.h"

class order
{
public:
    int     order_id;              // 序号
    int     line_id;               // 运营线路编码
    int     train_id;              // 列车编码
    int     station_id;            // 站点id
    QDateTime timestamp;           // 时间戳 (合并的年月日+时间)
    int     boarding_count;        // 上客量
    int     alighting_count;       // 下客量
    double  price;                 // 车票价格
    QString seat_type_code;        // 座位类型编码
    QString start_station_telecode; // 起始站电报码
    QString start_station_name;    // 起点站名称
    QString end_station_telecode;  // 终点站电报码
    QString end_station_name;      // 终到站名称
    double  income;                // 收入

    order() = default;
    order(int id, int l_id, int t_id, int s_id, QDateTime ts,
          int board_count, int alight_count, double p,
          QString seat_code, QString start_tel, QString start_name,
          QString end_tel, QString end_name, double rev)
        : order_id(id), line_id(l_id), train_id(t_id), station_id(s_id),
        timestamp(ts), boarding_count(board_count), alighting_count(alight_count),
        price(p), seat_type_code(seat_code), start_station_telecode(start_tel),
        start_station_name(start_name), end_station_telecode(end_tel),
        end_station_name(end_name), income(rev) {}
};

class orderManager
{
public:
    QVector<order> orders;

    orderManager();
    bool convertOrderData(const QVector<forder>& rawFile);
    void qdebugShow() const;


private:
    QDateTime combineTimestamp(const QString& date, const QString& time);
    void processOrderData(const forder& data);

    QMutex ordersMutex;  // 用于保护 orders 的互斥锁
};

#endif // ORDERMANAGER_H
