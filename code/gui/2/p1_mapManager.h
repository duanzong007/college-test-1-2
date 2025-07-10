#ifndef P1_MAPMANAGER_H
#define P1_MAPMANAGER_H

#include <QObject>
#include <QWebEngineView>
#include <QWebChannel>
#include <QVariantMap>
#include <QList>

class p1_mapManager : public QObject {
    Q_OBJECT
public:
    explicit p1_mapManager(QWebEngineView* view, QObject* parent = nullptr);

public slots:
    /** 接收文件管理器信号：刷新热力图 */
    void s_refreshHeatmap(const QList<QVariantMap>& data);
    /** 接收文件管理器信号：刷新站点标注 */
    void s_refreshStations(const QList<QVariantMap>& data);

private slots:
    void onLoadFinished(bool ok);

private:
    QWebEngineView*    m_view;
    QWebChannel*       m_channel;
    bool               m_pageLoaded = false;
    QList<QVariantMap> m_pendingHeatmap;
    QList<QVariantMap> m_pendingStations;
};

#endif // P1_MAPMANAGER_H
