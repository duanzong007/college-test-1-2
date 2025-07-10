#ifndef FLOW_STATIONMANAGER_H
#define FLOW_STATIONMANAGER_H
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QVector>
#include <QMap>
#include <QString>
#include <QDebug>
#include "file_code\orderManager.h"
#include "file_code\lineManager.h"
#include "file_code\stationmanager.h"
#include "file_code\trainManager.h"



struct flow_station{
    int station_id;
    int boarding_count = 0;
    int alighting_count = 0;
    int ab_count = 0;

};


class flow_stationManager
{
public:
    QMap<QDateTime, QMap<int,flow_station>> flow_stations;
    void runFlowManager( const QVector<order> &orders);
    void qdebugShow() const;

    flow_stationManager() {}
};

#endif // FLOW_STATIONMANAGER_H
