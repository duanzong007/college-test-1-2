#include "Page1TimeManager.h"
#include <algorithm>

Page1TimeManager::Page1TimeManager(
    QComboBox* yearCombo,
    QComboBox* monthCombo,
    QComboBox* dayCombo,
    QComboBox* hourCombo,
    QPushButton* confirmBtn,
    QObject* parent
    ) : QObject(parent),
    m_yearCombo(yearCombo),
    m_monthCombo(monthCombo),
    m_dayCombo(dayCombo),
    m_hourCombo(hourCombo),
    m_confirmBtn(confirmBtn)
{
    connect(m_yearCombo,  QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Page1TimeManager::onYearChanged);
    connect(m_monthCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Page1TimeManager::onMonthChanged);
    connect(m_dayCombo,   QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Page1TimeManager::onDayChanged);
    connect(m_confirmBtn, &QPushButton::clicked,
            this, &Page1TimeManager::onConfirmClicked);
}

void Page1TimeManager::importAvailableTimes(
    const QMap<QDateTime, QMap<int, flow_station>>* flowStations
    ) {
    m_timeMap.clear();
    // 使用 const_iterator 遍历，不生成临时 keys 列表
    for (auto it = flowStations->constBegin(); it != flowStations->constEnd(); ++it) {
        const QDateTime dt = it.key();
        const QDate d = dt.date();
        m_timeMap[d.year()][d.month()][d.day()].insert(dt.time().hour());
    }
    populateYears();
    applyDefaultSelection();
}

void Page1TimeManager::setDefaultDateTime(const QDateTime& dt)
{
    m_defaultDateTime = dt;
}

void Page1TimeManager::populateYears()
{
    m_yearCombo->clear();
    QList<int> years = m_timeMap.keys();
    std::sort(years.begin(), years.end());
    for (int y : years) {
        m_yearCombo->addItem(QString::number(y));
    }
    if (!years.isEmpty()) {
        m_yearCombo->setCurrentIndex(0);
        onYearChanged(0);
    }
}

// ----------- Page1TimeManager.cpp -----------

void Page1TimeManager::populateMonths()
{
    m_monthCombo->clear();
    const auto monthsMap = m_timeMap.value(m_curYear);
    QList<int> months = monthsMap.keys();
    std::sort(months.begin(), months.end());
    for (int M : months) {
        if (M <= 0) continue;                    // 屏蔽零
        m_monthCombo->addItem(QString::number(M));
    }
    if (m_monthCombo->count() > 0) {
        m_monthCombo->setCurrentIndex(0);
        onMonthChanged(0);                       // 级联刷新：天 → 时
    } else {
        m_dayCombo->clear();
        m_hourCombo->clear();
    }
}

void Page1TimeManager::populateDays()
{
    m_dayCombo->clear();
    const auto daysMap = m_timeMap[m_curYear][m_curMonth];
    QList<int> days = daysMap.keys();
    std::sort(days.begin(), days.end());
    for (int d : days) {
        if (d <= 0) continue;                    // 屏蔽零
        m_dayCombo->addItem(QString::number(d));
    }
    if (m_dayCombo->count() > 0) {
        m_dayCombo->setCurrentIndex(0);
        onDayChanged(0);                         // 级联刷新：时
    } else {
        m_hourCombo->clear();
    }
}

void Page1TimeManager::onYearChanged(int)
{
    m_curYear = m_yearCombo->currentText().toInt();
    populateMonths();
    // 万一上面没有调用 onDayChanged，也再保证一次小时刷新：
    if (m_dayCombo->count() > 0)
        onDayChanged(m_dayCombo->currentIndex());
}

void Page1TimeManager::onMonthChanged(int)
{
    m_curMonth = m_monthCombo->currentText().toInt();
    populateDays();
    // 同理，再次保证小时刷新：
    if (m_dayCombo->count() > 0)
        onDayChanged(m_dayCombo->currentIndex());
}


void Page1TimeManager::onDayChanged(int)
{
    m_curDay = m_dayCombo->currentText().toInt();
    populateHours();
}

void Page1TimeManager::populateHours()
{
    m_hourCombo->clear();
    QList<int> hours = m_timeMap[m_curYear][m_curMonth][m_curDay].values();
    std::sort(hours.begin(), hours.end());
    for (int h : hours) {
        m_hourCombo->addItem(QString::number(h));
    }
    if (!hours.isEmpty()) {
        m_hourCombo->setCurrentIndex(0);
    }
}

void Page1TimeManager::onConfirmClicked()
{
    QDate date(m_curYear, m_curMonth, m_curDay);
    int h = m_hourCombo->currentText().toInt();
    emit timeChanged(QDateTime(date, QTime(h, 0)));
}

void Page1TimeManager::applyDefaultSelection()
{
    if (m_defaultDateTime.isValid()) {
        int y = m_defaultDateTime.date().year();
        int M = m_defaultDateTime.date().month();
        int d = m_defaultDateTime.date().day();
        int h = m_defaultDateTime.time().hour();
        int yi = m_yearCombo->findText(QString::number(y));
        if (yi >= 0) m_yearCombo->setCurrentIndex(yi);
        int Mi = m_monthCombo->findText(QString::number(M));
        if (Mi >= 0) m_monthCombo->setCurrentIndex(Mi);
        int di = m_dayCombo->findText(QString::number(d));
        if (di >= 0) m_dayCombo->setCurrentIndex(di);
        int hi = m_hourCombo->findText(QString::number(h));
        if (hi >= 0) m_hourCombo->setCurrentIndex(hi);
    }
}
