#include "orderManager.h"
#include <QThreadPool>  // 引入 QThreadPool

orderManager::orderManager(){
    orders.reserve(510000); // 根据预计的数据量来进行预留内存
    QThreadPool::globalInstance()->setMaxThreadCount(QThread::idealThreadCount());  // 让线程池大小适应 CPU 核心数
}

// 合并日期和时间为标准的时间戳
QDateTime orderManager::combineTimestamp(const QString& date, const QString& time) {
    QString formattedDate = date.trimmed();
    QString formattedTime = time.trimmed();

    // 将日期格式化为 QDate
    int year = formattedDate.left(4).toInt();
    int month = formattedDate.mid(4, 2).toInt();
    int day = formattedDate.mid(6, 2).toInt();
    QDate qDate(year, month, day);  // 使用 QDate 构造日期

    // 将时间格式化为 QTime
    int hour = formattedTime.left(2).toInt();
    int minute = formattedTime.mid(2, 2).toInt();
    QTime qTime(hour, minute);  // 使用 QTime 构造时间

    // 合并 QDate 和 QTime 为 QDateTime
    return QDateTime(qDate, qTime);
}

// 处理每条数据的函数（非静态成员函数）
void orderManager::processOrderData(const forder& r)
{
    // 校验必填字段
    if (r.xh <= 0 || r.yyxlbm <= 0 || r.lcbm <= 0 || r.zdid <= 0) {
        qWarning() << "无效的订单数据，跳过";
        return;
    }
    // 合并时间戳
    const QDateTime timestamp = combineTimestamp(r.yxrq, r.yxsj);

    // 创建订单对象并推入 orders
    order o(r.xh, r.yyxlbm, r.lcbm, r.zdid, timestamp,
            r.skl.toInt(), r.xkl.toInt(), r.ticket_price.toDouble(),
            r.seat_type_code, r.start_station_telecode, r.start_station_name,
            r.end_station_telecode, r.end_station_name, r.shouru.toDouble());



    QMutexLocker locker(&ordersMutex);
    orders.push_back(o);  // 安全地修改 orders
}

void orderManager::qdebugShow() const
{
    auto it = orders.begin();
    for (int i=0;i<10;i++) {
        auto o=*it;
        qDebug().nospace().noquote()
            << "订单ID:" << o.order_id
            << " 线路ID:" << o.line_id
            << " 列车ID:" << o.train_id
            << " 站点ID:" << o.station_id
            << " 时间戳:" << o.timestamp.toString("yyyy-MM-dd hh:mm")
            << " 上客量:" << o.boarding_count
            << " 下客量:" << o.alighting_count
            << " 车票价格:" << o.price
            << " 座位类型编码:" << o.seat_type_code
            << " 起始站电报码:" << o.start_station_telecode
            << " 起点站名称:" << o.start_station_name
            << " 终点站电报码:" << o.end_station_telecode
            << " 终到站名称:" << o.end_station_name
            << " 收入:" << o.income;
        it++;
    }
}


bool orderManager::convertOrderData(const QVector<forder>& rawFile)
{
    qDebug() << "清洗订单数据";

    // 设定每个线程处理的数据块大小
    int chunkSize = 5000;  // 每个线程处理 80000 个数据
    int numChunks = (rawFile.size() + chunkSize - 1) / chunkSize;  // 计算总共有多少个块

    QVector<QFuture<void>> futures;  // 用来存储每个并行任务的 QFuture

    // 将数据分块并行处理
    for (int i = 0; i < numChunks; ++i) {
        int start = i * chunkSize;  // 当前块的起始索引
        int end = qMin((i + 1) * chunkSize, rawFile.size());  // 当前块的结束索引（防止越界）

        // 每个线程处理一个数据块
        futures.append(QtConcurrent::run([this, &rawFile, start, end]() {
            for (int j = start; j < end; ++j) {
                processOrderData(rawFile[j]);  // 处理这块数据
            }
        }));
         QCoreApplication::processEvents();
    }

    // 等待所有线程完成
    for (auto& future : futures) {
        future.waitForFinished();
        QCoreApplication::processEvents();
    }
    QCoreApplication::processEvents();
    // // 排序 `orders` 按照 order_id 或者 index
    // std::sort(orders.begin(), orders.end(), [](const order &a, const order &b) {
    //     return a.order_id < b.order_id;  // 根据 `order_id` 排序
    // });


    // 1) 在后台线程里跑排序
    auto sortFuture = QtConcurrent::run([this]() {
        std::sort(orders.begin(), orders.end(),
                  [](const order &a, const order &b) {
                      return a.order_id < b.order_id;
                  });
    });

    // 2) 在主线程中轮询并让出事件循环，保证 UI 不会卡死
    while (!sortFuture.isFinished()) {
        QCoreApplication::processEvents();
        // 如果你觉得这里太紧凑，可以稍微 sleep 一下：
        // QThread::msleep(1);
    }


    qDebug() << "订单数据清洗完成";
    return !orders.isEmpty();  // 如果订单列表不为空，则返回 true
}


