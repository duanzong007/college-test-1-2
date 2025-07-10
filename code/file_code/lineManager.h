#ifndef LINEMANAGER_H
#define LINEMANAGER_H
#include <QString>
#include <QVector>
#include <QMap>
#include "filemanager.h"
class line{
public:
    int seq_in_line;             //线路站点id
    int station_id;              //站点id
    int prev_station_id;         //上一站id
    int next_station_id;         //下一站id
    int interstation_distance;   //运行线路站间距离（与上一站）
    QString line_code;           //线路代码
    bool is_stop;                //是否停靠

};


class lineManager
{
public:
    QMap<int,QVector<line>> lines;//键为线路id


    lineManager();

    //从filemanager中导入数据，如果lines有数据就返回真值
    bool convertLineData(const QVector<fline>& rawFile);
    //qdebug输出stations中所有内容
    void qdebugShow() const;
};

#endif // LINEMANAGER_H
