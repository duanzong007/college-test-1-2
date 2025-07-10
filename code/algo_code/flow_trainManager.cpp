#include "flow_trainManager.h"



int findStation(QString code){
    if(code=="CDW")return 1640;
    else if(code=="CUW")return 1037;
    else if(code=="ICW")return 1695;
    else return -1;
}


void flow_trainManager::runFlowManager(const QMap<int, QVector<line>> &lines,
                                       const QVector<order> &orders,
                                       const QVector<station> &stations,
                                       const QVector<train> &trainss){

    qDebug()<<"开始构建flow_trainManager";
    // 清空之前的数据
    flow_trains.clear();

    QMap<QDate, QMap<int, QVector<order>>> groupedOrders;
    QMap<int,QMap<int,int>> s_seq_in_line;//最外层是线路id，第二层是站点id，第三层是seq
    // 遍历订单并分组，同时按时间戳对每个列车的订单进行排序
    for (const order& ord : orders) {
        // 将订单添加到对应的日期和列车ID组中
        groupedOrders[ord.timestamp.date()][ord.train_id].append(ord);
    }

    // 对每个日期和列车内的订单进行排序
    for (auto& dateIt : groupedOrders) {
        for (auto& trainIt : dateIt) {
            // 对每个列车的订单按时间戳进行排序
            std::sort(trainIt.begin(), trainIt.end(), [](const order& o1, const order& o2) {
                return o1.timestamp < o2.timestamp;
            });
        }
    }



    for (auto it = groupedOrders.begin(); it != groupedOrders.end(); ++it) {
        QDate date = it.key();

        QMap<int, QVector<order>>& trains = it.value();
        // 对每个列车进行处理
        for (auto trainIt = trains.begin(); trainIt != trains.end(); ++trainIt) {
            int trainId = trainIt.key();
            QVector<order>& trainOrders = trainIt.value();
            flow_train put;
            int lineId = trainOrders[0].line_id;
            put.train_id = trainId;
            put.line_id = lineId;
            bool trfind=false;
            for(auto &tr : trainss){
                if(tr.train_id==trainId){
                    if(tr.train_capacity!=0) put.capacity=tr.train_capacity;
                    else put.capacity=1500;
                    trfind=true;
                }
            }
            if(!trfind) put.capacity=1500;

            // 查找起始站点和终点站点
            QString startStationTelecode = trainOrders[0].start_station_telecode;
            QString endStationTelecode = trainOrders[0].end_station_telecode;

            // 查找站点信息
            int startStation_id = 0, endStation_id = 0;
            if(findStation(startStationTelecode)!=-1&&
                findStation(endStationTelecode)!=-1)
            {
                startStation_id=findStation(startStationTelecode);
                endStation_id=findStation(endStationTelecode);
            }
            else{
                for (const auto &s : stations) {
                    if (s.station_telecode == startStationTelecode) {
                        startStation_id = s.station_id;
                    }
                    if (s.station_telecode == endStationTelecode) {
                        endStation_id = s.station_id;
                    }
                }
            }



            // 设置起始站和终点站
            put.start_station_id = startStation_id;
            put.end_station_id = endStation_id;

            // 计算列车的行进方向（根据线路信息）
            const QVector<line> &lineStations = lines[lineId];
            bool directionDetermined = false;
            int start = 0, end = 0;


            if(s_seq_in_line[lineId].contains(startStation_id)&&
                s_seq_in_line[lineId].contains(endStation_id)){
                start = s_seq_in_line[lineId][startStation_id];
                end = s_seq_in_line[lineId][endStation_id];
            }
            else{
                for (const auto &l : lineStations) {
                    if (l.station_id == startStation_id) {
                        start = l.seq_in_line;
                        s_seq_in_line[lineId][startStation_id]=start;
                    }
                    if (l.station_id == endStation_id) {
                        end = l.seq_in_line;
                        s_seq_in_line[lineId][endStation_id]=end;
                    }
                }
            }


            if (end > start) {
                put.direction= Forward;
                directionDetermined = true;
            }
            else{
                put.direction= Backward;
                directionDetermined = true;
            }
            if (!directionDetermined) {
                put.direction = Forward;
            }
            int lastCount = 0;  // 用于记录上一站的累计人数
            // 访问排序后的订单数据
            for (const order& ord : trainOrders) {
                bool found = false;

                // 手动查找元素
                for (auto &f:put.stations) {
                    if (f.station_id == ord.station_id) {
                        found = true;
                        f.people = lastCount - ord.alighting_count + ord.boarding_count;
                        break;
                    }
                }
                if (!found) {
                    flow_train_station ne;
                    ne.station_id=ord.station_id;
                    ne.time=ord.timestamp.time();

                    if(s_seq_in_line[lineId].contains(ne.station_id)){
                        ne.seq_in_line=s_seq_in_line[lineId][ne.station_id];
                    }
                    else{
                        for (const auto &l : lineStations) {
                            if (l.station_id == ne.station_id) {
                                ne.seq_in_line = l.seq_in_line;
                                s_seq_in_line[lineId][ne.station_id]=ne.seq_in_line;
                            }
                        }
                    }


                    ne.people = lastCount - ord.alighting_count + ord.boarding_count;
                    put.stations.append(ne);

                }
                lastCount = put.stations.last().people;
            }


            flow_trains[date][trainId] = put;

        }


    }


    for(auto &a:flow_trains){
        for(auto &b: a){
            for(auto &c: b.stations){
                if(c.people<0){
                    c.people = 0;
                }
            }
        }
    }

    qDebug()<<"flow_trainManager构建完成";
}







void flow_trainManager::qdebugShow() const
{
    // 遍历每一天的流量数据
    for (auto dateIt = flow_trains.cbegin(); dateIt != flow_trains.cend(); ++dateIt) {
        const QDate &date = dateIt.key();
        const QMap<int, flow_train> &trains = dateIt.value();

        qDebug() << "日期: " << date.toString("yyyy-MM-dd");

        // 遍历同一天的每列车
        for (auto trainIt = trains.cbegin(); trainIt != trains.cend(); ++trainIt) {
            const flow_train &train = trainIt.value();
            qDebug().noquote() << "  列车ID:" << train.train_id
                               << " 线路ID:" << train.line_id
                               << " 容量:"   << train.capacity
                               << " 起始站ID:" << train.start_station_id
                               << " 终点站ID:" << train.end_station_id
                               << " 行进方向:" << (train.direction == Forward ? "正向" : "反向");

            // 遍历列车的各个站点信息
            for (const flow_train_station &station : train.stations) {
                qDebug().nospace().noquote() << "    站点ID:" << station.station_id
                                             << " 序号:" << station.seq_in_line
                                             << " 时间:" << station.time.toString("hh:mm")
                                             << " 客流量:" << station.people;
            }
        }
    }

    // QSet<int> seenLineIds;  // 用于存储已经输出过的线路ID

    // // 遍历 flow_trains 中的每个日期和每个列车
    // for (auto dateIt = flow_trains.cbegin(); dateIt != flow_trains.cend(); ++dateIt) {
    //     const QMap<int, flow_train> &trains = dateIt.value();

    //     // 遍历每个列车
    //     for (auto trainIt = trains.cbegin(); trainIt != trains.cend(); ++trainIt) {
    //         const flow_train &train = trainIt.value();

    //         // 如果该线路ID没有被输出过
    //         if (!seenLineIds.contains(train.line_id)) {
    //             seenLineIds.insert(train.line_id);

    //             // 输出该线路的信息
    //             qDebug() << "线路ID:" << train.line_id;
    //         }
    //     }
    // }
}
