#include "p0_fileManager.h"
static const QVector<QColor>& kPalette = []() -> const QVector<QColor>& {
    static const QVector<QColor> palette = {
        QColor::fromRgb(0x0029ff),QColor::fromRgb(0x6f00ff),QColor::fromRgb(0xff00f5),QColor::fromRgb(0xff005c),QColor::fromRgb(0xff3c00),
        QColor::fromRgb(0xffbf00),QColor::fromRgb(0x8f00ff),QColor::fromRgb(0xff00ea),QColor::fromRgb(0xff0043),QColor::fromRgb(0xff2900),
        QColor::fromRgb(0xffd600),QColor::fromRgb(0x8dff00),QColor::fromRgb(0x00ff11),QColor::fromRgb(0x00ffc6),QColor::fromRgb(0xb400ff),
        QColor::fromRgb(0xff0092),QColor::fromRgb(0xff0001),QColor::fromRgb(0xff0300),QColor::fromRgb(0x00c8ff),QColor::fromRgb(0x0018ff),
        QColor::fromRgb(0x9700ff),QColor::fromRgb(0xff00d2),QColor::fromRgb(0xff0031),QColor::fromRgb(0xff2100),QColor::fromRgb(0xffe100),
        QColor::fromRgb(0x93ff00),QColor::fromRgb(0xff000c),QColor::fromRgb(0xff1000),QColor::fromRgb(0xff0800),QColor::fromRgb(0xfff800),
        QColor::fromRgb(0xffd500),QColor::fromRgb(0x8fff00),QColor::fromRgb(0x00ff09),QColor::fromRgb(0x00ffa2),QColor::fromRgb(0x00c2ff),
        QColor::fromRgb(0xff2600),QColor::fromRgb(0xa2ff00),QColor::fromRgb(0x00ff52),QColor::fromRgb(0xb000ff),QColor::fromRgb(0xff009f),
        QColor::fromRgb(0xff0005),QColor::fromRgb(0xfffb00),QColor::fromRgb(0xa5ff00),QColor::fromRgb(0x00ff5e),QColor::fromRgb(0x00fff8),
        QColor::fromRgb(0x00dbff),QColor::fromRgb(0x00c5ff),QColor::fromRgb(0x001eff),QColor::fromRgb(0x9300ff),QColor::fromRgb(0xff00df),
        QColor::fromRgb(0xff0038),QColor::fromRgb(0xff2500),QColor::fromRgb(0xffde00),QColor::fromRgb(0x90ff00),QColor::fromRgb(0x00ff1b),
        QColor::fromRgb(0x00ffd0),QColor::fromRgb(0x0000f5),QColor::fromRgb(0x00fff3),QColor::fromRgb(0x00d7ff),QColor::fromRgb(0xa5ff00),
        QColor::fromRgb(0x0cff00),QColor::fromRgb(0x00ff74),QColor::fromRgb(0x00baff),QColor::fromRgb(0x0023ff),QColor::fromRgb(0x0000ff),
        QColor::fromRgb(0x00fffa),QColor::fromRgb(0x00dfff),QColor::fromRgb(0x0000eb),QColor::fromRgb(0xb700ff),QColor::fromRgb(0xff0085),
        QColor::fromRgb(0xff0016),QColor::fromRgb(0xfff600),QColor::fromRgb(0xa7ff00),QColor::fromRgb(0x00ff26),QColor::fromRgb(0x00ffd9),
        QColor::fromRgb(0xfff500),QColor::fromRgb(0x9fff00),QColor::fromRgb(0x00ff47),QColor::fromRgb(0x00cbff),QColor::fromRgb(0x0011ff),
        QColor::fromRgb(0x9e00ff),QColor::fromRgb(0xff00c4),QColor::fromRgb(0xff0024),QColor::fromRgb(0xff1c00),QColor::fromRgb(0xffea00),
        QColor::fromRgb(0x97ff00),QColor::fromRgb(0xfff900),QColor::fromRgb(0xaaff00),QColor::fromRgb(0x00ff74),QColor::fromRgb(0x00ffec),
        QColor::fromRgb(0x00e7ff),QColor::fromRgb(0x0000d7),QColor::fromRgb(0xbd00ff),QColor::fromRgb(0xff006a),QColor::fromRgb(0xff003b),
        QColor::fromRgb(0xff1800),QColor::fromRgb(0xfff100),QColor::fromRgb(0x9cff00),QColor::fromRgb(0x00ff3c),QColor::fromRgb(0x00ffe9),
        QColor::fromRgb(0x00d1ff),QColor::fromRgb(0x0006ff),QColor::fromRgb(0xfffb00),QColor::fromRgb(0x00ff31),QColor::fromRgb(0x00ffdd),
        QColor::fromRgb(0x00ceff),QColor::fromRgb(0x000cff),QColor::fromRgb(0xa600ff),QColor::fromRgb(0xff00b7),QColor::fromRgb(0xff0018),
        QColor::fromRgb(0xab00ff),QColor::fromRgb(0xff00ab),QColor::fromRgb(0x00ff69),QColor::fromRgb(0x00fffd),QColor::fromRgb(0x00e3ff),
        QColor::fromRgb(0x0000e1),QColor::fromRgb(0xba00ff),QColor::fromRgb(0xff0078),QColor::fromRgb(0xff0029),QColor::fromRgb(0xacff00),
        QColor::fromRgb(0x00ff80),QColor::fromRgb(0x00ffeb),QColor::fromRgb(0x00ebff),QColor::fromRgb(0x0000cd),QColor::fromRgb(0xc000ff),
    };
    return palette;
}();



p0_fileManager::p0_fileManager(QObject* parent)
    : QObject(parent)
{}

void p0_fileManager::preloadLinesData() {


    for (auto& a : system->stationManager_.stations) {
        station_id_to_name[a.station_id] = a.station_name;
    }

    for (auto& b : system->trainManager_.trains) {
        train_id_to_code    [b.train_id] = b.train_number;
    }


    // 使用迭代器遍历 QMap 中的元素
    for (auto it = system->lineManager_.lines.begin(); it != system->lineManager_.lines.end(); ++it) {

        Line lineData;
        lineData.id = it.key();
        lineData.capacityMatch = -1.0;  // 默认运力匹配度为 -1

        int idx = lineData.id % kPalette.size();
        lineData.color = kPalette.value(idx, kPalette.first());

        if(lineData.id == 1  || lineData.id == 3  || lineData.id == 4  || lineData.id == 6  || lineData.id == 7  || lineData.id == 8  ||
           lineData.id == 11 || lineData.id == 13 || lineData.id == 15 || lineData.id == 26 || lineData.id == 29 || lineData.id == 30 ||
           lineData.id == 36 || lineData.id == 39 || lineData.id == 47 || lineData.id == 49 || lineData.id == 62 || lineData.id == 79 ||
           lineData.id == 89 || lineData.id == 107 || lineData.id == 108
            ) lineData.visible = true;
        else lineData.visible = false;  // 默认可见性为 false

        const QVector<line>& lineStations = it.value();  // 用 lineStations 确保一致

        // 找到 seq_in_line 最大和最小值的元素
        auto minElem = std::min_element(lineStations.begin(), lineStations.end(),
                                        [](const line& a, const line& b) { return a.seq_in_line < b.seq_in_line; });
        auto maxElem = std::max_element(lineStations.begin(), lineStations.end(),
                                        [](const line& a, const line& b) { return a.seq_in_line < b.seq_in_line; });

        if (minElem != lineStations.end()) {
            lineData.startStation = station_id_to_name[minElem->station_id];
        }

        if (maxElem != lineStations.end()) {
            lineData.endStation = station_id_to_name[maxElem->station_id];
        }

        QVector<LineStation> stations;

        // 遍历线路的每个站点
        for (const auto& lineStation : lineStations) {
            int stationId = lineStation.station_id;  // 获取站点ID
            int seqInLine = lineStation.seq_in_line;  // 获取站点在线路上的顺序

            // 获取站点的经纬度（从 ip_check 中获取）
            if (system->ipManager_.ips.contains(stationId)) {
                const ip& stationIp = system->ipManager_.ips[stationId];  // 获取站点的 IP 信息
                double jing = stationIp.jing;  // 经度
                double wei = stationIp.wei;  // 纬度

                // 创建 LineStation 对象并添加到 stations 中
                stations.append(LineStation(stationId, seqInLine, jing, wei));
            } else {
                // 如果找不到站点的经纬度信息，输出警告
                qWarning() << "Warning: Station " << stationId << " not found in IP data.";
            }
        }

        // 按照 seq_in_line 排序站点，确保顺序正确
        std::sort(stations.begin(), stations.end(), [](const LineStation& a, const LineStation& b) {
            return a.seq_in_line < b.seq_in_line;  // 根据 seq_in_line 排序
        });

        // 将排序后的站点信息赋给线路数据
        lineData.stations = stations;

        // 将当前线路的完整数据添加到 m_dynamicLines 中
        m_dynamicLines.insert(lineData.id, lineData);
    }
}

bool p0_fileManager::isTrainRunningNow(const flow_train& train,
                                       const QDateTime&  selectedTime)
{
    if (train.stations.isEmpty())
        return false;                      // 没有站点数据，直接判定未运行

    // —— 1. 找到“实际出现过的”第一站 & 最后一站 ——
    //     根据方向选 seq_in_line 最小/最大
    const flow_train_station* first  = nullptr;
    const flow_train_station* last   = nullptr;

    for (const auto& st : train.stations) {
        if (!first || st.seq_in_line < first->seq_in_line)
            first = &st;
        if (!last  || st.seq_in_line > last->seq_in_line)
            last  = &st;
    }

    if (!first || !last)
        return false;                      // 理论不会发生，做二次保护

    // 对方向做调整：Forward → first=minSeq，last=maxSeq
    //              Backward→ first=maxSeq，last=minSeq
    if (train.direction == Backward)
        std::swap(first, last);

    // —— 2. 拿到“列车开始时间 / 结束时间” ——
    QTime trainStartTime = first->time;
    QTime trainEndTime   = last ->time;

    // —— 3. 与当前时刻比较 ——
    const QTime cur      = selectedTime.time();
    return (trainStartTime <= cur && cur <= trainEndTime);
}

bool  p0_fileManager::updateTrainRuntimeData(const flow_train& fTrain,
                                             const QDateTime&  selTime,
                                             core_system*      system,
                                             Train&            trainData)
{
    if (fTrain.stations.isEmpty() || !system) return false;

    // —— 1. 复制并按方向排序站点 ——
    QVector<flow_train_station> sts = fTrain.stations;
    std::sort(sts.begin(), sts.end(), [](const flow_train_station& a,
                                         const flow_train_station& b)
              { return a.seq_in_line < b.seq_in_line; });
    if (fTrain.direction == Backward)
        std::reverse(sts.begin(), sts.end());

    const QTime cur = selTime.time();

    // —— 2. 在排序后序列中找到 "A-B 区段" ——
    const flow_train_station* prev = nullptr;
    const flow_train_station* next = nullptr;

    for (int i = 0; i < sts.size(); ++i) {
        if (cur < sts[i].time) {                 // 当前时刻位于 sts[i-1] 与 sts[i] 之间
            if (i == 0) return false;            // 早于首站，不视为运行
            prev = &sts[i - 1];
            next = &sts[i];
            break;
        }
        if (cur == sts[i].time) {                // 刚好停靠在某站
            prev = next = &sts[i];
            break;
        }
    }
    if (!prev && !next) {                        // 晚于最后一站，视为未运行
        return false;
    }
    if (!next) next = prev;                      // 落在最后站之后的保护

    // —— 3. 车上人数 & 满载率 ——
    int peopleNow = prev->people;                // 若 between，取前站人数；若恰在站上 prev==next
    trainData.currentPeople = peopleNow;
    trainData.loadRate      = (trainData.capacity > 0)
                             ? static_cast<double>(peopleNow) / trainData.capacity
                             : 0.0;

    // —— 4. 当前位置经纬度 ——
    // 1) 取 prev 站在线路中的 seq
    int curSeq     = prev->seq_in_line;
    int lineId     = fTrain.line_id;
    int neighSeq   = (fTrain.direction == Forward) ? curSeq + 1 : curSeq - 1;
    int neighStaId = -1;

    // 2) 到 lineManager 查找 “与 curSeq 紧邻的那个 seq” 的站点 id
    const auto& lineVec = system->lineManager_.lines[lineId];
    for (const line& ln : lineVec) {
        if (ln.seq_in_line == neighSeq) {
            neighStaId = ln.station_id;
            break;
        }
    }

    // 3) 用 ip 对照表拿两站经纬度
    auto getCoord = [&](int sid)->QPair<double,double>{
        if (system->ipManager_.ips.contains(sid)) {
            const ip& rec = system->ipManager_.ips[sid];
            return {rec.jing, rec.wei};
        }
        return {0.0, 0.0};
    };

    QPair<double,double> coordPrev  = getCoord(prev->station_id);
    QPair<double,double> coordNeigh = (neighStaId != -1)
                                           ? getCoord(neighStaId)
                                           : coordPrev;           // 找不到邻站就用自身

    // 4) 若在站上(prev==next) ⇒ 用站点坐标；否则取 prev 与邻站坐标中点
    if (prev == next) {
        trainData.position = coordPrev;
    } else {
        trainData.position = {
            (coordPrev.first  + coordNeigh.first ) / 2.0,
            (coordPrev.second + coordNeigh.second) / 2.0
        };
    }

    return true;
}


void p0_fileManager::filterTrainsByTime(const QDateTime& selectedTime) {
    // 清空之前筛选的列车数据
    m_dynamicTrains.clear();

    for( auto it = system->flow_trainManager_.flow_trains[selectedTime.date()].begin() ;
         it != system->flow_trainManager_.flow_trains[selectedTime.date()].end(); ++it ){

        flow_train &m_train = it.value();
        if(!isTrainRunningNow(m_train,selectedTime)) continue;

        Train trainData;
        trainData.id           = m_train.train_id;
        trainData.code         = train_id_to_code[trainData.id];
        trainData.capacity     = m_train.capacity;
        trainData.startStation = station_id_to_name[m_train.start_station_id];
        trainData.endStation   = station_id_to_name[m_train.end_station_id];
        trainData.lineId       = m_train.line_id;

        if(!updateTrainRuntimeData(m_train, selectedTime, system, trainData)) continue;
        trainData.visible = m_dynamicLines[trainData.lineId].visible;

        m_dynamicTrains[trainData.id] = trainData;
    }


    qDebug() << "Filtered Trains Count:" << m_dynamicTrains.size();
}

void p0_fileManager::filterLinesByTrain()
{
    // —— 遍历所有动态线路，逐条计算匹配度 ——
    for (auto &linePair : m_dynamicLines) {
        Line &lineRef = linePair;              // 直接引用，便于写回

        long long peopleSum   = 0;             // 线路上所有列车的总乘客数
        long long capacitySum = 0;             // 线路上所有列车的总运力

        // —— 聚合属于该线路的列车数据 ——
        for (const auto &trPair : m_dynamicTrains) {
            const Train &tr = trPair;
            if (tr.lineId == lineRef.id) {
                peopleSum   += tr.currentPeople;
                capacitySum += tr.capacity;
            }
        }

        // —— 计算百分比（带小数） ——
        if (capacitySum > 0) {
            lineRef.capacityMatch = 100.0 * static_cast<double>(peopleSum) / capacitySum; // 例如 72.35
        } else {
            lineRef.capacityMatch = -1.0;        // 当前线路无车运行
        }
    }

}

void p0_fileManager::onTimeChanged(const QDateTime& dt)
{
    filterTrainsByTime(dt);     // 更新列车
    filterLinesByTrain();       // 更新线路匹配度

    emit sendLinesToLineWindow   (m_dynamicLines);
    emit sendTrainsToTrainWindow (m_dynamicTrains);

    emit sendLinesToMap          (m_dynamicLines);
    emit sendTrainsToMap         (m_dynamicTrains);

}



void p0_fileManager::onLineVisibilityChanged(int a, bool b) {

    qDebug()<<"线路"<<a<<"可见性设为：";
    if(b)qDebug()<<"可见";
    else qDebug()<<"不可见";

    m_dynamicLines[a].visible=b;

    for( auto& k : m_dynamicTrains){
        k.visible = m_dynamicLines[k.lineId].visible;
    }

    emit sendLinesToMap          (m_dynamicLines);
    emit sendTrainsToMap         (m_dynamicTrains);

}

void p0_fileManager::cancelChoose(){

    for(auto& a : m_dynamicLines){
        a.visible = false;
    }
    for( auto& b : m_dynamicTrains){
        b.visible = false;
    }

    emit sendLinesToMap          (m_dynamicLines);
    emit sendTrainsToMap         (m_dynamicTrains);
    emit sendLinesToLineWindow   (m_dynamicLines);
    qDebug()<<"清空路线选择";

}
