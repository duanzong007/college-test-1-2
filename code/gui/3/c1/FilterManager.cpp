#include "FilterManager.h"
#include <QComboBox>
#include <QDate>
#include <QMouseEvent>
#include <QComboBox>
#include <QEvent>
#include <QAbstractItemView>
FilterManager::FilterManager() {}



void FilterManager::setsystem(core_system* s)
{
    system = s;
    dataPreload = &(s->data_preload_);
    for(auto& a :system->stationManager_.stations){
        station_name.insert(a.station_id,a.station_name);
        station_id.insert(a.station_name,a.station_id);
    }

}

void FilterManager::setComboBoxes(QComboBox* styleComboBox,
                                  QComboBox* keliduComboBox,
                                  QComboBox* idComboBox,
                                  QComboBox* yearComboBox,
                                  QComboBox* monthComboBox,
                                  QComboBox* dayComboBox)
{
    // 将 QComboBox 的指针保存到成员变量
    this->styleComboBox = styleComboBox;
    this->keliduComboBox = keliduComboBox;
    this->idComboBox = idComboBox;
    this->yearComboBox = yearComboBox;
    this->monthComboBox = monthComboBox;
    this->dayComboBox = dayComboBox;

    // 连接信号与槽，更新成员变量
    connect(styleComboBox,   SIGNAL(currentTextChanged(const QString&)), this, SLOT(onStyleChanged(const QString&)));
    connect(keliduComboBox,  SIGNAL(currentTextChanged(const QString&)), this, SLOT(onKeliduChanged(const QString&)));
    connect(idComboBox,      SIGNAL(currentTextChanged(const QString&)), this, SLOT(onIdChanged(const QString&)));
    connect(yearComboBox,    SIGNAL(currentTextChanged(const QString&)), this, SLOT(onYearChanged(const QString&)));
    connect(monthComboBox,   SIGNAL(currentTextChanged(const QString&)), this, SLOT(onMonthChanged(const QString&)));
    connect(dayComboBox,     SIGNAL(currentTextChanged(const QString&)), this, SLOT(onDayChanged(const QString&)));


}

void FilterManager::initializeComboBoxes()
{
    styleComboBox->   setVisible(true);
    keliduComboBox->  setVisible(false);
    idComboBox->      setVisible(false);
    yearComboBox->    setVisible(false);
    monthComboBox->   setVisible(false);
    dayComboBox->     setVisible(false);

    styleComboBox->blockSignals(true);
    styleComboBox->clear();

    styleComboBox->addItem("请选择");
    styleComboBox->addItem("运力匹配度");
    styleComboBox->addItem("站点客流量");
    styleComboBox->addItem("站点压力值");

    styleComboBox->blockSignals(false);

}


void FilterManager::onStyleChanged(const QString& text)
{
    if(text == "请选择"){
        styleComboBox->   setVisible(true);
        keliduComboBox->  setVisible(false);
        idComboBox->      setVisible(false);
        yearComboBox->    setVisible(false);
        monthComboBox->   setVisible(false);
        dayComboBox->     setVisible(false);
    }
    else{
        keliduComboBox->  setVisible(true);
        idComboBox->      setVisible(false);
        yearComboBox->    setVisible(false);
        monthComboBox->   setVisible(false);
        dayComboBox->     setVisible(false);

        keliduComboBox->blockSignals(true);
        keliduComboBox->clear();
        keliduComboBox->addItem("请选择");
        keliduComboBox->addItem("年");
        keliduComboBox->addItem("月");
        keliduComboBox->addItem("日");
        keliduComboBox->addItem("时");
        keliduComboBox->blockSignals(false);
        style = text;
    }
}

void FilterManager::onKeliduChanged(const QString& text)
{
    if(text == "请选择"){
        keliduComboBox->  setVisible(true);
        idComboBox->      setVisible(false);
        yearComboBox->    setVisible(false);
        monthComboBox->   setVisible(false);
        dayComboBox->     setVisible(false);
    }
    else{
        idComboBox->      setVisible(true);
        yearComboBox->    setVisible(false);
        monthComboBox->   setVisible(false);
        dayComboBox->     setVisible(false);

        idComboBox->blockSignals(true);
        idComboBox->clear();
        idComboBox->addItem("请选择");
        if(style == "运力匹配度"){
            for (auto it = dataPreload->lineCapacityHourly.begin(); it != dataPreload->lineCapacityHourly.end(); ++it) {
                idComboBox->addItem(QString::number(it.key()));
            }
        }
        else if(style == "站点客流量"){
            for (auto it = dataPreload->stationFlowHourly.begin(); it != dataPreload->stationFlowHourly.end(); ++it) {
                idComboBox->addItem(station_name[it.key()]);
            }
        }
        else if(style == "站点压力值"){
            for (auto it = dataPreload->stationPressureHourly.begin(); it != dataPreload->stationPressureHourly.end(); ++it) {
                idComboBox->addItem(station_name[it.key()]);
            }
        }
        idComboBox->blockSignals(false);
        kelidu = text;
    }

}

void FilterManager::onIdChanged(const QString& text)
{
    if(text == "请选择"){
        idComboBox->      setVisible(true);
        yearComboBox->    setVisible(false);
        monthComboBox->   setVisible(false);
        dayComboBox->     setVisible(false);
    }
    else{
        if(style == "运力匹配度")id = text.toInt();
        else if(style == "站点客流量")id = station_id[text];
        else if(style == "站点压力值")id = station_id[text];

        if(kelidu == "年"){
            c1_graph sig;
            if(style == "运力匹配度")sig.style = 1;
            else if(style == "站点客流量")sig.style = 2;
            else if(style == "站点压力值")sig.style = 3;
            else sig.style = -1;
            sig.kelidu = 1;
            sig.id = id;
            sig.year = -1;
            sig.month = -1;
            sig.day = -1;
            emit graphChange(sig);
        }
        else{
            yearComboBox->    setVisible(true);
            monthComboBox->   setVisible(false);
            dayComboBox->     setVisible(false);

            yearComboBox->blockSignals(true);
            yearComboBox->clear();
            yearComboBox->addItem("");
            if(style == "运力匹配度"){
                for (auto it = dataPreload->lineCapacityHourly[id].begin(); it != dataPreload->lineCapacityHourly[id].end(); ++it) {
                    yearComboBox->addItem(QString::number(it.key()));
                }
            }
            else if(style == "站点客流量"){
                for (auto it = dataPreload->stationFlowHourly[id].begin(); it != dataPreload->stationFlowHourly[id].end(); ++it) {
                    yearComboBox->addItem(QString::number(it.key()));
                }
            }
            else if(style == "站点压力值"){
                for (auto it = dataPreload->stationPressureHourly[id].begin(); it != dataPreload->stationPressureHourly[id].end(); ++it) {
                    yearComboBox->addItem(QString::number(it.key()));
                }
            }
            yearComboBox->blockSignals(false);
        }
    }


}

void FilterManager::onYearChanged(const QString& text)
{
    if(text == ""){
        yearComboBox->    setVisible(true);
        monthComboBox->   setVisible(false);
        dayComboBox->     setVisible(false);
    }
    else{
        year = text.toInt();
        if(kelidu == "月"){
            c1_graph sig;
            if(style == "运力匹配度")sig.style = 1;
            else if(style == "站点客流量")sig.style = 2;
            else if(style == "站点压力值")sig.style = 3;
            else sig.style = -1;
            sig.kelidu = 2;
            sig.id = id;
            sig.year = year;
            sig.month = -1;
            sig.day = -1;
            emit graphChange(sig);

        }
        else{
            monthComboBox->   setVisible(true);
            dayComboBox->     setVisible(false);

            monthComboBox->blockSignals(true);
            monthComboBox->clear();
            monthComboBox->addItem("");
            if(style == "运力匹配度"){
                for (auto it = dataPreload->lineCapacityHourly[id][year].begin(); it != dataPreload->lineCapacityHourly[id][year].end(); ++it) {
                    monthComboBox->addItem(QString::number(it.key()));
                }
            }
            else if(style == "站点客流量"){
                for (auto it = dataPreload->stationFlowHourly[id][year].begin(); it != dataPreload->stationFlowHourly[id][year].end(); ++it) {
                    monthComboBox->addItem(QString::number(it.key()));
                }
            }
            else if(style == "站点压力值"){
                for (auto it = dataPreload->stationPressureHourly[id][year].begin(); it != dataPreload->stationPressureHourly[id][year].end(); ++it) {
                    monthComboBox->addItem(QString::number(it.key()));
                }
            }
            monthComboBox->blockSignals(false);
        }
    }

}

void FilterManager::onMonthChanged(const QString& text)
{
    if(text == ""){
        monthComboBox->   setVisible(true);
        dayComboBox->     setVisible(false);
    }
    else{
        month = text.toInt();
        if(kelidu == "日"){
            c1_graph sig;
            if(style == "运力匹配度")sig.style = 1;
            else if(style == "站点客流量")sig.style = 2;
            else if(style == "站点压力值")sig.style = 3;
            else sig.style = -1;
            sig.kelidu = 3;
            sig.id = id;
            sig.year = year;
            sig.month = month;
            sig.day = -1;
            emit graphChange(sig);
        }
        else{
            dayComboBox->     setVisible(true);

            dayComboBox->blockSignals(true);
            dayComboBox->clear();
            dayComboBox->addItem("");
            if(style == "运力匹配度"){
                for (auto it = dataPreload->lineCapacityHourly[id][year][month].begin(); it != dataPreload->lineCapacityHourly[id][year][month].end(); ++it) {
                    dayComboBox->addItem(QString::number(it.key()));
                }
            }
            else if(style == "站点客流量"){
                for (auto it = dataPreload->stationFlowHourly[id][year][month].begin(); it != dataPreload->stationFlowHourly[id][year][month].end(); ++it) {
                    dayComboBox->addItem(QString::number(it.key()));
                }
            }
            else if(style == "站点压力值"){
                for (auto it = dataPreload->stationPressureHourly[id][year][month].begin(); it != dataPreload->stationPressureHourly[id][year][month].end(); ++it) {
                    dayComboBox->addItem(QString::number(it.key()));
                }
            }
            dayComboBox->blockSignals(false);
        }
    }


}

void FilterManager::onDayChanged(const QString& text)
{
    if(text == ""){
         dayComboBox->     setVisible(true);
    }
    else{
        day = text.toInt();
        c1_graph sig;
        if(style == "运力匹配度")sig.style = 1;
        else if(style == "站点客流量")sig.style = 2;
        else if(style == "站点压力值")sig.style = 3;
        else sig.style = -1;
        sig.kelidu = 4;
        sig.id = id;
        sig.year = year;
        sig.month = month;
        sig.day = day;
        emit graphChange(sig);
    }

}
