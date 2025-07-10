#ifndef LINEWINDOW_H
#define LINEWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QShowEvent>
#include <QScreen>
#include "ui_linewindow.h"
#include "lineCard.h"
#include "p0_fileManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class lineWindow; }
QT_END_NAMESPACE

class lineWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit lineWindow(p0_fileManager* mgr, QWidget *parent = nullptr);
    ~lineWindow() override;
    Ui::lineWindow* getUi() { return ui; }
    void addLineCard(const LineInfo &info); // 供 page0 调用

public slots:
    void refreshLines(const QMap<int, Line>& lines);

private slots:
    void onPrevPage();       // ← 新增
    void onNextPage();       // ← 新增
    void onJumpPage();       // ← 新增（回车跳页）

signals:
    void cancalChoose();


protected:
#ifdef Q_OS_WIN
    bool nativeEvent(const QByteArray &eventType,
                     void *message,
                     qintptr *result) override;
#endif
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void showEvent(QShowEvent *event) override;           // ★ 新增：首次居中

private:
    Ui::lineWindow *ui;
    const int m_borderWidth    = 8;   // 缩放热区
    const int m_titleBarHeight = 35;  // 自定义标题栏高度
    p0_fileManager* m_fileMgr = nullptr;


    void buildCardPool();    // 创建 10 张固定空卡片
    void updatePage();       // 根据 m_currentPage 把数据写进卡片

    QVector<Line>       m_allLines;    // 全量线路数据
    QVector<lineCard*>  m_cards;       // 10 张卡片指针
    int  m_pageSize    = 10;
    int  m_currentPage = 0;

    void setfont();
};

#endif // LINEWINDOW_H
