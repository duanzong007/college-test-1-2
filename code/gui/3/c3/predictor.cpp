#include "predictor.h"
#include "card3.h"
#include <QComboBox>
#include <QDateEdit>
#include <QLabel>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QMessageBox>
#include <QSet>
#include <random>
#include <algorithm>
#include <array>
#include <QDebug>

/* ===== 构造 ===== */
predictor::predictor(card3* uiCard, core_system* system)
    : QObject(uiCard), uiCard_(uiCard), sys_(system)
{
    stationCB_  = uiCard_->findChild<QComboBox*>("station");
    hourCB_     = uiCard_->findChild<QComboBox*>("hour");
    startDE_    = uiCard_->findChild<QDateEdit*>("chooseTime");
    aimLB_      = uiCard_->findChild<QLabel*>("aimTime");
    confirmBtn_ = uiCard_->findChild<QPushButton*>("confirm");
    tableView_  = uiCard_->findChild<QTableView*>("tableView");

    initUI();

    connect(stationCB_,  QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &predictor::onStationChanged);
    connect(hourCB_,     QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &predictor::onHorizonChanged);
    connect(startDE_,    &QDateEdit::dateChanged,
            this, &predictor::onStartDateChanged);
    connect(confirmBtn_, &QPushButton::clicked,
            this, &predictor::onConfirmClicked);


}

void predictor::initUI()
{
    /* ---------- ① 收集“既有数据”的站点 id ---------- */
    QSet<int> idSet;
    for (auto it = sys_->flow_stationManager_.flow_stations.begin();
         it != sys_->flow_stationManager_.flow_stations.end(); ++it)
    {
        const QList<int> keyList = it.value().keys();   // Qt6: keys() 返回 QList<int>
        for (int id : keyList)                          // 手动插入到 QSet
            idSet.insert(id);
    }

    /* ---------- ② 用 stationManager_ 找到对应中文名 ---------- */
    struct Pair { int id; QString name; };
    QVector<Pair> stations;
    for (const auto &s : sys_->stationManager_.stations) {
        if (idSet.contains(s.station_id))
            stations.push_back({s.station_id, s.station_name});
    }
    std::sort(stations.begin(), stations.end(),
              [](const Pair& a, const Pair& b){ return a.name < b.name; });

    /* ---------- ③ 填 QComboBox：显示中文名，UserRole 存数值 id ---------- */
    stationCB_->clear();
    for (const auto &p : stations)
        stationCB_->addItem(p.name, p.id);              // 文本 = 名称，数据 = id

    /* ---------- ④ 时长列表 & 默认起点 ---------- */
    hourCB_->clear();
    hourCB_->addItems({"24","48","72"});
    hourCB_->setCurrentIndex(0);

    if (!sys_->flow_stationManager_.flow_stations.isEmpty()) {
        QDate lastDate = sys_->flow_stationManager_.flow_stations.lastKey().date();
        startDE_->setDate(lastDate.addDays(1));         // 默认 = 最新日期 + 1
    }

    refreshAimTime();
}


/* ===== 辅助 ===== */
void predictor::refreshAimTime()
{
    int h = hourCB_->currentText().toInt();
    QDateTime startDT(startDE_->date(), QTime(0,0));
    QDateTime endDT = startDT.addSecs(h*3600);

    QString end = endDT.date().toString("yyyy/M/d");
    aimLB_->setText(end);
}

void predictor::onStartDateChanged(const QDate&){ refreshAimTime(); }
void predictor::onHorizonChanged(int){ refreshAimTime(); }
void predictor::onStationChanged(int){}

bool predictor::validateInputs(QString& err)
{
    if (stationCB_->currentText().isEmpty()){
        err = "请选择站点"; return false;
    }
    return true;
}

/* ===== 主槽 ===== */
void predictor::onConfirmClicked(){ runForecast(); }

/* ===== 主流程 ===== */
void predictor::runForecast()
{
    QString err;
    if(!validateInputs(err)){
        QMessageBox::warning(uiCard_, "输入错误", err); return;
    }

    int sid = stationCB_->currentData().toInt();

    QVector<int> totHist;
    QDateTime first, last;
    buildFullSeries(sid, totHist, first, last);
    if (totHist.isEmpty()){
        QMessageBox::information(uiCard_,"无数据","该站点没有历史记录"); return;
    }

    int horizonUser = hourCB_->currentText().toInt();
    QDateTime startDT(startDE_->date(), QTime(0,0));
    qint64 gapH = last.secsTo(startDT) / 3600;
    int totalH = int(qMax<qint64>(0,gapH)) + horizonUser;

    QVector<int> totPred = forecastSeriesSmart(totHist, totalH);
    int cut = totPred.size() - horizonUser;
    QVector<int> totShow(totPred.begin()+cut, totPred.end());

    QVector<int> bdShow, alShow;
    bdShow.reserve(horizonUser); alShow.reserve(horizonUser);
    for (int v: totShow){
        int b = int(qRound(v * boardRatio_));
        bdShow << b;
        alShow << v - b;
    }

    QVector<QDateTime> ts;
    ts.reserve(horizonUser);
    for (int i=0;i<horizonUser;++i)
        ts << startDT.addSecs(i*3600);

    fillTable(ts, bdShow, alShow);
}

/* ===== 构造完整小时序列 & 计算 clamp / ratio ===== */
void predictor::buildFullSeries(int sid,
                                QVector<int>& tot,
                                QDateTime& first,
                                QDateTime& last)
{
    const auto& raw = sys_->flow_stationManager_.flow_stations;
    if (raw.isEmpty()) return;

    first = raw.firstKey().addSecs(-(raw.firstKey().time().minute()*60
                                     + raw.firstKey().time().second()));
    last  = raw.lastKey();

    /* 归档日 → 24h 数组，同时收集非零值 */
    QMap<QDate, std::array<int,24>> dayMap;
    QVector<int> nonZero;
    qint64 bdSum=0, totSum=0;

    for (auto it = raw.begin(); it != raw.end(); ++it){
        const auto &inner = it.value();
        if (!inner.contains(sid)) continue;

        const auto &st = inner[sid];
        int v = st.ab_count;
        QDate d = it.key().date();
        int h = it.key().time().hour();
        if (!dayMap.contains(d)) dayMap[d].fill(-1);
        dayMap[d][h] = v;
        if (v>0) nonZero << v;

        if (st.ab_count && st.boarding_count && st.alighting_count){
            bdSum  += st.boarding_count;
            totSum += st.ab_count;
        }
    }
    boardRatio_ = totSum ? double(bdSum)/totSum : 0.5;

    /* 计算中位数 ×5 作为离群上限 */
    if (!nonZero.isEmpty()){
        std::nth_element(nonZero.begin(),
                         nonZero.begin()+nonZero.size()/2,
                         nonZero.end());
        int median = nonZero[nonZero.size()/2];
        clampMax_ = qMax(10000, median * 5);
    }

    /* weekday-hour 均值用于整天克隆 */
    double wkAvg[7][24]={{0}}; int wkCnt[7][24]={{0}};
    for (auto it = dayMap.begin(); it != dayMap.end(); ++it){
        int wd = it.key().dayOfWeek()%7;
        for (int h=0; h<24; ++h){
            int v = it.value()[h];
            if (v>=0){ wkAvg[wd][h]+=v; wkCnt[wd][h]++; }
        }
    }
    for(int w=0; w<7; ++w)
        for(int h=0; h<24; ++h)
            wkAvg[w][h] = wkCnt[w][h] ? wkAvg[w][h]/wkCnt[w][h] : 0;

    /* 补零 / 克隆整天，并裁剪离群 */
    QDate cur = first.date();
    while(cur <= last.date()){
        std::array<int,24> day;
        if (dayMap.contains(cur)) day = dayMap[cur];
        else day.fill(-1);

        for(int h=0; h<24; ++h){
            int v = day[h];
            if (v<0){
                int wd = cur.dayOfWeek()%7;
                v = int(qRound(wkAvg[wd][h]));
            }
            if (v > clampMax_) v = clampMax_;
            tot << qMax(0,v);
        }
        cur = cur.addDays(1);
    }
}

/* ===== 预测：自适应窗口 + 噪声 + 裁剪 ===== */
QVector<int> predictor::forecastSeriesSmart(const QVector<int>& hist,
                                            int H)
{
    if (hist.isEmpty()) return {};
    constexpr int HPD = 24;
    const int LOOKS[] = {14,30,60,3650};   // 动态回溯
    QVector<int> out; out.reserve(H);
    int n = hist.size();

    std::default_random_engine eng(
        quint32(QDateTime::currentSecsSinceEpoch()&0xffffffff));
    std::normal_distribution<double> noise(0.0,0.05);

    for (int h=0; h<H; ++h){
        int g = n-HPD+h;                   // 昨天同小时索引
        double base=0; int cnt=0;

        for(int span: LOOKS){
            base=0; cnt=0;
            for(int d=1; d<=span; ++d){
                int idx = g - d*HPD; if (idx<0) break;
                int v = hist[idx]; if (v>0){ base+=v; ++cnt; }
            }
            if(cnt>0){ base/=cnt; break; }
        }
        double recent = (g>=0 && g<n && hist[g]>0) ? hist[g] : base;
        double pred = (base==0&&recent==0)? 0 : 0.8*base + 0.2*recent;
        pred *= (1.0 + noise(eng));
        if (pred > clampMax_) pred = clampMax_;
        out << int(qRound(pred));
    }
    return out;
}

/* ===== 填表 ===== */
void predictor::fillTable(const QVector<QDateTime>& ts,
                          const QVector<int>& bd,
                          const QVector<int>& al)
{
    int N = ts.size();
    auto *m = new QStandardItemModel(N,4,tableView_);
    m->setHorizontalHeaderLabels({"时间","上客量","下客量","总客流量"});

    for(int i=0;i<N;++i){
        int tot = bd[i] + al[i];
        m->setItem(i,0,new QStandardItem(ts[i].toString("yyyy-MM-dd HH:mm")));
        m->setItem(i,1,new QStandardItem(QString::number(bd[i])));
        m->setItem(i,2,new QStandardItem(QString::number(al[i])));
        m->setItem(i,3,new QStandardItem(QString::number(tot)));
    }
    tableView_->setModel(m);
    tableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}
