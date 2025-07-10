#include "page1_main.h"
#include "ui_page1_main.h"
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QSet>


page1_main::page1_main(QWidget *parent, core_system* s)
    : QWidget(parent)
    , ui(new Ui::page1_main)
    , system(s)
{
    ui->setupUi(this);

    // 为每个控件创建独立的阴影效果
    QGraphicsDropShadowEffect *shadowEffect1 = new QGraphicsDropShadowEffect(this);
    shadowEffect1->setColor(QColor(0, 0, 0, 30));  // 黑色，透明度160（0-255）
    shadowEffect1->setOffset(2, 2);  // 水平和垂直偏移量
    shadowEffect1->setBlurRadius(2);  // 模糊半径

    // 应用到不同控件
    ui->time->setGraphicsEffect(shadowEffect1);


    timeMgr = new Page1TimeManager(
        ui->year, ui->month, ui->day, ui->hour, ui->checkbutton, this
        );
    timeMgr->setDefaultDateTime({ QDate(2015, 10, 2), QTime(17, 0) });
    timeMgr->importAvailableTimes(&(system->flow_stationManager_.flow_stations));
    connect(timeMgr, &Page1TimeManager::timeChanged,
            this,    &page1_main::onUserPickedTime);



    p1_fileManager* fileMgr = new p1_fileManager(
        &(system->flow_stationManager_),
        &(system->ipManager_),
        this
        );

    // 连接信号
    connect(timeMgr, &Page1TimeManager::timeChanged,
            fileMgr, &p1_fileManager::onTimeChanged);

    webView = new QWebEngineView(ui->map);                // ui->map 为你的容器 QWidget
    ui->layout->addWidget(webView);
    mapMgr = new p1_mapManager(webView, this);

    // 4. 信号连线到地图
    connect(fileMgr, &p1_fileManager::sendFieldData,
            mapMgr,   &p1_mapManager::s_refreshHeatmap);
    connect(fileMgr, &p1_fileManager::sendFieldData,
            mapMgr,   &p1_mapManager::s_refreshStations);


    setfont();
    timeMgr->onConfirmClicked();
}

page1_main::~page1_main()
{
    delete ui;
}


void page1_main::setfont(){
    int font1 = QFontDatabase::addApplicationFont(":/font/syHei-Regular.otf");
    if(font1==-1){
        qDebug()<<"syHei-Regular.otf加载失败！";
    }
    else{
        QString syHei_Regular = QFontDatabase::applicationFontFamilies(font1).at(0);
        ui->year->setFont        (QFont(syHei_Regular,13));
        ui->month->setFont       (QFont(syHei_Regular,13));
        ui->day->setFont         (QFont(syHei_Regular,13));
        ui->hour->setFont        (QFont(syHei_Regular,13));
        ui->checkbutton->setFont (QFont(syHei_Regular,13));
    }

}



void page1_main::onUserPickedTime(const QDateTime& dt) {
    qDebug() << "用户选择的时间：" << dt.toString(Qt::ISODate);

}
