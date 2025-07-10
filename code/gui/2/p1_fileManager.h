#ifndef P1_FILEMANAGER_H
#define P1_FILEMANAGER_H

#include <QObject>
#include <QDateTime>
#include <QMap>
#include <QString>
#include <QVariantMap>
#include <QList>
#include "algo_code/ip_check.h"
#include "algo_code/flow_stationManager.h"

class flow_stationManager;
class ip_check;  // 前向声明

class p1_fileManager : public QObject {
    Q_OBJECT

public:
    /**
     * @param flowMgr   指向已有的 flow_stationManager
     * @param ipMgr     指向已填充好 ips 映射的 ip_check
     */
    explicit p1_fileManager(flow_stationManager* flowMgr,
                            ip_check*         ipMgr,
                            QObject*          parent = nullptr);

public slots:
    /** 接收时间变化，计算并发送给地图层 */
    void onTimeChanged(const QDateTime& dt);

signals:
    /** 发给地图层的站点数据列表 */
    void sendFieldData(const QList<QVariantMap>& data);

private:
    flow_stationManager* m_flowMgr;      // 客流数据源
    ip_check*            m_ipCheck;      // 精度/伪度数据源

    // 下面两张 Map 在构造函数里手动打表
    QMap<int, QString>   m_nameMap;      // 站点ID → 站点名称
    QMap<int, int>       m_capacityMap;  // 站点ID → 承载能力
};

#endif // P1_FILEMANAGER_H
