#include "p1_fileManager.h"

p1_fileManager::p1_fileManager(flow_stationManager* flowMgr,
                               ip_check*           ipMgr,
                               QObject*            parent)
    : QObject(parent)
    , m_flowMgr(flowMgr)
    , m_ipCheck(ipMgr)
{
    m_nameMap.insert(810, "遂宁站");
    m_nameMap.insert(1037, "重庆北站");
    m_nameMap.insert(1640, "成都站");
    m_nameMap.insert(1689, "合川站");
    m_nameMap.insert(1690, "潼南站");
    m_nameMap.insert(1694, "大英东站");
    m_nameMap.insert(1695, "成都东站");

    m_capacityMap.insert(810, 700);
    m_capacityMap.insert(1037, 5000);
    m_capacityMap.insert(1640, 3000);
    m_capacityMap.insert(1689, 500);
    m_capacityMap.insert(1690, 300);
    m_capacityMap.insert(1694, 200);
    m_capacityMap.insert(1695, 5000);

}
void p1_fileManager::onTimeChanged(const QDateTime& dt) {
    QList<QVariantMap> outList;

    // 1) 取出该小时的所有站点客流记录
    auto recs = m_flowMgr->flow_stations.value(dt);
    for (auto it = recs.constBegin(); it != recs.constEnd(); ++it) {
        const flow_station &fs = it.value();
        int   id         = fs.station_id;
        int   boarding   = fs.boarding_count;
        int   alighting  = fs.alighting_count;
        int   flow       = fs.ab_count;
        int   capacity   = m_capacityMap.value(id, 1);              // 防止除0
        double pressure  = capacity > 0 ? double(flow) / capacity : 0.0;

        // 2) 从 ip_check 拿经纬度
        ip ipInfo = m_ipCheck->ips.value(id, ip());
        double jing = ipInfo.jing;
        double wei  = ipInfo.wei;


        // // 输出调试信息
        // qDebug() << QString("站点ID:%1 名称:%2 上车:%3 下车:%4 总流:%5 承载:%6 压力:%7 经纬:[%8, %9]")
        //                 .arg(id)
        //                 .arg(m_nameMap.value(id, "未知站点"))
        //                 .arg(boarding)
        //                 .arg(alighting)
        //                 .arg(flow)
        //                 .arg(capacity)
        //                 .arg(pressure, 0, 'f', 3)
        //                 .arg(jing, 0, 'f', 6)
        //                 .arg(wei, 0, 'f', 6);




        // 3) 组装 QVariantMap
        QVariantMap map;
        map["id"]        = id;
        map["name"]      = m_nameMap.value(id, QStringLiteral("未知站点"));
        map["boarding"]  = boarding;
        map["alighting"] = alighting;
        map["flow"]      = flow;
        map["pressure"]  = pressure;
        map["jing"]      = jing;
        map["wei"]       = wei;

        outList.append(map);
    }

    // 4) 发信号给地图层
    emit sendFieldData(outList);
}
