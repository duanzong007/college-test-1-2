#ifndef FILTERMANAGER_H
#define FILTERMANAGER_H

#include "core_system.h"
#include <QComboBox>  // 引入 QComboBox
#include <QObject>    // 引入 QObject 用于支持信号与槽机制
#include <QMap>

struct c1_graph{
    int style  = -1;
    int kelidu = -1;
    int id     = -1;
    int year   = -1;
    int month  = -1;
    int day    = -1;
};

class FilterManager : public QObject
{
    Q_OBJECT  // 必须添加 Q_OBJECT 宏，以支持 Qt 的信号与槽机制

public:
    FilterManager();
    void setsystem(core_system* s);
    void setComboBoxes(QComboBox* styleComboBox,
                       QComboBox* keliduComboBox,
                       QComboBox* idComboBox,
                       QComboBox* yearComboBox,
                       QComboBox* monthComboBox,
                       QComboBox* dayComboBox);

    // 一次性初始化所有的筛选器
    void initializeComboBoxes();

private:
    core_system* system = nullptr;
    data_preload* dataPreload = nullptr;

    // 保存传入的 QComboBox 指针
    QComboBox* styleComboBox = nullptr;
    QComboBox* keliduComboBox = nullptr;
    QComboBox* idComboBox = nullptr;
    QComboBox* yearComboBox = nullptr;
    QComboBox* monthComboBox = nullptr;
    QComboBox* dayComboBox = nullptr;


    QString style = 0;
    QString kelidu = 0;
    int id = 0;
    int year = 0;
    int month = 0;
    int day = 0;

    QMap<int,QString> station_name;
    QMap<QString,int> station_id;

private slots:
    void onStyleChanged(const QString& text);  // 添加槽函数，用于响应 styleComboBox 的变化
    void onKeliduChanged(const QString& text); // 添加槽函数，用于响应 keliduComboBox 的变化
    void onIdChanged(const QString& text);     // 添加槽函数，用于响应 idComboBox 的变化
    void onYearChanged(const QString& text);   // 添加槽函数，用于响应 yearComboBox 的变化
    void onMonthChanged(const QString& text);  // 添加槽函数，用于响应 monthComboBox 的变化
    void onDayChanged(const QString& text);    // 添加槽函数，用于响应 dayComboBox 的变化

signals:
    void graphChange(c1_graph sig);
};

#endif // FILTERMANAGER_H
