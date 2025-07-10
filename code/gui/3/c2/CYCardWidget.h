#ifndef CYCARDWIDGET_H
#define CYCARDWIDGET_H


#include <QWidget>
#include <QFontDatabase>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>



#include "core_system.h"
#include "gui/3/c2/FilterManager_c2.h"

namespace Ui {
class CYCardWidget;
}

class CYCardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CYCardWidget(QWidget *parent = nullptr, core_system* s = nullptr);
    ~CYCardWidget();

private:
    Ui::CYCardWidget *ui;
    core_system* system;
    FilterManager_c2 filterMgr;
    void setfont();
    void chartInit();
    QBarSet          *setA   = nullptr;    // 第一个数据集
    QBarSet          *setB   = nullptr;    // 第二个数据集
    QBarSeries       *series = nullptr;   // 分组柱状序列
    QChart           *chart  = nullptr;
    QBarCategoryAxis *axisX = nullptr;
    QValueAxis       *axisY  = nullptr;
    void debugShow(c2_graph sig);

    QVector<QGraphicsSimpleTextItem*> m_barLabels;

private slots:
    void changeGraph(c2_graph sig);

};

#endif // CYCARDWIDGET_H
