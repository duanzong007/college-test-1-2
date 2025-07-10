// data_preload.h
#ifndef DATA_PRELOAD_H
#define DATA_PRELOAD_H

#include <QMap>
#include <QDate>
#include <QDateTime>
#include <QPair>
#include <QDebug>
#include <QtGlobal>
#include "algo_code/flow_trainManager.h"
#include "algo_code/flow_stationManager.h"

// 时间索引别名
using HourMapInt   = QMap<int, qint64>;
using DayMapInt    = QMap<int, HourMapInt>;
using MonthMapInt  = QMap<int, DayMapInt>;
using YearMapInt   = QMap<int, MonthMapInt>;

using HourMapDbl   = QMap<int, double>;
using DayMapDbl    = QMap<int, HourMapDbl>;
using MonthMapDbl  = QMap<int, DayMapDbl>;
using YearMapDbl   = QMap<int, MonthMapDbl>;

// 日/月/年 聚合别名
template<typename T> using DailyMap   = QMap<QDate, T>;
template<typename T> using MonthlyMap = QMap<QPair<int,int>, T>;
template<typename T> using YearlyMap  = QMap<int, T>;

class data_preload
{
public:
    data_preload() {
        // 站点承载能力打表（同 p1_fileManager） :contentReference[oaicite:0]{index=0}
        m_stationCapacity = {
            {810,  700}, {1037,5000}, {1640,3000},
            {1689, 500}, {1690, 300}, {1694, 200},
            {1695,5000}

        };
    }

    /** 不变的外部接口 **/
    void runData_preload(flow_trainManager* trains,
                         flow_stationManager* stations);
    void debugPrintAllData() const;

    // —— 原始小时数据 ——
    QMap<int, YearMapDbl>   lineCapacityHourly;
    QMap<int, YearMapInt>   stationFlowHourly;
    QMap<int, YearMapDbl>   stationPressureHourly;

    // —— 日/月/年 聚合 ——
    QMap<int, DailyMap<double>>   lineCapacityDaily;
    QMap<int, MonthlyMap<double>> lineCapacityMonthly;
    QMap<int, YearlyMap<double>>  lineCapacityYearly;

    QMap<int, DailyMap<qint64>>   stationFlowDaily;
    QMap<int, MonthlyMap<qint64>> stationFlowMonthly;
    QMap<int, YearlyMap<qint64>>  stationFlowYearly;

    QMap<int, DailyMap<double>>   stationPressureDaily;
    QMap<int, MonthlyMap<double>> stationPressureMonthly;
    QMap<int, YearlyMap<double>>  stationPressureYearly;

private:
    QMap<int,int> m_stationCapacity;

    // 核心步骤
    void computeLineCapacityHourly(flow_trainManager* trains);
    void computeStationDataHourly(flow_stationManager* stations);
    void computeAggregates();

    // —— 内联的辅助逻辑 ——
    static bool isTrainRunningNow(const flow_train& train,
                                  const QDateTime& selTime);
    static int  getTrainCurrentPeople(const flow_train& train,
                                     const QDateTime& selTime);
};

#endif // DATA_PRELOAD_H
