#ifndef TRAINWINDOW_H
#define TRAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QShowEvent>
#include <QScreen>
#include "ui_trainwindow.h"          // 由 trainwindow.ui 自动生成
#include "trainCard.h"
#include "p0_fileManager.h"
QT_BEGIN_NAMESPACE
namespace Ui { class trainWindow; }
QT_END_NAMESPACE

class trainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit trainWindow(QWidget *parent = nullptr);
    ~trainWindow() override;
    Ui::trainWindow* getUi() { return ui; }
    void addTrainCard(const TrainInfo &info);   // 单张卡片

protected:
#ifdef Q_OS_WIN
    bool nativeEvent(const QByteArray &eventType,
                     void *message,
                     qintptr *result) override;
#endif
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    Ui::trainWindow *ui;
    const int m_borderWidth    = 8;   // 缩放热区
    const int m_titleBarHeight = 35;  // 自定义标题栏高度

    void buildCardPool();         // 创建固定 10 张卡片
    void updatePage();            // 按页填入数据

    QVector<Train>      m_allTrains;   // 全量列车
    QVector<trainCard*> m_cards;       // 10 张卡片
    int  m_pageSize    = 10;
    int  m_currentPage = 0;

    void setfont();


public slots:                     // ← 新增
    void refreshTrains(const QMap<int, Train>& trains);

private slots:                    // ← 新增
    void onPrevPage();
    void onNextPage();
    void onJumpPage();
};

#endif // TRAINWINDOW_H
