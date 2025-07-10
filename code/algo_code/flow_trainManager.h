#ifndef FLOW_TRAINMANAGER_H
#define FLOW_TRAINMANAGER_H
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QVector>
#include <QMap>
#include <QString>
#include <QDebug>
#include <QSet>
#include "file_code\orderManager.h"
#include "file_code\lineManager.h"
#include "file_code\stationmanager.h"
#include "file_code\trainManager.h"

enum Direction {
    Forward,
    Backward
};

struct flow_train_station{
    int station_id;
    int seq_in_line;
    QTime time;
    int people;
};

struct flow_train{
    int train_id;
    int line_id;
    int start_station_id;
    int end_station_id;
    int capacity;
    Direction direction;
    QVector<flow_train_station> stations;
};



class flow_trainManager
{
public:
    QMap<QDate, QMap<int, flow_train>> flow_trains;//第二级别为列车id
    void runFlowManager(const QMap<int, QVector<line>> &lines,
                        const QVector<order> &orders,
                        const QVector<station> &stations,
                        const QVector<train> &trains);
    void qdebugShow() const;


    flow_trainManager() {}
};

#endif // FLOW_TRAINMANAGER_H
