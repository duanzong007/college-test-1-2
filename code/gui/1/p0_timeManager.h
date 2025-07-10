#ifndef P0_TIMEMANAGER_H
#define P0_TIMEMANAGER_H

#include <QObject>
#include <QComboBox>
#include <QPushButton>
#include <QDateTime>
#include <QVector>
#include <QMap>
#include <QSet>
#include "algo_code/flow_trainManager.h"    // for flow_train

class p0_timeManager : public QObject {
    Q_OBJECT
public:
    /**
     * @param yearCombo   年份下拉框
     * @param monthCombo  月份下拉框
     * @param dayCombo    日下拉框
     * @param hourCombo   小时下拉框
     * @param confirmBtn  “确认”按钮
     * @param parent      父 QObject
     */
    explicit p0_timeManager(
        QComboBox*   yearCombo,
        QComboBox*   monthCombo,
        QComboBox*   dayCombo,
        QComboBox*   hourCombo,
        QPushButton* confirmBtn,
        QObject*     parent = nullptr
        );

    /**（兼容接口）直接导入 QDateTime 列表，当前不使用 */
    void importTimes(const QVector<QDateTime>& times);

    /** 导入 flow_trains，内部构建年→月→日→小时的嵌套 Map */
    void importFlowTrains(const QMap<QDate, QMap<int, flow_train>>& flowTrains);

    /** 设置程序启动时要默认选中的时间，必须在 importFlowTrains 之前调用 */
    void setDefaultDateTime(const QDateTime& dt);

signals:
    /** 用户点击或默认确认后，发送所选的完整 QDateTime */
    void timeSelected(const QDateTime& datetime);

private slots:
    void onYearChanged(int index);
    void onMonthChanged(int index);
    void onDayChanged(int index);
public slots:
    void onConfirmClicked();

private:
    void populateYears();
    void populateMonths();
    void populateDays();
    void populateHours();
    void applyDefaultSelection();

    // —— 内部存储：年→月→日→小时 四层嵌套 Map ——
    QMap<int, QMap<int, QMap<int, QSet<int>>>> m_timeMap;

    // 默认启动时的时间
    QDateTime m_defaultDateTime;

    // 当前级联选择值
    int m_curYear  = 0;
    int m_curMonth = 0;
    int m_curDay   = 0;

    // UI 控件
    QComboBox*   m_yearCombo;
    QComboBox*   m_monthCombo;
    QComboBox*   m_dayCombo;
    QComboBox*   m_hourCombo;
    QPushButton* m_confirmBtn;
};

#endif // P0_TIMEMANAGER_H
