#ifndef FILTERMANAGER_C2_H
#define FILTERMANAGER_C2_H

#include "core_system.h"
#include <QComboBox>  // 引入 QComboBox
#include <QObject>    // 引入 QObject 用于支持信号与槽机制
#include <QMap>

struct c2_graph{
    int kelidu = -1;
    int year   = -1;
    int month  = -1;
};

class FilterManager_c2 : public QObject
{
    Q_OBJECT  // 必须添加 Q_OBJECT 宏，以支持 Qt 的信号与槽机制

public:
    FilterManager_c2();
    void setsystem(core_system* s);
    void setComboBoxes(QComboBox* keliduComboBox,
                       QComboBox* yearComboBox,
                       QComboBox* monthComboBox);

    // 一次性初始化所有的筛选器
    void initializeComboBoxes();

private:
    core_system* system = nullptr;
    flow_cydiffer* flowCydiffer = nullptr;

    // 保存传入的 QComboBox 指针
    QComboBox* keliduComboBox = nullptr;
    QComboBox* yearComboBox = nullptr;
    QComboBox* monthComboBox = nullptr;

    QString kelidu = 0;
    int year = 0;
    int month = 0;

private slots:
    void onKeliduChanged(const QString& text); // 添加槽函数，用于响应 keliduComboBox 的变化
    void onYearChanged(const QString& text);   // 添加槽函数，用于响应 yearComboBox 的变化
    void onMonthChanged(const QString& text);  // 添加槽函数，用于响应 monthComboBox 的变化
signals:
    void graphChange(c2_graph sig);
};

#endif // FILTERMANAGER_C2_H
