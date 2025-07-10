#ifndef DATACARDWIDGET_H
#define DATACARDWIDGET_H

#include <QWidget>
#include "core_system.h"
#include "gui/3/c1/FilterManager.h"
#include <QFontDatabase>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <qtcharts/QCategoryAxis>
namespace Ui {
class DataCardWidget;
}

class DataCardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataCardWidget(QWidget *parent = nullptr,core_system* = nullptr);
    ~DataCardWidget();

private:
    Ui::DataCardWidget *ui;
    core_system* system;
    FilterManager filterMgr;
    void setfont();

    void chartInit();
    QLineSeries *series = nullptr;
    QChart *chart       = nullptr;
    QValueAxis *axisX   = nullptr;
    QValueAxis *axisY   = nullptr;
    void debugShow(c1_graph sig);

private slots:
    void changeGraph(c1_graph sig);


};

#endif // DATACARDWIDGET_H
