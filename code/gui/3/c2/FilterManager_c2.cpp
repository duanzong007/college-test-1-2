#include "FilterManager_c2.h"
#include <QComboBox>
#include <QDate>
#include <QMouseEvent>
#include <QComboBox>
#include <QEvent>
#include <QAbstractItemView>
FilterManager_c2::FilterManager_c2() {}



void FilterManager_c2::setsystem(core_system* s)
{
    system = s;
    flowCydiffer = &(s->flow_cydiffer_);

}

void FilterManager_c2::setComboBoxes(QComboBox* keliduComboBox,
                                  QComboBox* yearComboBox,
                                  QComboBox* monthComboBox)
{
    // 将 QComboBox 的指针保存到成员变量
    this->keliduComboBox = keliduComboBox;
    this->yearComboBox = yearComboBox;
    this->monthComboBox = monthComboBox;

    // 连接信号与槽，更新成员变量
    connect(keliduComboBox,  SIGNAL(currentTextChanged(const QString&)), this, SLOT(onKeliduChanged(const QString&)));
    connect(yearComboBox,    SIGNAL(currentTextChanged(const QString&)), this, SLOT(onYearChanged(const QString&)));
    connect(monthComboBox,   SIGNAL(currentTextChanged(const QString&)), this, SLOT(onMonthChanged(const QString&)));

}

void FilterManager_c2::initializeComboBoxes()
{
    keliduComboBox->  setVisible(true);
    yearComboBox->    setVisible(false);
    monthComboBox->   setVisible(false);

    keliduComboBox->blockSignals(true);
    keliduComboBox->clear();

    keliduComboBox->addItem("请选择");
    keliduComboBox->addItem("年");
    keliduComboBox->addItem("月");
    keliduComboBox->addItem("日");

    keliduComboBox->blockSignals(false);

}


void FilterManager_c2::onKeliduChanged(const QString& text)
{
    if(text == "请选择"){
        keliduComboBox->  setVisible(true);
        yearComboBox->    setVisible(false);
        monthComboBox->   setVisible(false);
    }
    else{
        kelidu = text;
        if(kelidu == "年"){
            yearComboBox->    setVisible(false);
            monthComboBox->   setVisible(false);
            c2_graph sig;
            sig.kelidu = 1;
            sig.year   = -1;
            sig.month  = -1;
            emit graphChange(sig);
        }
        else{
            yearComboBox->    setVisible(true);
            monthComboBox->   setVisible(false);

            yearComboBox->blockSignals(true);
            yearComboBox->clear();
            yearComboBox->addItem("");

            for(auto it = flowCydiffer->year_flow.begin();it != flowCydiffer->year_flow.end();++it){
                yearComboBox->addItem(QString::number(it.key()));
            }
            yearComboBox->blockSignals(false);

        }

    }

}

void FilterManager_c2::onYearChanged(const QString& text)
{
    if(text == ""){
        yearComboBox->    setVisible(true);
        monthComboBox->   setVisible(false);
    }
    else{
        year = text.toInt();
        if(kelidu == "月"){
            c2_graph sig;
            sig.kelidu = 2;
            sig.year   = year;
            sig.month  = -1;
            emit graphChange(sig);

            monthComboBox->   setVisible(false);
        }
        else{
            monthComboBox->   setVisible(true);

            monthComboBox->blockSignals(true);
            monthComboBox->clear();
            monthComboBox->addItem("");

            for(auto it = flowCydiffer->month_flow.begin();it != flowCydiffer->month_flow.end();++it){
                if(it.key().year == year){
                    monthComboBox->addItem(QString::number(it.key().month));
                }
            }

            monthComboBox->blockSignals(false);
        }
    }

}

void FilterManager_c2::onMonthChanged(const QString& text)
{
    if(text == ""){
        monthComboBox->   setVisible(true);

    }
    else{
        month = text.toInt();
        c2_graph sig;
        sig.kelidu = 3;
        sig.year   = year;
        sig.month  = month;
        emit graphChange(sig);
    }
}
