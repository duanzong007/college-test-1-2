// data_preload.cpp
#include "data_preload.h"
#include <algorithm>
#include <QDate>
#include <QPair>


void data_preload::runData_preload(flow_trainManager* trains,
                                   flow_stationManager* stations)
{
    qDebug()<<"data_preload开始构建";
    // 1) 清空所有容器
    lineCapacityHourly.clear();
    stationFlowHourly.clear();
    stationPressureHourly.clear();

    lineCapacityDaily.clear();
    lineCapacityMonthly.clear();
    lineCapacityYearly.clear();

    stationFlowDaily.clear();
    stationFlowMonthly.clear();
    stationFlowYearly.clear();

    stationPressureDaily.clear();
    stationPressureMonthly.clear();
    stationPressureYearly.clear();

    // 2) 小时级计算
    computeLineCapacityHourly(trains);
    computeStationDataHourly(stations);

    // 3) 日/月/年 聚合
    computeAggregates();

    qDebug()<<"data_preload构建完成";
}

// —— 判断列车是否在 selTime 时刻运行 ——
// 逻辑源自 p0_fileManager::isTrainRunningNow :contentReference[oaicite:1]{index=1}
bool data_preload::isTrainRunningNow(const flow_train& train,
                                     const QDateTime& selTime)
{
    if (train.stations.isEmpty()) return false;
    // 找到最早/最晚 seq
    const flow_train_station* first = nullptr;
    const flow_train_station* last  = nullptr;
    for (auto &st : train.stations) {
        if (!first || st.seq_in_line < first->seq_in_line) first = &st;
        if (!last  || st.seq_in_line > last->seq_in_line)   last  = &st;
    }
    if (!first || !last) return false;
    // 方向反转时交换
    if (train.direction == Backward) std::swap(first,last);
    QTime start = first->time, end = last->time, cur = selTime.time();
    return (start <= cur && cur <= end);
}

// —— 获取列车在 selTime 时刻的当前人数 ——
// 源自 p0_fileManager::updateTrainRuntimeData（只取 people） :contentReference[oaicite:2]{index=2}
int data_preload::getTrainCurrentPeople(const flow_train& train,
                                        const QDateTime& selTime)
{
    if (train.stations.isEmpty()) return 0;
    // 排序＋按方向调整
    auto vec = train.stations;
    std::sort(vec.begin(), vec.end(),
              [](auto &a, auto &b){ return a.seq_in_line < b.seq_in_line; });
    if (train.direction == Backward) std::reverse(vec.begin(), vec.end());

    QTime cur = selTime.time();
    // 找到前后站
    const flow_train_station *prev = nullptr, *next = nullptr;
    for (int i = 0; i < vec.size(); ++i) {
        if (cur < vec[i].time) {
            if (i == 0) return 0;
            prev = &vec[i-1];
            next = &vec[i];
            break;
        }
        if (cur == vec[i].time) {
            prev = next = &vec[i];
            break;
        }
    }
    if (!prev && !next) return 0;
    if (!next) next = prev;
    // 当前人数取 prev->people
    return prev->people;
}

void data_preload::computeLineCapacityHourly(flow_trainManager* trains)
{
    // 按日期 & 小时遍历
    for (auto itDate = trains->flow_trains.constBegin();
         itDate != trains->flow_trains.constEnd(); ++itDate)
    {
        QDate date = itDate.key();
        auto &trainMap = itDate.value(); // trainId → flow_train

        for (int h = 0; h < 24; ++h) {
            QDateTime dt(date, QTime(h,0));
            // 累加每条线路的总人数 & 总运力
            QMap<int, qint64> sumP, sumC;
            for (auto itT = trainMap.constBegin();
                 itT != trainMap.constEnd(); ++itT)
            {
                const auto &ft = itT.value();
                if (!isTrainRunningNow(ft, dt)) continue;
                int people = getTrainCurrentPeople(ft, dt);
                sumP[ft.line_id] += people;
                sumC[ft.line_id] += ft.capacity;
            }
            // 存入 hourly 容器
            for (auto it = sumP.constBegin(); it != sumP.constEnd(); ++it) {
                int lid = it.key();
                double match = sumC[lid] > 0
                                   ? 100.0 * double(it.value()) / double(sumC[lid])
                                   : -1.0;
                lineCapacityHourly[lid][date.year()]
                                  [date.month()]
                                  [date.day()]
                                  [h] = match;
            }
        }
    }
}

void data_preload::computeStationDataHourly(flow_stationManager* stations)
{
    // 直接遍历所有 QDateTime
    for (auto it = stations->flow_stations.constBegin();
         it != stations->flow_stations.constEnd(); ++it)
    {
        QDateTime dt = it.key();
        int Y = dt.date().year(),
            M = dt.date().month(),
            D = dt.date().day(),
            h = dt.time().hour();

        for (auto itFs = it.value().constBegin();
             itFs != it.value().constEnd(); ++itFs)
        {
            int sid = itFs.key();
            const auto &fs = itFs.value();
            qint64 flow = fs.ab_count;
            int cap = m_stationCapacity.value(sid, 1);
            double pres = cap>0 ? double(flow)/cap : 0.0;
            stationFlowHourly    [sid][Y][M][D][h] = flow;
            stationPressureHourly[sid][Y][M][D][h] = pres;
        }
    }
}

void data_preload::computeAggregates()
{
    // —— 线路运力：小时→日平均→月平均→年平均 ——
    for (auto itLine = lineCapacityHourly.constBegin();
         itLine != lineCapacityHourly.constEnd(); ++itLine)
    {
        int lineId = itLine.key();

        // 1) 日平均
        for (auto itY = itLine.value().constBegin(); itY != itLine.value().constEnd(); ++itY) {
            int year = itY.key();
            const MonthMapDbl &monthMap = itY.value();
            for (auto itM = monthMap.constBegin(); itM != monthMap.constEnd(); ++itM) {
                int month = itM.key();
                const DayMapDbl &dayMap = itM.value();
                for (auto itD = dayMap.constBegin(); itD != dayMap.constEnd(); ++itD) {
                    int day = itD.key();
                    const HourMapDbl &hourMap = itD.value();

                    double sum = 0;
                    int count = 0;
                    for (auto itH = hourMap.constBegin(); itH != hourMap.constEnd(); ++itH) {
                        sum   += itH.value();
                        ++count;
                    }
                    double avg = (count > 0 ? sum / count : 0.0);
                    QDate date(year, month, day);
                    lineCapacityDaily[lineId][date] = avg;
                }
            }
        }

        // 2) 月平均
        // 临时 map： key=(year,month) → (sum, count)
        QMap<QPair<int,int>, QPair<double,int>> tmpMon;
        for (auto it = lineCapacityDaily[lineId].constBegin();
             it != lineCapacityDaily[lineId].constEnd(); ++it)
        {
            QDate d = it.key();
            QPair<int,int> key = qMakePair(d.year(), d.month());
            tmpMon[key].first  += it.value();
            tmpMon[key].second += 1;
        }
        for (auto it = tmpMon.constBegin(); it != tmpMon.constEnd(); ++it) {
            QPair<int,int> ym = it.key();
            double sum  = it.value().first;
            int    cnt  = it.value().second;
            double avgM = (cnt > 0 ? sum / cnt : 0.0);
            lineCapacityMonthly[lineId][ym] = avgM;
        }

        // 3) 年平均
        QMap<int, QPair<double,int>> tmpYear;
        for (auto it = lineCapacityMonthly[lineId].constBegin();
             it != lineCapacityMonthly[lineId].constEnd(); ++it)
        {
            int year = it.key().first;
            tmpYear[year].first  += it.value();
            tmpYear[year].second += 1;
        }
        for (auto it = tmpYear.constBegin(); it != tmpYear.constEnd(); ++it) {
            int year = it.key();
            double sum = it.value().first;
            int    cnt = it.value().second;
            double avgY = (cnt > 0 ? sum / cnt : 0.0);
            lineCapacityYearly[lineId][year] = avgY;
        }
    }

    // —— 站点客流：小时→日求和→月求和→年求和 ——
    for (auto itSt = stationFlowHourly.constBegin();
         itSt != stationFlowHourly.constEnd(); ++itSt)
    {
        int sid = itSt.key();
        // 日、月、年临时累加容器
        QMap<QDate, qint64>                 tmpDay;
        QMap<QPair<int,int>, qint64>        tmpMon;
        QMap<int, qint64>                   tmpYear;

        for (auto itY = itSt.value().constBegin();
             itY != itSt.value().constEnd(); ++itY)
        {
            int year = itY.key();
            const MonthMapInt &monthMap = itY.value();
            for (auto itM = monthMap.constBegin(); itM != monthMap.constEnd(); ++itM) {
                int month = itM.key();
                const DayMapInt &dayMap = itM.value();
                for (auto itD = dayMap.constBegin(); itD != dayMap.constEnd(); ++itD) {
                    int day = itD.key();
                    const HourMapInt &hourMap = itD.value();

                    qint64 sum = 0;
                    for (auto itH = hourMap.constBegin(); itH != hourMap.constEnd(); ++itH) {
                        sum += itH.value();
                    }
                    QDate date(year, month, day);
                    stationFlowDaily[sid][date] = sum;
                    tmpDay[date]               = sum;
                    tmpMon[qMakePair(year,month)] += sum;
                    tmpYear[year]               += sum;
                }
            }
        }
        // 写入月/年
        for (auto it = tmpMon.constBegin(); it != tmpMon.constEnd(); ++it)
            stationFlowMonthly[sid][it.key()] = it.value();
        for (auto it = tmpYear.constBegin(); it != tmpYear.constEnd(); ++it)
            stationFlowYearly[sid][it.key()]  = it.value();
    }

    // —— 站点压力：小时→日平均→月平均→年平均 ——
    for (auto itSt = stationPressureHourly.constBegin();
         itSt != stationPressureHourly.constEnd(); ++itSt)
    {
        int sid = itSt.key();
        QMap<QDate, QPair<double,int>>      tmpDay;
        QMap<QPair<int,int>, QPair<double,int>> tmpMon;
        QMap<int, QPair<double,int>>        tmpYear;

        for (auto itY = itSt.value().constBegin();
             itY != itSt.value().constEnd(); ++itY)
        {
            int year = itY.key();
            const MonthMapDbl &monthMap = itY.value();
            for (auto itM = monthMap.constBegin(); itM != monthMap.constEnd(); ++itM) {
                int month = itM.key();
                const DayMapDbl &dayMap = itM.value();
                for (auto itD = dayMap.constBegin(); itD != dayMap.constEnd(); ++itD) {
                    int day = itD.key();
                    const HourMapDbl &hourMap = itD.value();

                    double sum = 0;
                    int    cnt = 0;
                    for (auto itH = hourMap.constBegin(); itH != hourMap.constEnd(); ++itH) {
                        sum += itH.value();
                        ++cnt;
                    }
                    QDate date(year, month, day);
                    tmpDay[date].first  += sum;
                    tmpDay[date].second += cnt;
                    tmpMon[qMakePair(year,month)].first  += sum;
                    tmpMon[qMakePair(year,month)].second += cnt;
                    tmpYear[year].first  += sum;
                    tmpYear[year].second += cnt;
                }
            }
        }
        // 写入日／月／年平均
        for (auto it = tmpDay.constBegin(); it != tmpDay.constEnd(); ++it) {
            stationPressureDaily[sid][it.key()] =
                (it.value().second>0 ? it.value().first/it.value().second : 0.0);
        }
        for (auto it = tmpMon.constBegin(); it != tmpMon.constEnd(); ++it) {
            stationPressureMonthly[sid][it.key()] =
                (it.value().second>0 ? it.value().first/it.value().second : 0.0);
        }
        for (auto it = tmpYear.constBegin(); it != tmpYear.constEnd(); ++it) {
            stationPressureYearly[sid][it.key()] =
                (it.value().second>0 ? it.value().first/it.value().second : 0.0);
        }
    }
}
void data_preload::debugPrintAllData() const
{
    // —— LineCapacityHourly ——
    qDebug() << "=== LineCapacityHourly ===";
    for (auto itLine = lineCapacityHourly.constBegin();
         itLine != lineCapacityHourly.constEnd(); ++itLine)
    {
        int lid = itLine.key();
        const YearMapDbl &yMap = itLine.value();
        for (auto itY = yMap.constBegin(); itY != yMap.constEnd(); ++itY) {
            int year = itY.key();
            const MonthMapDbl &mMap = itY.value();
            for (auto itM = mMap.constBegin(); itM != mMap.constEnd(); ++itM) {
                int month = itM.key();
                const DayMapDbl &dMap = itM.value();
                for (auto itD = dMap.constBegin(); itD != dMap.constEnd(); ++itD) {
                    int day = itD.key();
                    const HourMapDbl &hMap = itD.value();
                    for (auto itH = hMap.constBegin(); itH != hMap.constEnd(); ++itH) {
                        int hour = itH.key();
                        double match = itH.value();
                        qDebug()
                            << "Line" << lid
                            << QString("%1-%2-%3 %4:00")
                                   .arg(year,4,10,QChar('0'))
                                   .arg(month,2,10,QChar('0'))
                                   .arg(day,2,10,QChar('0'))
                                   .arg(hour,2,10,QChar('0'))
                            << "match=" << match;
                    }
                }
            }
        }
    }

    // —— LineCapacityDaily ——
    qDebug() << "=== LineCapacityDaily ===";
    for (auto itLine = lineCapacityDaily.constBegin();
         itLine != lineCapacityDaily.constEnd(); ++itLine)
    {
        int lid = itLine.key();
        const DailyMap<double> &map = itLine.value();
        for (auto itDate = map.constBegin(); itDate != map.constEnd(); ++itDate) {
            QDate d = itDate.key();
            double avg = itDate.value();
            qDebug()
                << "Line" << lid
                << d.toString("yyyy-MM-dd")
                << "dayAvg=" << avg;
        }
    }

    // —— LineCapacityMonthly ——
    qDebug() << "=== LineCapacityMonthly ===";
    for (auto itLine = lineCapacityMonthly.constBegin();
         itLine != lineCapacityMonthly.constEnd(); ++itLine)
    {
        int lid = itLine.key();
        const MonthlyMap<double> &map = itLine.value();
        for (auto itYM = map.constBegin(); itYM != map.constEnd(); ++itYM) {
            int year  = itYM.key().first;
            int month = itYM.key().second;
            double avg = itYM.value();
            qDebug()
                << "Line" << lid
                << QString("%1-%2")
                       .arg(year,4,10,QChar('0'))
                       .arg(month,2,10,QChar('0'))
                << "monAvg=" << avg;
        }
    }

    qDebug() << "=== LineCapacityYearly ===";
    for (auto itLine = lineCapacityYearly.constBegin();
         itLine != lineCapacityYearly.constEnd(); ++itLine)
    {
        int lid = itLine.key();
        const YearlyMap<double> &yrMap = itLine.value();
        for (auto itYr = yrMap.constBegin(); itYr != yrMap.constEnd(); ++itYr) {
            int year = itYr.key();
            double avg = itYr.value();
            qDebug()
                << "Line" << lid
                << QString::number(year)
                << "yrAvg=" << avg;
        }
    }
    // —— StationFlowHourly ——
    qDebug() << "=== StationFlowHourly ===";
    for (auto itSt = stationFlowHourly.constBegin();
         itSt != stationFlowHourly.constEnd(); ++itSt)
    {
        int sid = itSt.key();
        const YearMapInt &yMap = itSt.value();
        for (auto itY = yMap.constBegin(); itY != yMap.constEnd(); ++itY) {
            int year = itY.key();
            const MonthMapInt &mMap = itY.value();
            for (auto itM = mMap.constBegin(); itM != mMap.constEnd(); ++itM) {
                int month = itM.key();
                const DayMapInt &dMap = itM.value();
                for (auto itD = dMap.constBegin(); itD != dMap.constEnd(); ++itD) {
                    int day = itD.key();
                    const HourMapInt &hMap = itD.value();
                    for (auto itH = hMap.constBegin(); itH != hMap.constEnd(); ++itH) {
                        int hour = itH.key();
                        qint64 flow = itH.value();
                        qDebug()
                            << "Sta" << sid
                            << QString("%1-%2-%3 %4:00")
                                   .arg(year,4,10,QChar('0'))
                                   .arg(month,2,10,QChar('0'))
                                   .arg(day,2,10,QChar('0'))
                                   .arg(hour,2,10,QChar('0'))
                            << "flow=" << flow;
                    }
                }
            }
        }
    }

    // —— StationPressureHourly ——
    qDebug() << "=== StationPressureHourly ===";
    for (auto itSt = stationPressureHourly.constBegin();
         itSt != stationPressureHourly.constEnd(); ++itSt)
    {
        int sid = itSt.key();
        const YearMapDbl &yMap = itSt.value();
        for (auto itY = yMap.constBegin(); itY != yMap.constEnd(); ++itY) {
            int year = itY.key();
            const MonthMapDbl &mMap = itY.value();
            for (auto itM = mMap.constBegin(); itM != mMap.constEnd(); ++itM) {
                int month = itM.key();
                const DayMapDbl &dMap = itM.value();
                for (auto itD = dMap.constBegin(); itD != dMap.constEnd(); ++itD) {
                    int day = itD.key();
                    const HourMapDbl &hMap = itD.value();
                    for (auto itH = hMap.constBegin(); itH != hMap.constEnd(); ++itH) {
                        int hour = itH.key();
                        double pres = itH.value();
                        qDebug()
                            << "Sta" << sid
                            << QString("%1-%2-%3 %4:00")
                                   .arg(year,4,10,QChar('0'))
                                   .arg(month,2,10,QChar('0'))
                                   .arg(day,2,10,QChar('0'))
                                   .arg(hour,2,10,QChar('0'))
                            << "pres=" << pres;
                    }
                }
            }
        }
    }

    // —— StationFlow Daily/Monthly/Yearly ——
    qDebug() << "=== StationFlow Daily/Monthly/Yearly ===";
    // Daily
    for (auto itSt = stationFlowDaily.constBegin();
         itSt != stationFlowDaily.constEnd(); ++itSt)
    {
        int sid = itSt.key();
        const DailyMap<qint64> &map = itSt.value();
        for (auto itD = map.constBegin(); itD != map.constEnd(); ++itD) {
            qDebug()
            << "Sta" << sid
            << itD.key().toString("yyyy-MM-dd")
            << "flowDay=" << itD.value();
        }
    }
    // Monthly
    for (auto itSt = stationFlowMonthly.constBegin();
         itSt != stationFlowMonthly.constEnd(); ++itSt)
    {
        int sid = itSt.key();
        const MonthlyMap<qint64> &map = itSt.value();
        for (auto itM = map.constBegin(); itM != map.constEnd(); ++itM) {
            int y = itM.key().first, m = itM.key().second;
            qDebug()
                << "Sta" << sid
                << QString("%1-%2").arg(y,4,10,QChar('0')).arg(m,2,10,QChar('0'))
                << "flowMon=" << itM.value();
        }
    }
    // Yearly
    for (auto itSt = stationFlowYearly.constBegin();
         itSt != stationFlowYearly.constEnd(); ++itSt)
    {
        int sid = itSt.key();
        for (auto itY = itSt.value().constBegin(); itY != itSt.value().constEnd(); ++itY) {
            qDebug()
            << "Sta" << sid
            << itY.key()
            << "flowYr=" << itY.value();
        }
    }

    // —— StationPressure Daily/Monthly/Yearly ——
    qDebug() << "=== StationPressure Daily/Monthly/Yearly ===";
    // Daily
    for (auto itSt = stationPressureDaily.constBegin();
         itSt != stationPressureDaily.constEnd(); ++itSt)
    {
        int sid = itSt.key();
        const DailyMap<double> &map = itSt.value();
        for (auto itD = map.constBegin(); itD != map.constEnd(); ++itD) {
            qDebug()
            << "Sta" << sid
            << itD.key().toString("yyyy-MM-dd")
            << "presDay=" << itD.value();
        }
    }
    // Monthly
    for (auto itSt = stationPressureMonthly.constBegin();
         itSt != stationPressureMonthly.constEnd(); ++itSt)
    {
        int sid = itSt.key();
        const MonthlyMap<double> &map = itSt.value();
        for (auto itM = map.constBegin(); itM != map.constEnd(); ++itM) {
            int y = itM.key().first, m = itM.key().second;
            qDebug()
                << "Sta" << sid
                << QString("%1-%2").arg(y,4,10,QChar('0')).arg(m,2,10,QChar('0'))
                << "presMon=" << itM.value();
        }
    }
    // Yearly
    for (auto itSt = stationPressureYearly.constBegin();
         itSt != stationPressureYearly.constEnd(); ++itSt)
    {
        int sid = itSt.key();
        for (auto itY = itSt.value().constBegin(); itY != itSt.value().constEnd(); ++itY) {
            qDebug()
            << "Sta" << sid
            << itY.key()
            << "presYr=" << itY.value();
        }
    }
}
