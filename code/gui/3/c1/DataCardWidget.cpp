#include "DataCardWidget.h"
#include "ui_DataCardWidget.h"
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSimpleTextItem>
DataCardWidget::DataCardWidget(QWidget *parent, core_system* s)
    : QWidget(parent)
    , ui(new Ui::DataCardWidget)
    , system(s)
{
    ui->setupUi(this);

    QGraphicsDropShadowEffect *shadowEffect1 = new QGraphicsDropShadowEffect(this);
    shadowEffect1->setColor(QColor(0, 0, 0, 30));  // 黑色，透明度160（0-255）
    shadowEffect1->setOffset(2, 2);  // 水平和垂直偏移量
    shadowEffect1->setBlurRadius(2);  // 模糊半径
    ui->check->setGraphicsEffect(shadowEffect1);

    QGraphicsDropShadowEffect *shadowEffect2 = new QGraphicsDropShadowEffect(this);
    shadowEffect2->setColor(QColor(0, 0, 0, 40));  // 黑色，透明度160（0-255）
    shadowEffect2->setOffset(3, 3);  // 水平和垂直偏移量
    shadowEffect2->setBlurRadius(4);  // 模糊半径
    ui->graphbb->setGraphicsEffect(shadowEffect2);

    filterMgr.setsystem(s);

    // 传递所有的筛选器指针给 FilterManager
    filterMgr.setComboBoxes(ui->style, ui->kelidu, ui->id, ui->year, ui->month, ui->day);

    // 一次性初始化所有的筛选器
    filterMgr.initializeComboBoxes();

    connect(&filterMgr, &FilterManager::graphChange, this, &DataCardWidget::changeGraph);
    setfont();
    chartInit();
}

DataCardWidget::~DataCardWidget()
{
    delete ui;
}

void DataCardWidget::chartInit(){

    // 在 DataCardWidget 构造函数中添加以下代码：

    // 创建折线系列对象
    series = new QLineSeries();


    series->setPointLabelsVisible(true);
    series->setPointLabelsFormat("@yPoint");
    series->setPointLabelsColor(Qt::black);
    series->setPointLabelsFont(QFont("Arial", 10));
    series->setPointLabelsClipping(false); // 允许标签显示在图表边界外

    // 创建图表对象
    chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("暂无数据");
    chart->legend()->setVisible(false);


    // 设置坐标轴
    axisX = new QValueAxis();
    axisX->setRange(0, 100);  // 设置X轴范围
    axisX->setLabelFormat("%d");  // 设置X轴标签格式
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    axisY = new QValueAxis();
    axisY->setRange(0, 100);  // 设置Y轴范围
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // 设置图表显示视图
    ui->widget->setChart(chart); // ui->widget 是你在 UI 中创建的 QChartView
    ui->widget->setRenderHint(QPainter::Antialiasing);  // 启用抗锯齿

}

void DataCardWidget::changeGraph(c1_graph sig){
    debugShow(sig);

    QValueAxis* xAxis = qobject_cast<QValueAxis*>(chart->axisX());
    QFont font("Arial", 10);  // 你可以根据需要调整字体
    xAxis->setLabelsFont(font);  // 设置 X 轴的字体
    chart->axisX()->setLabelsAngle(0);
    series->clear();
    chart->setTitle("");
    int x_min=100000,x_max=-1;
    double y_min=100000.0,y_max=-1.0;
    long long a=0,b=0;
    switch (sig.style) {
    case 1:
        //"运力匹配度"
        switch (sig.kelidu) {
        case 1://年
            for(auto it = system->data_preload_.lineCapacityYearly[sig.id].begin();it != system->data_preload_.lineCapacityYearly[sig.id].end();++it){

                double y_rounded = qRound(it.value() * 10) / 10.0;
                if(it.key()<x_min) x_min=it.key();
                if(it.key()>x_max) x_max=it.key();
                if(y_rounded<y_min) y_min=y_rounded;
                if(y_rounded>y_max) y_max=y_rounded;
                series->append(it.key(),y_rounded);

            }
            axisX->setRange(x_min-1,x_max+1);
            axisY->setRange(y_min-5,y_max+5);
            axisX->setTickCount(x_max-x_min+3);

            break;

        case 2://月
            for(auto it = system->data_preload_.lineCapacityMonthly[sig.id].begin();
                 it != system->data_preload_.lineCapacityMonthly[sig.id].end();++it){
                auto &a = it.key();
                if(a.first == sig.year){
                    double y_rounded = qRound(it.value() * 10) / 10.0;
                    if(a.second<x_min) x_min=a.second;
                    if(a.second>x_max) x_max=a.second;
                    if(y_rounded<y_min) y_min=y_rounded;
                    if(y_rounded>y_max) y_max=y_rounded;
                    series->append(a.second,y_rounded);
                }


            }
            if(x_min!=1)--x_min;
            if(x_max!=12)++x_max;
            axisX->setRange(x_min,x_max);
            axisY->setRange(y_min-5,y_max+5);
            axisX->setTickCount(x_max-x_min+1);

            break;

        case 3://日


            for(auto it = system->data_preload_.lineCapacityDaily[sig.id].begin();
                 it != system->data_preload_.lineCapacityDaily[sig.id].end();++it){
                auto &a = it.key();
                if(a.year() == sig.year&&a.month() == sig.month){
                    double y_rounded = qRound(it.value());
                    if(a.day()<x_min) x_min=a.day();
                    if(a.day()>x_max) x_max=a.day();
                    if(y_rounded<y_min) y_min=y_rounded;
                    if(y_rounded>y_max) y_max=y_rounded;
                    series->append(a.day(),y_rounded);
                }


            }
            if(x_min!=1)--x_min;
            if(x_max!=31)++x_max;
            if(x_max-x_min+1>25){
                QFont font("Arial", 4);  // 你可以根据需要调整字体
                xAxis->setLabelsFont(font);  // 设置 X 轴的字体
            }
            else if(x_max-x_min+1>20&&x_max-x_min+1<=25){
                QFont font("Arial", 7);  // 你可以根据需要调整字体
                xAxis->setLabelsFont(font);  // 设置 X 轴的字体


            }
            axisX->setRange(x_min,x_max);
            axisY->setRange(y_min-5,y_max+5);
            axisX->setTickCount(x_max-x_min+1);
            chart->update();
            break;

        case 4://时

            for(auto it = system->data_preload_.lineCapacityHourly[sig.id][sig.year][sig.month][sig.day].begin();
                 it != system->data_preload_.lineCapacityHourly[sig.id][sig.year][sig.month][sig.day].end();++it){

                double y_rounded = qRound(it.value() * 10) / 10.0;
                if(it.key()<x_min) x_min=it.key();
                if(it.key()>x_max) x_max=it.key();
                if(y_rounded<y_min) y_min=y_rounded;
                if(y_rounded>y_max) y_max=y_rounded;
                series->append(it.key(),y_rounded);
            }
            if(x_min!=0)--x_min;
            if(x_max!=24)++x_max;

            if(x_max-x_min+1>20){
                QFont font("Arial", 7);  // 你可以根据需要调整字体
                xAxis->setLabelsFont(font);  // 设置 X 轴的字体
            }
            axisX->setRange(x_min,x_max);
            axisY->setRange(y_min-5,y_max+5);
            axisX->setTickCount(x_max-x_min+1);

            break;
        }

        break;
    case 2:
        //"站点客流量"
        switch (sig.kelidu) {
        case 1://年
            for(auto it = system->data_preload_.stationFlowYearly[sig.id].begin();it != system->data_preload_.stationFlowYearly[sig.id].end();++it){

                long long y_rounded = it.value();
                if(it.key()<x_min) x_min=it.key();
                if(it.key()>x_max) x_max=it.key();
                if(y_rounded<y_min) y_min=y_rounded;
                if(y_rounded>y_max) y_max=y_rounded;
                series->append(it.key(),y_rounded);

            }
            axisX->setRange(x_min-1,x_max+1);
            a=round(y_min*0.9),b=round(y_max*1.1);
            axisY->setRange(a,b);
            axisX->setTickCount(x_max-x_min+3);

            break;

        case 2://月
            for(auto it = system->data_preload_.stationFlowMonthly[sig.id].begin();
                 it != system->data_preload_.stationFlowMonthly[sig.id].end();++it){
                auto &a = it.key();
                if(a.first == sig.year){
                    long long y_rounded = it.value();
                    if(a.second<x_min) x_min=a.second;
                    if(a.second>x_max) x_max=a.second;
                    if(y_rounded<y_min) y_min=y_rounded;
                    if(y_rounded>y_max) y_max=y_rounded;
                    series->append(a.second,y_rounded);
                }


            }
            if(x_min!=1)--x_min;
            if(x_max!=12)++x_max;
            axisX->setRange(x_min,x_max);
            a=round(y_min*0.9),b=round(y_max*1.1);
            axisY->setRange(a,b);
            axisX->setTickCount(x_max-x_min+1);

            break;

        case 3://日


            for(auto it = system->data_preload_.stationFlowDaily[sig.id].begin();
                 it != system->data_preload_.stationFlowDaily[sig.id].end();++it){
                auto &a = it.key();
                if(a.year() == sig.year&&a.month() == sig.month){
                    long long y_rounded = it.value();
                    if(a.day()<x_min) x_min=a.day();
                    if(a.day()>x_max) x_max=a.day();
                    if(y_rounded<y_min) y_min=y_rounded;
                    if(y_rounded>y_max) y_max=y_rounded;
                    series->append(a.day(),y_rounded);
                }


            }
            if(x_min!=1)--x_min;
            if(x_max!=31)++x_max;
            if(x_max-x_min+1>25){
                QFont font("Arial", 4);  // 你可以根据需要调整字体
                xAxis->setLabelsFont(font);  // 设置 X 轴的字体
            }
            else if(x_max-x_min+1>20&&x_max-x_min+1<=25){
                QFont font("Arial", 7);  // 你可以根据需要调整字体
                xAxis->setLabelsFont(font);  // 设置 X 轴的字体


            }
            axisX->setRange(x_min,x_max);
            a=round(y_min*0.9),b=round(y_max*1.1);
            axisY->setRange(a,b);
            axisX->setTickCount(x_max-x_min+1);
            chart->update();
            break;

        case 4://时

            for(auto it = system->data_preload_.stationFlowHourly[sig.id][sig.year][sig.month][sig.day].begin();
                 it != system->data_preload_.stationFlowHourly[sig.id][sig.year][sig.month][sig.day].end();++it){

                long long y_rounded = it.value();
                if(it.key()<x_min) x_min=it.key();
                if(it.key()>x_max) x_max=it.key();
                if(y_rounded<y_min) y_min=y_rounded;
                if(y_rounded>y_max) y_max=y_rounded;
                series->append(it.key(),y_rounded);
            }
            if(x_min!=0)--x_min;
            if(x_max!=24)++x_max;

            if(x_max-x_min+1>20){
                QFont font("Arial", 7);  // 你可以根据需要调整字体
                xAxis->setLabelsFont(font);  // 设置 X 轴的字体
            }
            axisX->setRange(x_min,x_max);
            a=round(y_min*0.9),b=round(y_max*1.1);
            axisY->setRange(a,b);
            axisX->setTickCount(x_max-x_min+1);

            break;
        }

        break;
    case 3:
        //"站点压力值"
        switch (sig.kelidu) {
        case 1://年
            for(auto it = system->data_preload_.stationPressureYearly[sig.id].begin();it != system->data_preload_.stationPressureYearly[sig.id].end();++it){

                double y_rounded = qRound(it.value() * 10 * 100) / 10.0;
                if(it.key()<x_min) x_min=it.key();
                if(it.key()>x_max) x_max=it.key();
                if(y_rounded<y_min) y_min=y_rounded;
                if(y_rounded>y_max) y_max=y_rounded;
                series->append(it.key(),y_rounded);

            }
            axisX->setRange(x_min-1,x_max+1);
            axisY->setRange(y_min-5,y_max+5);
            axisX->setTickCount(x_max-x_min+3);

            break;

        case 2://月
            for(auto it = system->data_preload_.stationPressureMonthly[sig.id].begin();
                 it != system->data_preload_.stationPressureMonthly[sig.id].end();++it){
                auto &a = it.key();
                if(a.first == sig.year){
                    double y_rounded = qRound(it.value() * 10 * 100) / 10.0;
                    if(a.second<x_min) x_min=a.second;
                    if(a.second>x_max) x_max=a.second;
                    if(y_rounded<y_min) y_min=y_rounded;
                    if(y_rounded>y_max) y_max=y_rounded;
                    series->append(a.second,y_rounded);
                }


            }
            if(x_min!=1)--x_min;
            if(x_max!=12)++x_max;
            axisX->setRange(x_min,x_max);
            axisY->setRange(y_min-5,y_max+5);
            axisX->setTickCount(x_max-x_min+1);

            break;

        case 3://日


            for(auto it = system->data_preload_.stationPressureDaily[sig.id].begin();
                 it != system->data_preload_.stationPressureDaily[sig.id].end();++it){
                auto &a = it.key();
                if(a.year() == sig.year&&a.month() == sig.month){
                    double y_rounded = qRound(it.value() * 100);
                    if(a.day()<x_min) x_min=a.day();
                    if(a.day()>x_max) x_max=a.day();
                    if(y_rounded<y_min) y_min=y_rounded;
                    if(y_rounded>y_max) y_max=y_rounded;
                    series->append(a.day(),y_rounded);
                }


            }
            if(x_min!=1)--x_min;
            if(x_max!=31)++x_max;
            if(x_max-x_min+1>25){
                QFont font("Arial", 4);  // 你可以根据需要调整字体
                xAxis->setLabelsFont(font);  // 设置 X 轴的字体
            }
            else if(x_max-x_min+1>20&&x_max-x_min+1<=25){
                QFont font("Arial", 7);  // 你可以根据需要调整字体
                xAxis->setLabelsFont(font);  // 设置 X 轴的字体


            }
            axisX->setRange(x_min,x_max);
            axisY->setRange(y_min-5,y_max+5);
            axisX->setTickCount(x_max-x_min+1);
            chart->update();
            break;

        case 4://时

            for(auto it = system->data_preload_.stationPressureHourly[sig.id][sig.year][sig.month][sig.day].begin();
                 it != system->data_preload_.stationPressureHourly[sig.id][sig.year][sig.month][sig.day].end();++it){

                double y_rounded = qRound(it.value() * 10 * 100) / 10.0;
                if(it.key()<x_min) x_min=it.key();
                if(it.key()>x_max) x_max=it.key();
                if(y_rounded<y_min) y_min=y_rounded;
                if(y_rounded>y_max) y_max=y_rounded;
                series->append(it.key(),y_rounded);
            }
            if(x_min!=0)--x_min;
            if(x_max!=24)++x_max;

            if(x_max-x_min+1>20){
                QFont font("Arial", 7);  // 你可以根据需要调整字体
                xAxis->setLabelsFont(font);  // 设置 X 轴的字体
            }
            axisX->setRange(x_min,x_max);
            axisY->setRange(y_min-5,y_max+5);
            axisX->setTickCount(x_max-x_min+1);

            break;
        }

        break;



    }
    chart->update();
}

void DataCardWidget::debugShow(c1_graph sig){
    // 输出结构体内容，使用中文描述
    qDebug() << "收到信号：";

    // 打印 style 的中文描述
    QString styleText;
    switch (sig.style) {
    case 1: styleText = "运力匹配度"; break;
    case 2: styleText = "站点客流量"; break;
    case 3: styleText = "站点压力值"; break;
    default: styleText = "未知"; break;
    }

    // 打印 kelidu 的中文描述
    QString keliduText;
    switch (sig.kelidu) {
    case 1: keliduText = "年"; break;
    case 2: keliduText = "月"; break;
    case 3: keliduText = "日"; break;
    case 4: keliduText = "时"; break;
    default: keliduText = "未知"; break;
    }

    // 输出每个字段的中文描述
    qDebug() << "样式：" << styleText;
    qDebug() << "颗粒度：" << keliduText;
    qDebug() << "ID：" << sig.id;
    qDebug() << "年份：" << sig.year;
    qDebug() << "月份：" << sig.month;
    qDebug() << "日期：" << sig.day;
}

void DataCardWidget::setfont(){
    int font1 = QFontDatabase::addApplicationFont(":/font/syHei-Regular.otf");
    if(font1==-1){
        qDebug()<<"syHei-Regular.otf加载失败！";
    }
    else{
        QString syHei_Regular = QFontDatabase::applicationFontFamilies(font1).at(0);
        ui->style->setFont       (QFont(syHei_Regular,13));
        ui->kelidu->setFont      (QFont(syHei_Regular,13));
        ui->id->setFont          (QFont(syHei_Regular,13));
        ui->year->setFont        (QFont(syHei_Regular,13));
        ui->month->setFont       (QFont(syHei_Regular,13));
        ui->day->setFont         (QFont(syHei_Regular,13));



    }


}
