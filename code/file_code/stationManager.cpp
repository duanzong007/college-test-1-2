#include "stationManager.h"
#include <QDebug>
#include <QtGlobal>

stationManager::stationManager()
{
    stations.reserve(4500);
}
bool stationManager::convertStationData(const QVector<fstation>& rawFile)
{
    qDebug() << "清洗站点数据";
    for (const auto &r : rawFile)
    {
        /* ① ID 合法性 */
        if (r.zdid <= 0) {
            qWarning() << "非法站点ID，跳过:" << r.zdid;
            continue;
        }

        /* ② 名称不能为空 */
        const QString name = r.zdmc.trimmed();
        if (name.isEmpty()) {
            qWarning() << "站点" << r.zdid << "缺失名称，跳过";
            continue;
        }

        /* ③ 运营状态 */
        bool inService = (r.sfty.trimmed() != "1");

        /* ④ 其余字符串做基本 trim，缺失保持为空 */
        const QString code      = r.station_code.trimmed();
        const QString telecode  = r.station_telecode.trimmed();
        const QString shortname = r.station_shortname.trimmed();

        /* ⑤ 推入结果容器 */
        stations.push_back(
            station{ r.zdid, name, inService,
                    code, telecode, shortname });
    }

    qDebug() << "站点数据清洗完成";
    return !stations.isEmpty();
}

void stationManager::qdebugShow() const
{
    for (const auto &s : stations) {
        const char* status = s.is_in_service ? "正常运营" : "停运";
        qDebug().nospace().noquote()
            << s.station_id << "号站点：" << s.station_name
            << "，" << status
            << "，代码：" << s.station_code
            << "，电报码：" << s.station_telecode
            << "，简称：" << s.station_shortname;
    }
}
