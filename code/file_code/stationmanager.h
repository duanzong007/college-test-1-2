#ifndef STATIONMANAGER_H
#define STATIONMANAGER_H

#include <QVector>
#include "filemanager.h"

class station
{
public:
    int     station_id;        // 站点 id
    QString station_name;      // 站点名称
    bool    is_in_service;     // 是否在运用
    QString station_code;      // 站点代码
    QString station_telecode;  // 客运站编号
    QString station_shortname; // 站点缩写

    station() = default;
    station(int id, QString name, bool svc,
            QString code, QString tele, QString shortn) :
        station_id(id),
        station_name(std::move(name)),
        is_in_service(svc),
        station_code(std::move(code)),
        station_telecode(std::move(tele)),
        station_shortname(std::move(shortn))
    {}
};

class stationManager
{
public:
    QVector<station> stations;


    stationManager();

    //从filemanager中导入数据，如果stations有数据就返回真值
    bool convertStationData(const QVector<fstation>& rawFile);
    //qdebug输出stations中所有内容
    void qdebugShow() const;
};


#endif // STATIONMANAGER_H
