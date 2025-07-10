#ifndef P__FILEMANAGER_H
#define P__FILEMANAGER_H

#include <QObject>
#include <QDateTime>
#include <QMap>
#include <QVector>
#include <QPair>
#include <QColor>
#include "core_system.h"

struct LineStation {
    int station_id;          // 站点 ID
    int seq_in_line;        // 站点在线路上的顺序（seq）
    double jing;             // 经度
    double wei;              // 纬度

    LineStation(int _station_id = -1, int _seq_in_line = 0, double _jing = 0.0, double _wei = 0.0)
        : station_id(_station_id), seq_in_line(_seq_in_line), jing(_jing), wei(_wei) {}
};

struct Line {
    int id;                  // 线路 ID
    QString startStation;    // 起始站
    QString endStation;      // 终点站
    QVector<LineStation> stations; // 按照顺序存储站点（包括站点 ID 和经纬度）
    QColor color;            // 线路颜色
    double capacityMatch;    // 运力匹配度，初始化为 -1
    bool visible;            // 可见性，初始化为 false

    Line(int _id = -1, QString _start = "", QString _end = "", QColor _color = Qt::gray,
         double _capacityMatch = -1.0, bool _visible = false)
        : id(_id), startStation(_start), endStation(_end), color(_color),
        capacityMatch(_capacityMatch), visible(_visible) {}
};

struct Train {
    int id;
    QString code;
    int currentPeople;
    int capacity;
    double loadRate;
    QString startStation;
    QString endStation;
    int lineId;
    QPair<double, double> position;
    bool visible =false;

    Train(int _id = -1, QString _code = "", int _people = 0, int _capacity = 0,
          double _loadRate = 0.0, QString _startStation = "", QString _endStation = "",
          int _lineId = -1, QPair<double, double> _position = QPair<double, double>())
        : id(_id), code(_code), currentPeople(_people), capacity(_capacity), loadRate(_loadRate),
        startStation(_startStation), endStation(_endStation), lineId(_lineId), position(_position) {}
};


class p0_fileManager : public QObject
{
    Q_OBJECT                       // ← 必须加
public:
    explicit p0_fileManager(QObject* parent=nullptr);

    void set_system(core_system* s){system = s;}

    // 用于预加载线路的函数
    void preloadLinesData();


private:
    core_system* system;

    QMap<int, QString> station_id_to_name;
    QMap<int, QString> train_id_to_code;


    // 存储与当前时间戳相关的动态线路数据
    QMap<int, Line> m_dynamicLines;//int为线路id

    // 存储与当前时间戳相关的动态列车数据
    QMap<int, Train> m_dynamicTrains;//int为列车id

    // 根据当前运行的列车去计算运力匹配度
    void filterLinesByTrain();

    // 根据时间筛选出正在运行的全部列车
    void filterTrainsByTime(const QDateTime& selectedTime);

    // 辅助函数
    static bool isTrainRunningNow(const flow_train& train,
                                  const QDateTime&  selectedTime);
    static bool updateTrainRuntimeData(const flow_train& fTrain,
                                       const QDateTime&  selTime,
                                       core_system*      system,
                                       Train&            trainData);

public slots:
    // 时间变化时的处理
    void onTimeChanged(const QDateTime& newTime);

    // 处理线路可见性变化
    void onLineVisibilityChanged(int lineId, bool visible);

    void cancelChoose();

signals:
    // 发送线路数据到地图
    void sendLinesToMap(const QMap<int, Line>& lines);
    void sendTrainsToMap(const QMap<int, Train>& trains);

    // 发送线路数据到 LineWindow
    void sendLinesToLineWindow(const QMap<int, Line>& lines);

    // 发送列车数据到 TrainWindow
    void sendTrainsToTrainWindow(const QMap<int, Train>& trains);
};

#endif // P__FILEMANAGER_H
