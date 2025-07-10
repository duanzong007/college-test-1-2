#ifndef MAPMANAGER_H
#define MAPMANAGER_H
#include <QObject>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtWebChannel/QWebChannel>
#include <QMap>
#include <QVariantMap>
#include "p0_fileManager.h"

class mapManager : public QObject
{
    Q_OBJECT
public:
    mapManager(QWebEngineView* view, QObject* parent=nullptr);

public slots:
    void s_refreshLines(const QMap<int, Line>&);   // 接 fileMgr
    void s_refreshTrains(const QMap<int, Train>&);

signals:                                         // 暴露给 JS
    void refreshLines(const QList<QVariantMap>&);
    void refreshTrains(const QList<QVariantMap>&);

private:
    QWebEngineView* m_view = nullptr;
    QWebChannel     m_channel;

    bool m_ready = false;
    QList<QVariantMap> m_lastLines;
    QList<QVariantMap> m_lastTrains;

private slots:
    void onPageReady(bool ok);
};

#endif // MAPMANAGER_H
