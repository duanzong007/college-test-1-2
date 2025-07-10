#include "page0_main.h"
#include "ui_page0_main.h"
#include <QDebug>
#include <QGraphicsDropShadowEffect>
page0_main::page0_main(QWidget *parent, core_system* s)
    : QWidget(parent)
    , ui(new Ui::page0_main)
    , system(s)
{
    ui->setupUi(this);

    // 为每个控件创建独立的阴影效果
    QGraphicsDropShadowEffect *shadowEffect1 = new QGraphicsDropShadowEffect(this);
    shadowEffect1->setColor(QColor(0, 0, 0, 30));  // 黑色，透明度160（0-255）
    shadowEffect1->setOffset(2, 2);  // 水平和垂直偏移量
    shadowEffect1->setBlurRadius(2);  // 模糊半径

    QGraphicsDropShadowEffect *shadowEffect2 = new QGraphicsDropShadowEffect(this);
    shadowEffect2->setColor(QColor(0, 0, 0, 30));  // 黑色，透明度160（0-255）
    shadowEffect2->setOffset(2, 2);  // 水平和垂直偏移量
    shadowEffect2->setBlurRadius(2);  // 模糊半径

    QGraphicsDropShadowEffect *shadowEffect3 = new QGraphicsDropShadowEffect(this);
    shadowEffect3->setColor(QColor(0, 0, 0, 30));  // 黑色，透明度160（0-255）
    shadowEffect3->setOffset(2, 2);  // 水平和垂直偏移量
    shadowEffect3->setBlurRadius(2);  // 模糊半径

    // 应用到不同控件
    ui->time->setGraphicsEffect(shadowEffect1);
    ui->linebutton->setGraphicsEffect(shadowEffect2);
    ui->trainbutton->setGraphicsEffect(shadowEffect3);



    // 设置timemanager
    timeMgr = new p0_timeManager(
        ui->year,
        ui->month,
        ui->day,
        ui->hour,
        ui->checkbutton,
        this
        );
    timeMgr->setDefaultDateTime({ QDate(2015, 10, 1), QTime(12, 0) });
    // 2) 导入数据
    timeMgr->importFlowTrains(system->flow_trainManager_.flow_trains);

    // 3) 连接 timeSelected 信号到一个槽函数，拿到用户选中的 QDateTime
    connect(timeMgr, &p0_timeManager::timeSelected,
            this,    &page0_main::onUserPickedTime);


    //设置filemanager
    fileMgr = new p0_fileManager(this);

    fileMgr->set_system(system);
    fileMgr->preloadLinesData();
    connect(timeMgr, &p0_timeManager::timeSelected,
            fileMgr,  &p0_fileManager::onTimeChanged);


    // 2. 把 linebutton 的 clicked 信号连接到 MainWindow 的 show() 槽
    connect(ui->linebutton, &QPushButton::clicked,
            this, &page0_main::showLineWindow);
    connect(ui->trainbutton, &QPushButton::clicked,
            this, &page0_main::showTrainWindow);



    line = new lineWindow(fileMgr);
    line->setAttribute(Qt::WA_DeleteOnClose);

    connect(fileMgr, &p0_fileManager::sendLinesToLineWindow,
            line,     &lineWindow::refreshLines);
    train = new trainWindow();
    train->setAttribute(Qt::WA_DeleteOnClose);

    connect(fileMgr, &p0_fileManager::sendTrainsToTrainWindow,
            train,    &trainWindow::refreshTrains,
            Qt::UniqueConnection);


    // 创建 Web 引擎视图实例
    webView = new QWebEngineView(this);

    ui->layout->addWidget(webView);

    mapMgr = new mapManager(webView, this);
    connect(fileMgr, &p0_fileManager::sendLinesToMap,
            mapMgr,   &mapManager::s_refreshLines, Qt::UniqueConnection);
    connect(fileMgr, &p0_fileManager::sendTrainsToMap,
            mapMgr,   &mapManager::s_refreshTrains, Qt::UniqueConnection);

    setfont();
    timeMgr->onConfirmClicked();
    connect(line,&lineWindow::cancalChoose,fileMgr,&p0_fileManager::cancelChoose);
}

page0_main::~page0_main()
{
    delete ui;
}


void page0_main::setfont(){
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
        ui->trainbutton->setFont (QFont(syHei_Regular,15));
        ui->linebutton->setFont  (QFont(syHei_Regular,15));
        ui->checkbutton->setFont (QFont(syHei_Regular,13));

    }


}


void page0_main::onUserPickedTime(const QDateTime& dt) {
    // dt 就是用户选好的年/月/日/时，你可以用它去调用 DataManager 或 flowMgr
    qDebug() << "用户选择的时间：" << dt.toString(Qt::ISODate);
    // 例如：
    // auto result = flowMgr.queryAt(dt);
    // mapCtrl->showData(result);
}

void page0_main::showLineWindow()
{
    if (!line) {
        line = new lineWindow(fileMgr);
        line->setAttribute(Qt::WA_DeleteOnClose);

        connect(fileMgr, &p0_fileManager::sendLinesToLineWindow,
                line,     &lineWindow::refreshLines);

    }
    if (line->isMinimized())line->showNormal();
    line->show();
    line->raise();
    line->activateWindow();
}


void page0_main::showTrainWindow()
{
    if (!train) {
        train = new trainWindow();
        train->setAttribute(Qt::WA_DeleteOnClose);

        connect(fileMgr, &p0_fileManager::sendTrainsToTrainWindow,
                train,    &trainWindow::refreshTrains,
                Qt::UniqueConnection);

    }

    if (train->isMinimized())train->showNormal();
    train->show();
    train->raise();
    train->activateWindow();
}

void page0_main::closeSubWindows()
{
    if (line)  {
        line->close();
    }
    if (train) {
        train->close();
    }
}



