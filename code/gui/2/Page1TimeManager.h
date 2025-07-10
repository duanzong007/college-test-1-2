#ifndef PAGE1TIMEMANAGER_H
#define PAGE1TIMEMANAGER_H

#include <QObject>
#include <QComboBox>
#include <QPushButton>
#include <QDateTime>
#include <QMap>
#include <QSet>
#include "algo_code/flow_stationManager.h"  // 定义了 flow_stationManager 和 flow_station

/**
 * Page1 专用时间管理器
 * - 直接接收 flow_stationManager::flow_stations 容器指针
 * - 遍历其所有 QDateTime 键，构建年→月→日→小时映射
 * - 四级下拉联动；点击确认后发出 timeChanged 信号
 */
class Page1TimeManager : public QObject {
    Q_OBJECT

public:
    explicit Page1TimeManager(
        QComboBox* yearCombo,
        QComboBox* monthCombo,
        QComboBox* dayCombo,
        QComboBox* hourCombo,
        QPushButton* confirmBtn,
        QObject* parent = nullptr
        );

    /** 传入 flow_stationManager.flow_stations 的地址 */
    void importAvailableTimes(const QMap<QDateTime, QMap<int, flow_station>>* flowStations);

    /** 可选：在 importAvailableTimes 之前调用，指定初始默认选中时间 */
    void setDefaultDateTime(const QDateTime& dt);

signals:
    /** 点击“确认”后，发出所选的小时粒度时间 */
    void timeChanged(const QDateTime& datetime);

private slots:
    void onYearChanged(int);
    void onMonthChanged(int);
    void onDayChanged(int);
public slots:
    void onConfirmClicked();

private:
    void populateYears();
    void populateMonths();
    void populateDays();
    void populateHours();
    void applyDefaultSelection();

    // 年→月→日→小时 映射
    QMap<int, QMap<int, QMap<int, QSet<int>>>> m_timeMap;

    QDateTime   m_defaultDateTime;
    int         m_curYear   = 0;
    int         m_curMonth  = 0;
    int         m_curDay    = 0;

    QComboBox*   m_yearCombo;
    QComboBox*   m_monthCombo;
    QComboBox*   m_dayCombo;
    QComboBox*   m_hourCombo;
    QPushButton* m_confirmBtn;
};

#endif // PAGE1TIMEMANAGER_H
