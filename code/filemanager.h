#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <Qvector>
#include <Qstring>

class fstation{//对应原文件：客运站点（站点名称、站点编号、备注）
public:
    int zdid;//站点id
    QString xlid;//线路id
    QString ysfsbm;//运输方式编码
    QString zdmc;//站点名称
    QString sfty;//是否停运
    QString station_code;//车站代码
    QString station_telecode;//车站电报码
    QString station_shortname;//车站简称

    fstation(int a,QString f,QString g,QString b,QString h,QString c,QString d,QString e)
        :zdid(a),xlid(f),ysfsbm(g),zdmc(b),sfty(h),station_code(c),station_telecode(d),station_shortname(e){}

};

class ftrain{//对应原文件：列车表（列车编码、列车代码、列车运量）
public:
    int lcbm;//列车编码
    QString ysfsbm;//运输方式编码
    QString lcdm;//列车代码
    QString cc;//车次
    QString sfzt;//是否正常
    QString lcyn;//列车运量

    ftrain(int a,QString c,QString d,QString e,QString f, QString g)
        :lcbm(a),ysfsbm(c),lcdm(d),cc(e),sfzt(f),lcyn(g){}
};

class fline{//对应原文件：运营线路客运站（运营线路编码、站点id、线路站点id、上一站id、运营线路站间距离 、下一站id、运输距离、线路代码）
public:
    int yyxlbm;//运行线路编码
    int zdid;//站点id
    QString xlzdid;//线路站点id
    QString Q_zdid;//上一站id
    QString yqzdjjl;//预期站点间距离
    QString H_zdid;//下一站id
    QString sfqszd;//是否起始站点
    QString sfzdzd;//是否终点站点
    QString ysjl;//运输距离
    QString xldm;//线路代码
    QString sfytk;//是否要停靠

    fline(int a,int b,QString c,QString d,QString e,QString f, QString g,QString k,QString h,QString i,QString j)
        :yyxlbm(a),zdid(b),xlzdid(c),Q_zdid(d),yqzdjjl(e),H_zdid(f),sfqszd(g),sfzdzd(k),ysjl(h),xldm(i),sfytk(j){}
};

class forder {//对应原文件：高铁客运量（成都--重庆）
    //（运营线路编码、列车编码、站点id、日期、到达时间、出发时间、
    // 上客量、下客量等，起点站、终点站、票价、收入等）
public:
    int xh;//序号
    int yyxlbm;//运行线路编码
    int lcbm;//列车编码
    int zdid;//站点id
    QString yxrq;//运行日期
    QString yxsj;//运行时间
    QString yqdzjjl;//与起点站距距离
    QString sfqszd;//是否起始站点
    QString sfzdzd;//是否终点站点
    QString ddsj;//到达时间
    QString cfsj;//出发时间
    QString kll;//客流量
    QString skl;//上客量
    QString xkl;//下客量
    QString ticket_type;//车票类型
    QString ticket_price;//车票价格
    QString seat_type_code;//座位类型代码
    QString train_coporation_code;//列车公司代码
    QString ksrq;//开始日期
    QString start_station_telecode;//起点站电报码
    QString start_station_name;//起点站名称
    QString end_station_telecode;//终点站电报码
    QString end_station_name;//终点站名称
    QString train_class_code;//列车等级代码
    QString train_type_code;//列车类型代码
    QString sale_station_telecode;//售票站电报码
    QString limit_station_telecode;//最远到达站电报码
    QString spsj;//售票时间
    QString to_station_telecode;//到达站电报码
    QString shouru;//收入


    forder(int a, int b, int c, int d, QString e, QString f, QString g, QString h, QString i,
           QString j, QString k, QString l, QString m, QString n, QString o, QString p, QString q,
           QString r, QString s, QString t, QString u, QString v, QString w, QString x, QString y,
           QString z, QString aa, QString bb, QString cc, QString dd)
        : xh(a), yyxlbm(b), lcbm(c), zdid(d), yxrq(e),
        yxsj(f), yqdzjjl(g), sfqszd(h), sfzdzd(i),ddsj(j),
        cfsj(k), kll(l), skl(m), xkl(n), ticket_type(o),
        ticket_price(p), seat_type_code(q),train_coporation_code(r), ksrq(s), start_station_telecode(t),
        start_station_name(u),end_station_telecode(v), end_station_name(w), train_class_code(x), train_type_code(y),
        sale_station_telecode(z), limit_station_telecode(aa), spsj(bb), to_station_telecode(cc),shouru(dd) {}




};


class fileManager
{
public:
    QVector<fstation> station_file;
    QVector<ftrain> train_file;
    QVector<fline> line_file;
    QVector<forder> order_file;

    fileManager();
    ~fileManager();
    bool inputStation();
    bool inputTrain();
    bool inputLine();
    bool inputOrder();

    void debugShowStation();
    void debugShowTrain();
    void debugShowLine();
    void debugShowOrder();

    bool loadAllData();


    bool saveLines(const QString& tempPath = "lines.csv");
    bool saveTrains(const QString& tempPath = "trains.csv");
    bool saveStations(const QString& tempPath = "stations.csv");
    bool saveOrders(const QString& tempPath = "orders.csv");
};


#endif // FILEMANAGER_H
