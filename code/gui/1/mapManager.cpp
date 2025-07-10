#include "mapManager.h"
#include <QtWebEngineCore/QWebEnginePage>
#include <QUrl>
#include <QVariantList>

mapManager::mapManager(QWebEngineView* v, QObject* parent)
    : QObject(parent), m_view(v)
{
    m_channel.registerObject("bridge", this);          // JS 端名字 bridge
    m_view->page()->setWebChannel(&m_channel);
    m_view->load(QUrl("qrc:/html/page0/map.html"));          // 或磁盘路径

    connect(m_view->page(), &QWebEnginePage::loadFinished,
            this, &mapManager::onPageReady);
}

void mapManager::onPageReady(bool ok)
{
    if(!ok) return;
    m_ready = true;
    if(!m_lastLines.isEmpty())  emit refreshLines(m_lastLines);
    if(!m_lastTrains.isEmpty()) emit refreshTrains(m_lastTrains);
}



/* ---------- 线路 ---------- */
void mapManager::s_refreshLines(const QMap<int, Line>& lines)
{
    QList<QVariantMap> arr;
    for(const Line& ln : lines){
        QVariantList sts;                       // [{jing,wei,seq,id}, ...]
        for(const LineStation& s: ln.stations){
            QVariantMap sp; sp["jing"]=s.jing; sp["wei"]=s.wei;
            sp["seq"]=s.seq_in_line; sp["id"]=s.station_id;  sts<<sp;
        }
        QVariantMap obj;
        obj["id"]=ln.id; obj["color"]=ln.color.name();
        obj["visible"]=ln.visible; obj["stations"]=sts;
        arr<<obj;
    }
    m_lastLines = arr;
    if(m_ready) emit refreshLines(arr);
}

/* ---------- 列车 ---------- */
void mapManager::s_refreshTrains(const QMap<int, Train>& trains)
{
    QList<QVariantMap> arr;
    for (const Train& tr : trains) {
        if (!tr.visible) continue;

        // 1) 先创建一个子 map 存放 first/second
        QVariantMap pos;
        pos["first"]  = tr.position.first;
        pos["second"] = tr.position.second;

        // 2) 构造最终要发给 JS 的对象
        QVariantMap obj;
        obj["id"]        = tr.id;
        obj["code"]      = tr.code;
        obj["visible"]   = true;
        obj["position"]  = pos;      // ← 这里改成一个子对象
        obj["endStation"]    = tr.endStation;
        obj["currentPeople"] = tr.currentPeople;
        obj["loadRate"]      = tr.loadRate;
        arr << obj;
    }

    m_lastTrains = arr;
    // 如果页面已经 ready，就立刻把新数据发给 JS
    if (m_ready)
        emit refreshTrains(arr);
}

