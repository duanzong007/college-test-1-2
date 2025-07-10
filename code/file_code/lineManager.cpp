#include "lineManager.h"
#include <QDebug>
#include <QHash>
#include <QMap>
#include <QtAlgorithms>   // std::sort


lineManager::lineManager(){

}

void lineManager::qdebugShow() const
{
    for (auto it=lines.cbegin(); it!=lines.cend(); ++it) {
        qDebug().noquote() << "线路" << it.key();
        for (const line &l: it.value())
            qDebug().nospace().noquote() << "#" << l.seq_in_line
                                         << " 站:" << l.station_id
                                         << " prev:" << l.prev_station_id
                                         << " next:" << l.next_station_id
                                         << " dist:" << l.interstation_distance
                                         << " 停:" << l.is_stop;
    }
}


/* —— 把 QString 安全转 int 的小工具 —— */
static int toInt(const QString &s)
{
    bool ok = false;
    int v = s.trimmed().toInt(&ok);
    return ok ? v : 0;
}

/* ---------------------------------------------------
   lineManager::convertLineData  ◆核心流程◆
   1) 丢掉关键字段缺失的行
   2) 以 (lineId, seq) 为键去重   →  TODO: 自定义规则
   3) seq 升序排好后，校正 prev/next 并打日志
 ---------------------------------------------------*/
bool lineManager::convertLineData(const QVector<fline> &rawFile)
{
    qDebug()<<"清洗线路数据";
    lines.clear();
    if (rawFile.isEmpty()) return false;

    /* 临时桶：lineId → (seq → line) */
    QMap<int, QMap<int, line>> tmp;

    /* === ① 过滤 + 基础拷贝 === */
    for (const auto &r : rawFile)
    {
        int lid  = r.yyxlbm;   // 运营线路编码
        int stid = r.zdid;     // 站点 ID
        int seq  = toInt(r.xlzdid);   // 线路站点序号
        if (!lid || !stid || !seq) {
            qDebug().noquote() << "[丢弃] lid" << r.yyxlbm
                               << " st" << r.zdid << " seq" << r.xlzdid
                               << " ← 关键字段缺失";
            continue;
        }

        line rec;
        rec.seq_in_line           = seq;
        rec.station_id            = stid;
        rec.prev_station_id       = toInt(r.Q_zdid);
        rec.next_station_id       = toInt(r.H_zdid);
        rec.interstation_distance = toInt(r.yqzdjjl);
        rec.line_code             = r.xldm.trimmed();
        rec.is_stop               = (r.sfytk.trimmed() == "1");

        /* === ② 去重：同 lineId+seq 只留一条 === */
        if (tmp[lid].contains(seq)) {
            // TODO 这里换成你想要的保留/合并规则
            qDebug().noquote() << "[去重] line" << lid
                               << " seq" << seq
                               << " → 已存在，忽略新行";
            continue;
        }
        tmp[lid][seq] = rec;
    }

    /* === ③ 排序 + 校正 prev/next === */
    for (auto it = tmp.cbegin(); it != tmp.cend(); ++it)
    {
        int lid = it.key();

        QVector<line> v = it.value().values().toVector();   // 已按 key(=seq) 升序
        // 理论 prev/next 并校正
        for (int i = 0; i < v.size(); ++i) {
            int expectPrev = (i == 0) ? -1 : v[i-1].station_id;
            int expectNext = (i == v.size()-1) ? -1 : v[i+1].station_id;

            if (v[i].prev_station_id && v[i].prev_station_id != expectPrev) {
                qDebug().noquote() << "[修正-prev] line" << lid
                                   << " seq" << v[i].seq_in_line
                                   << v[i].prev_station_id << "→" << expectPrev;
                v[i].prev_station_id = expectPrev;
            }
            if (v[i].next_station_id && v[i].next_station_id != expectNext) {
                qDebug().noquote() << "[修正-next] line" << lid
                                   << " seq" << v[i].seq_in_line
                                   << v[i].next_station_id << "→" << expectNext;
                v[i].next_station_id = expectNext;
            }
        }
        lines.insert(lid, std::move(v));
    }
    qDebug()<<"线路数据清洗完成";
    return !lines.isEmpty();
}

