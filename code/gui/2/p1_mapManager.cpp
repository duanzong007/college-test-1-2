#include "p1_mapManager.h"
#include <QWebEnginePage>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

p1_mapManager::p1_mapManager(QWebEngineView* view, QObject* parent)
    : QObject(parent)
    , m_view(view)
    , m_channel(new QWebChannel(this))
{
    // 暴露 bridge 给 JS
    m_channel->registerObject(QStringLiteral("bridge"), this);
    m_view->page()->setWebChannel(m_channel);

    // 等待页面加载完成
    connect(m_view, &QWebEngineView::loadFinished,
            this,   &p1_mapManager::onLoadFinished);

    // 加载资源中的 flow.html
    m_view->load(QUrl(QStringLiteral("qrc:/html/page1/flow.html")));
}

void p1_mapManager::onLoadFinished(bool ok) {
    if (!ok) return;
    m_pageLoaded = true;
    if (!m_pendingHeatmap.isEmpty())
        s_refreshHeatmap(m_pendingHeatmap);
    if (!m_pendingStations.isEmpty())
        s_refreshStations(m_pendingStations);
}

void p1_mapManager::s_refreshHeatmap(const QList<QVariantMap>& data) {
    if (!m_pageLoaded) {
        m_pendingHeatmap = data;
        return;
    }
    QJsonArray arr;
    for (const auto& item : data) {
        QJsonObject o;
        o["jing"] = item.value("jing").toDouble();
        o["wei"]  = item.value("wei").toDouble();
        o["flow"] = item.value("flow").toInt();
        arr.append(o);
    }
    QJsonObject root; root["data"] = arr;
    QString js = QStringLiteral("refreshHeatmap(%1);")
                     .arg(QString::fromUtf8(QJsonDocument(root)
                                                .toJson(QJsonDocument::Compact)));
    m_view->page()->runJavaScript(js);
}

void p1_mapManager::s_refreshStations(const QList<QVariantMap>& data) {
    if (!m_pageLoaded) {
        m_pendingStations = data;
        return;
    }
    QJsonArray arr;
    for (const auto& item : data) {
        QJsonObject o;
        o["jing"]     = item.value("jing").toDouble();
        o["wei"]      = item.value("wei").toDouble();
        o["flow"]     = item.value("flow").toInt();
        o["pressure"] = item.value("pressure").toDouble();
        o["name"]     = item.value("name").toString();
        arr.append(o);
    }
    QString js = QStringLiteral("refreshStations(%1);")
                     .arg(QString::fromUtf8(QJsonDocument(arr)
                                                .toJson(QJsonDocument::Compact)));
    m_view->page()->runJavaScript(js);
}
