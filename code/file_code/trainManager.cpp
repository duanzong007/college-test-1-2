#include "trainManager.h"
#include <QDebug>
trainManager::trainManager(){
    trains.reserve(800);
}

bool trainManager::convertTrainData(const QVector<ftrain>& rawFile)
{
    qDebug()<<"清洗列车数据";
    for (const auto& r : rawFile)
    {
        /* ① ID 合法性 */
        if (r.lcbm <= 0) {
            qWarning() << "非法列车ID，跳过:" << r.lcbm;
            continue;
        }

        /* ② 车次不能为空 */
        const QString num = r.lcdm.trimmed();
        if (num.isEmpty()) {
            qWarning() <<"列车"<<r.lcbm<< "缺失车次，跳过:" << r.lcdm;
            continue;
        }

        /* ③ 运行状态 → bool */
        const bool inService = (r.sfzt.trimmed() == "1");

        /* ④ 运量 → int；异常置 0 */
        bool okCap = false;
        int cap    = r.lcyn.trimmed().toInt(&okCap);
        if (!okCap || cap < 0) {
            //qWarning() <<"列车"<<r.lcbm<< "运量字段异常，已置 0:" << r.lcyn;
            cap = 0;
        }

        /* ⑤ 组装入 QVector */
        train put(r.lcbm, num, inService, cap);
        trains.push_back(put);
    }
    qDebug()<<"列车数据清洗完成";
    return !trains.isEmpty();
}

void trainManager::qdebugShow() const
{
    for (const auto &t : trains) {
        const char* status = t.is_in_service ? "在运行" : "停运";
        qDebug().nospace().noquote()
            << t.train_id << "号车车次号：" << t.train_number
            << "，" << status
            << "，载客量："  << t.train_capacity;
    }
}
