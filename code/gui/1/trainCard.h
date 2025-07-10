#ifndef TRAINCARD_H
#define TRAINCARD_H

#include <QWidget>
#include <QFontDatabase>
namespace Ui {
class trainCard;
}

struct TrainInfo
{
    QString code;           // 车次号
    int trainId = -1;       // 列车ID
    int people = 0;         // 当前人数
    int capacity = 0;       // 额定载荷
    double loadRate = 0.0;  // 列车满载率
    int lineId = -1;        // 所属线路
    QString startStation;   // 始发站
    QString endStation;     // 终点站

    TrainInfo(QString _code = {},
              int _id = -1,
              int _people = 0,
              int _capacity = 0,
              double _rate = 0.0,
              int _lineId = -1,
              QString _startStation = {},
              QString _endStation = {})
        : code(std::move(_code))
        , trainId(_id)
        , people(_people)
        , capacity(_capacity)
        , loadRate(_rate)
        , lineId(_lineId)
        , startStation(std::move(_startStation))
        , endStation(std::move(_endStation))
    {}
};



class trainCard : public QWidget
{
    Q_OBJECT

public:
    explicit trainCard(QWidget *parent = nullptr);
    void setTrainInfo(const TrainInfo &info);

    ~trainCard();

private:
    Ui::trainCard *ui;
    void setfont();
};

#endif // TRAINCARD_H
