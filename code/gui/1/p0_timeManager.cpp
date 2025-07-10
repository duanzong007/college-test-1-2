#include "p0_timeManager.h"
#include <algorithm>
#include <QDebug>

p0_timeManager::p0_timeManager(
    QComboBox*   yearCombo,
    QComboBox*   monthCombo,
    QComboBox*   dayCombo,
    QComboBox*   hourCombo,
    QPushButton* confirmBtn,
    QObject*     parent
    ) : QObject(parent)
    , m_yearCombo(yearCombo)
    , m_monthCombo(monthCombo)
    , m_dayCombo(dayCombo)
    , m_hourCombo(hourCombo)
    , m_confirmBtn(confirmBtn)
    , m_defaultDateTime()   // invalid by default
{
    connect(m_yearCombo,  QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &p0_timeManager::onYearChanged);
    connect(m_monthCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &p0_timeManager::onMonthChanged);
    connect(m_dayCombo,   QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &p0_timeManager::onDayChanged);


    connect(m_confirmBtn, &QPushButton::clicked,
            this, &p0_timeManager::onConfirmClicked);
}

void p0_timeManager::importTimes(const QVector<QDateTime>& times) {
    Q_UNUSED(times);
    // 兼容接口，目前不处理
}

void p0_timeManager::setDefaultDateTime(const QDateTime& dt) {
    m_defaultDateTime = dt;
}

void p0_timeManager::importFlowTrains(
    const QMap<QDate, QMap<int, flow_train>>& flowTrains
    ) {
    m_timeMap.clear();

    // 构建年→月→日→小时 嵌套 Map
    for (auto itDate = flowTrains.constBegin(); itDate != flowTrains.constEnd(); ++itDate) {

        const QDate& date = itDate.key();
        if (!date.isValid()) continue;      // 跳过无效日期
        int year  = date.year();
        int month = date.month();
        int day   = date.day();

        auto& hourSet = m_timeMap[year][month][day];
        const auto& trainMap = itDate.value();
        for (auto itTrain = trainMap.constBegin(); itTrain != trainMap.constEnd(); ++itTrain) {
            const flow_train& ft = itTrain.value();
            for (const auto& st : ft.stations) {
                int h = st.time.hour();
                if (h > 0 && h < 24)
                    hourSet.insert(h);
            }
        }
    }

    // 填充年份（第一级）
    populateYears();

    // 如果设置了默认时间，则选中并发出一次，否则默认最小值
    applyDefaultSelection();

}

void p0_timeManager::populateYears() {
    m_yearCombo->clear();
    for (int year : m_timeMap.keys()) {
        if (year > 0)
            m_yearCombo->addItem(QString::number(year));
    }
    if (!m_timeMap.isEmpty()) {
        // 默认选中最小年份（index 0），改为最新请改为 count()-1
        m_yearCombo->setCurrentIndex(0);
        onYearChanged(0);
    }
}

void p0_timeManager::onYearChanged(int) {
    m_curYear = m_yearCombo->currentText().toInt();
    populateMonths();
}

void p0_timeManager::populateMonths() {
    m_monthCombo->clear();
    const auto& monthsMap = m_timeMap.value(m_curYear);
    for (int month : monthsMap.keys()) {
        if (month > 0)
            m_monthCombo->addItem(QString::number(month));
    }
    if (!monthsMap.isEmpty()) {
        m_monthCombo->setCurrentIndex(0);  // 默认最小月份
        onMonthChanged(0);
    }
}

void p0_timeManager::onMonthChanged(int) {
    m_curMonth = m_monthCombo->currentText().toInt();
    populateDays();
}

void p0_timeManager::populateDays() {
    m_dayCombo->clear();
    const auto& daysMap = m_timeMap[m_curYear][m_curMonth];
    for (int day : daysMap.keys()) {
        if (day > 0)
            m_dayCombo->addItem(QString::number(day));
    }
    if (!daysMap.isEmpty()) {
        m_dayCombo->setCurrentIndex(0);  // 默认最小日期
        onDayChanged(0);
    }
}

void p0_timeManager::onDayChanged(int) {
    m_curDay = m_dayCombo->currentText().toInt();
    populateHours();
}

void p0_timeManager::populateHours() {
    m_hourCombo->clear();
    const auto& hoursSet = m_timeMap[m_curYear][m_curMonth][m_curDay];
    QList<int> hours = hoursSet.values();
    std::sort(hours.begin(), hours.end());  // 从小到大
    for (int h : hours) {
        if (h > 0)
            m_hourCombo->addItem(QString::number(h));
    }
    if (!hours.isEmpty()) {
        m_hourCombo->setCurrentIndex(0);  // 默认最小小时
    }
}

void p0_timeManager::onConfirmClicked() {
    QDate date(m_curYear, m_curMonth, m_curDay);
    int h = m_hourCombo->currentText().toInt();
    QTime time(h, 0);
    emit timeSelected(QDateTime(date, time));
}

void p0_timeManager::applyDefaultSelection() {
    if (m_defaultDateTime.isValid()) {
        // 自动在各级 ComboBox 中选中默认值
        int yi = m_yearCombo->findText(QString::number(m_defaultDateTime.date().year()));
        if (yi >= 0) {
            m_yearCombo->setCurrentIndex(yi);
            int Mi = m_monthCombo->findText(QString::number(m_defaultDateTime.date().month()));
            if (Mi >= 0) m_monthCombo->setCurrentIndex(Mi);
            int di = m_dayCombo->findText(QString::number(m_defaultDateTime.date().day()));
            if (di >= 0) m_dayCombo->setCurrentIndex(di);
            int hi = m_hourCombo->findText(QString::number(m_defaultDateTime.time().hour()));
            if (hi >= 0) m_hourCombo->setCurrentIndex(hi);
        }
    }

}
