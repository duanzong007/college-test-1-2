#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <QObject>
#include <QVector>
#include <QDate>
#include <QDateTime>
#include "core_system.h"
#include <QMap>

class card3;
class QComboBox;
class QDateEdit;
class QLabel;
class QPushButton;
class QTableView;

/*  稀疏数据友好版预测器  */
class predictor : public QObject
{
    Q_OBJECT
public:
    predictor(card3* uiCard, core_system* system);

private slots:
    void onStationChanged(int);
    void onStartDateChanged(const QDate &);
    void onHorizonChanged(int);
    void onConfirmClicked();

private:
    /* ---------- 指针 ---------- */
    card3*       uiCard_;
    core_system* sys_;
    QComboBox*   stationCB_;
    QComboBox*   hourCB_;
    QDateEdit*   startDE_;      //!< 只选日期
    QLabel*      aimLB_;
    QPushButton* confirmBtn_;
    QTableView*  tableView_;

    /* ---------- 运行时数据 ---------- */
    int    clampMax_ = 50000;   //!< 离群上限 (median×5)
    double boardRatio_ = 0.5;   //!< 上下客拆分比例

    /* ---------- 工具函数 ---------- */
    void initUI();
    void refreshAimTime();
    bool validateInputs(QString& err);

    /* ---------- 核心流程 ---------- */
    void   runForecast();
    void   buildFullSeries(int sid,
                         QVector<int>& totHist,
                         QDateTime& first,
                         QDateTime& last);
    QVector<int> forecastSeriesSmart(const QVector<int>& hist,
                                     int horizonHours);

    void   fillTable(const QVector<QDateTime>& ts,
                   const QVector<int>& bd,
                   const QVector<int>& al);


};

#endif // PREDICTOR_H
