#ifndef LINECARD_H
#define LINECARD_H

#include <QWidget>
#include <QColor>
#include <QFontDatabase>
QT_BEGIN_NAMESPACE
namespace Ui { class lineCard; }
QT_END_NAMESPACE

/** 线路展示所需全部字段 */
struct LineInfo
{
    int     id              = -1;        // 线路 ID（唯一键）
    double  matchPercent    = -1.0;        // 运力匹配度；-1 = 暂无数据
    QString startStation;                // 始发
    QString endStation;                  // 终点
    QColor  color           = Qt::gray;  // 主色：圆 + 线 + 勾选框
    bool    visible         = true;      // 初始是否勾选（来自你的 QMap）

    LineInfo(int _id                    = -1,
             double _matchPercent       = -1.0,
             QString _startStation      = QString(),
             QString _endStation        = QString(),
             const QColor &_color       = Qt::gray,
             bool _visible              = true)
        : id(_id)
        , matchPercent(_matchPercent)
        , startStation(std::move(_startStation))
        , endStation(std::move(_endStation))
        , color(_color)
        , visible(_visible)
    {}

};

class lineCard : public QWidget
{
    Q_OBJECT
public:
    explicit lineCard(QWidget *parent = nullptr);
    ~lineCard() override;

    /* ===== 外部接口 ===== */
    void setLineInfo(const LineInfo &info);   // 初始化 / 刷新整卡
    void setColor(const QColor &c);           // 单独改颜色
    void setChecked(bool on);                 // 单独设勾选状态
    int  lineId() const { return m_id; }      // 取 ID

signals:
    /** 复选框被用户勾/取消时发射：id, checked */
    void visibilityChanged(int id, bool checked);

private:
    Ui::lineCard *ui = nullptr;
    int  m_id        = -1;

    void applyColor(const QColor &c);         // 内部统一上色
    void setfont();
};

#endif // LINECARD_H
