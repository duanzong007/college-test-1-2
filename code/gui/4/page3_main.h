#ifndef PAGE3_MAIN_H
#define PAGE3_MAIN_H

#include <QWidget>
#include"core_system.h"
#include <QFontDatabase>
#include<QStackedWidget>
#include<QTableWidget>
#include<QEvent>
#include<QList>
#include"dis_page.h"
#include <QTableView>
#include<QPushButton>
#include<Qvector>
#include <QStandardItemModel>
#include"chart_change.h"
#include"tableedittracker.h"
#include <QGraphicsDropShadowEffect>
#include "logwindow.h"
namespace Ui {
class page3_main;
}

class page3_main : public QWidget
{
    Q_OBJECT

public:
    explicit page3_main(QWidget *parent = nullptr, core_system *sys = nullptr);//初始化ordermanager
    ~page3_main();
    bool eventFilter(QObject *watched, QEvent *event) override;
    void setSelected(int index);

    void initOrderTable();// 添加表格初始化方法
    void initTrainTable();// 添加表格初始化方法
    void initStationTable();// 添加表格初始化方法
    void initLineTable();// 添加表格初始化方法

    void loadOrderData(core_system *system);// 添加数据加载方法
    void loadTrainData(core_system *system);// 添加数据加载方法
    void loadStationData(core_system *system);// 添加数据加载方法
    void loadLineData(core_system *system);// 添加数据加载方法

    void closeSubWindows();

    void updatePageInfo();

    void setbottomfont(const QFont &font);
    void setTableFont(const QFont &font);
    void setHeaderFont(const QFont &font);

    //分页按钮
    QPushButton *order_pre;
    QPushButton* order_next;

    QPushButton *station_pre;
    QPushButton* station_next;

    QPushButton *train_pre;
    QPushButton* train_next;

    QPushButton *line_pre;
    QPushButton* line_next;

    chart_change* m_editController;

private slots:
    void onPageChanged(int page);
    void ontrainChanged(int page);
    void onlineChanged(int page);
    void onstationChanged(int page);

private slots:
    void showLogWindow();

private:
    Ui::page3_main *ui;
    QList<QWidget*> m_titles;  // 存储所有标题widget
    QStackedWidget* m_stackedWidget; // 下方的页面容器
    void setFonts();  // 声明字体设置方法
    void applyStyle(QWidget *w, bool selected);
    core_system *m_system; // 添加core_system指针成员
    bool m_ordersLoaded = false;//是否加载
    bool m_trainsLoaded = false;//是否加载
    bool m_stationsLoaded = false;//是否加载
    bool m_linesLoaded = false;//是否加载

    QTableWidget *orderTable = nullptr;
    QTableWidget *trainTable = nullptr;
    QTableWidget *lineTable = nullptr;//线路
    QTableWidget *stationTable = nullptr;//站点

    QWidget *orderPage=nullptr;//订单表格
    QWidget *trainPage=nullptr;//列车表格
    QWidget *linePage=nullptr;//线路表格
    QWidget *stationPage=nullptr;//站点表格

    dis_page*m_dispage;
    dis_page*m_trainpage;
    dis_page*m_stationpage;
    dis_page*m_linepage;
    logWindow* log = nullptr;

    TableEditTracker* m_tableEditTracker;

    void setshadow();

};

#endif // PAGE3_MAIN_H
