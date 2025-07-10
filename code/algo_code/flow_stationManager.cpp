#include "flow_stationManager.h"


void flow_stationManager::runFlowManager(const QVector<order> &orders){

    qDebug()<<"开始构建flow_stationManager";
    // 清空之前的数据
    flow_stations.clear();

    // 遍历所有订单，按照时间戳按小时和站点分组
    for (const auto &currentOrder : orders) {
        QCoreApplication::processEvents();
        // 获取订单时间戳的小时部分
        QDateTime orderDateTime = currentOrder.timestamp;
        QDateTime hourDateTime = QDateTime(orderDateTime.date(), QTime(orderDateTime.time().hour(), 0));

        // 使用站点ID作为二级索引，确保每个站点每小时的数据独立
        int stationId = currentOrder.station_id;

        // 如果该小时和站点的组合不存在，则初始化
        if (!flow_stations.contains(hourDateTime)) {
            flow_stations.insert(hourDateTime, QMap<int, flow_station>());
        }

        // 获取该小时的站点数据
        QMap<int, flow_station> &stationsData = flow_stations[hourDateTime];

        // 如果该站点的数据不存在，则初始化
        if (!stationsData.contains(stationId)) {
            flow_station newFlowStation;
            newFlowStation.station_id = stationId;
            stationsData.insert(stationId, newFlowStation);
        }

        // 获取该站点的客流数据
        flow_station &flow = stationsData[stationId];

        // 累加该订单的上客量和下客量
        flow.boarding_count += currentOrder.boarding_count;
        flow.alighting_count += currentOrder.alighting_count;

        // 累加上下车的总人数
        flow.ab_count += currentOrder.boarding_count + currentOrder.alighting_count;
    }

    qDebug()<<"flow_stationManager构建完成";

}

void flow_stationManager::qdebugShow() const
{
    // 打印每小时每个站点的客流量数据
    for (auto it = flow_stations.begin(); it != flow_stations.end(); ++it) {
        QDateTime hour = it.key();
        const QMap<int, flow_station> &stationsData = it.value();

        for (auto stationIt = stationsData.begin(); stationIt != stationsData.end(); ++stationIt) {
            int stationId = stationIt.key();
            const flow_station &flow = stationIt.value();

            qDebug() << "时间: " << hour.toString("yyyy-MM-dd HH:mm:ss")
                     << "站点ID: " << stationId
                     << "上车人数: " << flow.boarding_count
                     << "下车人数: " << flow.alighting_count
                     << "总乘客数: " << flow.ab_count;
        }
    }
}
