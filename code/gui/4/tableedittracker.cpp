#include "tableedittracker.h"

TableEditTracker::TableEditTracker(core_system* system, QObject *parent)
    : QObject{parent},m_system(system)
{}

void TableEditTracker::clear(){
    m_editRecords.clear();
    m_trainEditRecords.clear();
    m_stationEditRecords.clear();
    m_orderEditRecords.clear();
}

void TableEditTracker::recordEdit(int page, int row, int col, const QString& newValue, const QString& tableType) {
    if (tableType == "lines") {
        lineEditRecord record;
        record.page = page;
        record.row = row;
        record.col = col;
        record.newValue = newValue;
        record.tableType = tableType;
        m_editRecords.append(record);
    }
    else if (tableType == "trains") {
        trainEditRecord record;
        record.page = page;
        record.row = row;
        record.col = col;
        record.newValue = newValue;
        record.tableType = tableType;
        m_trainEditRecords.append(record);
    }
    else if (tableType == "stations") {
        stationEditRecord record;
        record.page = page;
        record.row = row;
        record.col = col;
        record.newValue = newValue;
        record.tableType = tableType;
        m_stationEditRecords.append(record);
    }
    else if (tableType == "orders") {
        orderEditRecord record;
        record.page = page;
        record.row = row;
        record.col = col;
        record.newValue = newValue;
        record.tableType = tableType;
        m_orderEditRecords.append(record);
    }
}

bool TableEditTracker::saveLineChanges() {
    if (!m_system) {
        qWarning() << "core_system 未初始化";
        return false;
    }

    lineManager& manager = m_system->lineManager_;
    if (manager.lines.isEmpty()) {
        qWarning() << "线路数据为空";
        return false;
    }

    bool hasChanges = false;
    const int pageSize = 200;

    // 构建与分页匹配的线性索引（与loadLineData完全一致）
    QVector<QPair<int, const line*>> indexedLines; // <line_id, line_ptr>
    for (auto it = manager.lines.constBegin(); it != manager.lines.constEnd(); ++it) {
        int line_id = it.key();
        for (const line& l : it.value()) {
            indexedLines.append(qMakePair(line_id, &l));
        }
    }

    for (const auto& edit : m_editRecords) {
        if (edit.tableType != "lines") continue;

        int dataIndex = (edit.page - 1) * pageSize + edit.row;
        if (dataIndex < 0 || dataIndex >= indexedLines.size()) {
            qDebug() << "忽略无效索引:" << dataIndex;
            continue;
        }

        // 获取目标数据
        int target_line_id = indexedLines[dataIndex].first;
        const line* target_line = indexedLines[dataIndex].second;

        // 在原始数据中定位
        if (!manager.lines.contains(target_line_id)) {
            qDebug() << "线路ID不存在:" << target_line_id;
            continue;
        }

        QVector<line>& lines = manager.lines[target_line_id];
        auto it = std::find_if(lines.begin(), lines.end(),
                               [&](const line& l) {
                                   return l.station_id == target_line->station_id &&
                                          l.seq_in_line == target_line->seq_in_line;
                               });

        if (it == lines.end()) continue;

        line& lineToUpdate = *it;
        QString oldValue;

        // 列映射与initLineTable()表头顺序严格一致
        switch (edit.col) {
        case 0: oldValue = QString::number(target_line_id); break;      // 线路编码
        case 1: oldValue = QString::number(lineToUpdate.station_id); break; // 站点ID
        case 2: oldValue = QString::number(lineToUpdate.seq_in_line); break; // 序号
        case 3: oldValue = QString::number(lineToUpdate.prev_station_id); break;
        case 4: oldValue = QString::number(lineToUpdate.next_station_id); break;
        case 5: oldValue = QString::number(lineToUpdate.interstation_distance); break;
        case 6: oldValue = lineToUpdate.line_code; break;
        case 7: oldValue = lineToUpdate.is_stop ? "是" : "否"; break;
        default: continue;
        }

        if (oldValue != edit.newValue) {
            // 更新数据
            switch (edit.col) {
            case 1: lineToUpdate.station_id = edit.newValue.toInt(); break;
            case 2: lineToUpdate.seq_in_line = edit.newValue.toInt(); break;
            case 3: lineToUpdate.prev_station_id = edit.newValue.toInt(); break;
            case 4: lineToUpdate.next_station_id = edit.newValue.toInt(); break;
            case 5: lineToUpdate.interstation_distance = edit.newValue.toDouble(); break;
            case 6: lineToUpdate.line_code = edit.newValue; break;
            case 7: lineToUpdate.is_stop = (edit.newValue == "是"); break;
            }
            hasChanges = true;
            qDebug() << "修改保存 - 线路:" << target_line_id
                     << " 站点:" << lineToUpdate.station_id
                     << " 列" << edit.col << ":" << oldValue << "→" << edit.newValue;
        }
    }

    if (hasChanges) {
        m_editRecords.erase(
            std::remove_if(m_editRecords.begin(), m_editRecords.end(),
                           [](const auto& r) { return r.tableType == "lines"; }),
            m_editRecords.end());

        return true;
    }
    return false;


}

bool TableEditTracker::saveLineFlie() {
    fileMgr = new fileManager;
    fileMgr->inputLine();


    //改文件的逻辑

    if (!m_system) {
        qWarning() << "core_system 未初始化";
        return false;
    }

    lineManager& lineMgr = m_system->lineManager_;
    for (auto it = lineMgr.lines.constBegin(); it != lineMgr.lines.constEnd(); ++it) {
        int line_id = it.key();
        const QVector<line>& lines = it.value();

        for (const line& l : lines) {
            // 在 fileMgr.line_file 中查找对应的记录并更新
            bool found = false;
            for (fline& fl : fileMgr->line_file) {
                if (fl.yyxlbm == line_id && fl.zdid == l.station_id) {
                    // 更新字段 - 与原文件结构保持一致
                    fl.xlzdid = QString::number(l.seq_in_line);
                    fl.Q_zdid = QString::number(l.prev_station_id);
                    fl.H_zdid = QString::number(l.next_station_id);
                    fl.yqzdjjl = QString::number(l.interstation_distance);
                    fl.xldm = l.line_code;
                    fl.sfytk = l.is_stop ? "1" : "0";

                    // 设置起始/终点站标志
                    fl.sfqszd = (l.prev_station_id == -1) ? "1" : "0";
                    fl.sfzdzd = (l.next_station_id == -1) ? "1" : "0";

                    found = true;
                    break;
                }
            }

            if (!found) {
                qWarning() << "未找到匹配的记录，线路ID:" << line_id << "站点ID:" << l.station_id;
            }
        }
    }


    fileMgr->saveLines();

    auto fileMgr_ = fileMgr;
    delete fileMgr_;
    fileMgr = nullptr;
    return true;

}

bool TableEditTracker::saveTrainChanges() {
    if (!m_system) {
        qWarning() << "core_system 未初始化";
        return false;
    }

    trainManager& manager = m_system->trainManager_;
    if (manager.trains.isEmpty()) {
        qWarning() << "列车数据为空";
        return false;
    }

    bool hasChanges = false;
    const int pageSize = 200;

    for (const auto& edit : m_trainEditRecords) {
        if (edit.tableType != "trains") continue;

        int dataIndex = (edit.page - 1) * pageSize + edit.row;
        if (dataIndex < 0 || dataIndex >= manager.trains.size()) {
            qDebug() << "忽略无效索引:" << dataIndex;
            continue;
        }

        train& trainToUpdate = manager.trains[dataIndex];
        QString oldValue;

        // 列映射与initTrainTable()表头顺序严格一致
        switch (edit.col) {
        case 0: oldValue = QString::number(trainToUpdate.train_id); break;
        case 1: oldValue = trainToUpdate.train_number; break;
        case 2: oldValue = trainToUpdate.is_in_service ? "是" : "否"; break;
        case 3: oldValue = QString::number(trainToUpdate.train_capacity); break;
        default: continue;
        }

        if (oldValue != edit.newValue) {
            // 更新数据
            switch (edit.col) {
            case 1: trainToUpdate.train_number = edit.newValue; break;
            case 2: trainToUpdate.is_in_service = (edit.newValue == "是"); break;
            case 3: trainToUpdate.train_capacity = edit.newValue.toInt(); break;
            }
            hasChanges = true;
            qDebug() << "修改保存 - 列车:" << trainToUpdate.train_id
                     << " 列" << edit.col << ":" << oldValue << "→" << edit.newValue;
        }
    }

    if (hasChanges) {
        m_trainEditRecords.erase(
            std::remove_if(m_trainEditRecords.begin(), m_trainEditRecords.end(),
                           [](const auto& r) { return r.tableType == "trains"; }),
            m_trainEditRecords.end());
        return true;
    }
    return false;
}

// 保存列车修改到文件
bool TableEditTracker::saveTrainFile() {
    fileMgr = new fileManager;
    fileMgr->inputTrain();

    if (!m_system) {
        qWarning() << "core_system 未初始化";
        return false;
    }

    trainManager& trainMgr = m_system->trainManager_;
    for (const train& t : trainMgr.trains) {
        // 在 fileMgr.train_file 中查找对应的记录并更新
        bool found = false;
        for (ftrain& ft : fileMgr->train_file) {
            if (ft.lcbm == t.train_id) {
                // 更新字段 - 与原文件结构保持一致
                ft.lcdm = t.train_number;
                ft.sfzt = t.is_in_service ? "1" : "0";
                ft.lcyn = QString::number(t.train_capacity);
                found = true;
                break;
            }
        }

        if (!found) {
            qWarning() << "未找到匹配的记录，列车ID:" << t.train_id;
        }
    }

    fileMgr->saveTrains();
    delete fileMgr;
    fileMgr = nullptr;
    return true;
}

bool TableEditTracker::saveStationChanges() {
    if (!m_system) {
        qWarning() << "core_system 未初始化";
        return false;
    }

    stationManager& manager = m_system->stationManager_;
    if (manager.stations.isEmpty()) {
        qWarning() << "站点数据为空";
        return false;
    }
    bool hasChanges = false;
    const int pageSize = 200;

    for (const auto& edit : m_stationEditRecords) {
        if (edit.tableType != "stations") continue;

        int dataIndex = (edit.page - 1) * pageSize + edit.row;
        if (dataIndex < 0 || dataIndex >= manager.stations.size()) {
            qDebug() << "忽略无效索引:" << dataIndex;
            continue;
        }

        station& stationToUpdate = manager.stations[dataIndex];
        QString oldValue;

        switch (edit.col) {
        case 0: oldValue = QString::number(stationToUpdate.station_id); break;
        case 1: oldValue = stationToUpdate.station_name; break;
        case 2: oldValue = stationToUpdate.is_in_service ? "否" : "是"; break;
        case 3: oldValue = stationToUpdate.station_code; break;
        case 4: oldValue = stationToUpdate.station_telecode; break;
        case 5: oldValue = stationToUpdate.station_shortname; break;
        default: continue;
        }
        if (oldValue != edit.newValue) {
            // 更新数据
            switch (edit.col) {
            case 1: stationToUpdate.station_name = edit.newValue; break;
            case 2: stationToUpdate.is_in_service = (edit.newValue == "否"); break;
            case 3: stationToUpdate.station_code = edit.newValue; break;
            case 4: stationToUpdate.station_telecode = edit.newValue; break;
            case 5: stationToUpdate.station_shortname = edit.newValue; break;
            }
            hasChanges = true;
            qDebug() << "修改保存 - 站点:" << stationToUpdate.station_id
                     << " 列" << edit.col << ":" << oldValue << "→" << edit.newValue;
        }
    }
    if (hasChanges) {
        m_stationEditRecords.erase(
            std::remove_if(m_stationEditRecords.begin(), m_stationEditRecords.end(),
                           [](const auto& r) { return r.tableType == "stations"; }),
            m_stationEditRecords.end());
        return true;
    }
    return false;
}

bool TableEditTracker::saveStationFile() {
    fileMgr = new fileManager;
    fileMgr->inputStation();

    if (!m_system) {
        qWarning() << "core_system 未初始化";
        return false;
    }

    stationManager& stationMgr = m_system->stationManager_;
    for (const station& s : stationMgr.stations) {
        // 在 fileMgr.station_file 中查找对应的记录并更新
        bool found = false;
        for (fstation& fs : fileMgr->station_file) {
            if (fs.zdid == s.station_id) {
                // 更新字段 - 与原文件结构保持一致
                fs.zdmc = s.station_name;
                fs.sfty = s.is_in_service ? "0" : "1";
                fs.station_code = s.station_code;
                fs.station_telecode = s.station_telecode;
                fs.station_shortname = s.station_shortname;
                found = true;
                break;
            }
        }
        if (!found) {
            qWarning() << "未找到匹配的记录，站点ID:" << s.station_id;
        }
    }

    fileMgr->saveStations();
    delete fileMgr;
    fileMgr = nullptr;
    return true;
}


bool TableEditTracker::saveOrderChanges() {
    if (!m_system) {
        qWarning() << "core_system 未初始化";
        return false;
    }

    orderManager& manager = m_system->orderManager_;
    if (manager.orders.isEmpty()) {
        qWarning() << "订单数据为空";
        return false;
    }

    // 加载文件数据
    fileMgr = new fileManager;
    if (!fileMgr->inputOrder()) {
        qWarning() << "加载订单文件失败";
        delete fileMgr;
        return false;
    }

    bool hasChanges = false;
    const int pageSize = 200;
    for (const auto& edit : m_orderEditRecords) {
        if (edit.tableType != "orders") continue;
        int dataIndex = (edit.page - 1) * pageSize + edit.row;
        if (dataIndex < 0 || dataIndex >= manager.orders.size()) {
            qDebug() << "忽略无效索引:" << dataIndex;
            continue;
        }

        //qDebug() << "修改的订单id：" << orderId;

        order& orderToUpdate = manager.orders[dataIndex];
        QString oldValue;
        switch (edit.col) {
        case 0: oldValue = QString::number(orderToUpdate.order_id); break;
        case 1: oldValue = QString::number(orderToUpdate.line_id); break;
        case 2: oldValue = QString::number(orderToUpdate.train_id); break;
        case 3: oldValue = QString::number(orderToUpdate.station_id); break;
        case 4: oldValue = orderToUpdate.timestamp.toString("yyyy-MM-dd hh:mm"); break;
        case 5: oldValue = QString::number(orderToUpdate.boarding_count); break;
        case 6: oldValue = QString::number(orderToUpdate.alighting_count); break;
        case 7: oldValue = QString::number(orderToUpdate.price, 'f', 2); break;
        case 8: oldValue = orderToUpdate.seat_type_code; break;
        case 9: oldValue = QString::number(orderToUpdate.income, 'f', 2); break;
        default: continue;
        }

        if (oldValue != edit.newValue) {
            // 更新数据
            switch (edit.col) {
            case 4: orderToUpdate.timestamp = QDateTime::fromString(edit.newValue, "yyyy-MM-dd hh:mm"); break;
            case 5: orderToUpdate.boarding_count = edit.newValue.toInt(); break;
            case 6: orderToUpdate.alighting_count = edit.newValue.toInt(); break;
            case 7: orderToUpdate.price = edit.newValue.toDouble(); break;
            case 8: orderToUpdate.seat_type_code = edit.newValue; break;
            case 9: orderToUpdate.income = edit.newValue.toDouble(); break;
            }
            int orderId = manager.orders[dataIndex].order_id;
            qDebug() << "修改的订单id：" << orderId;

            for (auto& fo : fileMgr->order_file) {

                if (fo.xh == orderId) {
                    switch (edit.col+1) {
                    case 2: fo.yyxlbm = edit.newValue.toInt(); break;
                    case 3: fo.lcbm = edit.newValue.toInt(); break;
                    case 4: fo.zdid = edit.newValue.toInt(); break;
                    case 5: fo.yxrq = edit.newValue; break;
                    case 6: fo.skl = edit.newValue; break;
                    case 7: fo.xkl = edit.newValue; break;
                    case 8: fo.ticket_price = edit.newValue; break;
                    case 9: fo.seat_type_code = edit.newValue; break;
                    case 10: fo.shouru = edit.newValue; break;
                    default:
                        break;
                    }
                    break;
                }
            }

            hasChanges = true;
            qDebug() << "修改保存 - 订单:" << orderToUpdate.order_id
                     << " 列" << edit.col << ":" << oldValue << "→" << edit.newValue;
        }
    }

    // 保存修改后的文件
    fileMgr->saveOrders();
    delete fileMgr;
    fileMgr = nullptr;
    if (hasChanges) {

        // 清除已处理的修改记录
        m_orderEditRecords.erase(
            std::remove_if(m_orderEditRecords.begin(), m_orderEditRecords.end(),
                           [](const auto& r) { return r.tableType == "orders"; }),
            m_orderEditRecords.end());
        return true;
    }
    return false;
}

bool TableEditTracker::saveOrderFile() {
    return true;
}

const QList<lineEditRecord>& TableEditTracker::getRecords() const
{
    return m_editRecords;
}
