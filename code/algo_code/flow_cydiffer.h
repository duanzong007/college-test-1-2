#ifndef FLOW_CYDIFFER_H
#define FLOW_CYDIFFER_H

#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QVector>
#include <QMap>
#include <QString>
#include <QDebug>

#include "flow_trainManager.h"

struct cy_flow{
    int c_to_y;
    int y_to_c;

};
struct cy_date {
    int year;
    int month;

    bool operator<(const cy_date& other) const {
        // 比较年份
        if (year != other.year) {
            return year < other.year;
        }
        // 如果年份相同，则比较月份
        return month < other.month;
    }
};


class flow_cydiffer
{
public:

    QMap<QDate, cy_flow>   flows;
    QMap<int, cy_flow>     year_flow;
    QMap<cy_date, cy_flow> month_flow;
    flow_cydiffer() {}
    void runFlowManager(const QMap<QDate, QMap<int, flow_train>> &trains);
    void qdebugShow() const;
private:
    void set_month_flow();
    void set_year_flow();



};

#endif // FLOW_CYDIFFER_H
