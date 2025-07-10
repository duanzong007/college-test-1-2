#include "CYCardWidget.h"
#include "ui_CYCardWidget.h"
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSimpleTextItem>
#include <QtCharts/QAbstractBarSeries>
#include <QtCharts/QAbstractSeries>
#include <QBrush>

CYCardWidget::CYCardWidget(QWidget *parent, core_system* s)
    : QWidget(parent)
    , ui(new Ui::CYCardWidget)
    , system(s)
{
    ui->setupUi(this);

    // 为控件设置阴影效果
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

    // 设置系统和 FilterManager
    filterMgr.setsystem(s);
    filterMgr.setComboBoxes(ui->kelidu, ui->year, ui->month);
    filterMgr.initializeComboBoxes();

    connect(&filterMgr, &FilterManager_c2::graphChange, this, &CYCardWidget::changeGraph);
    setfont();
    chartInit();
}

CYCardWidget::~CYCardWidget()
{
    delete ui;
}


void CYCardWidget::chartInit(){
    // 1. 创建两组柱状数据
    setA = new QBarSet("四川-重庆");
    setB = new QBarSet("重庆-四川");

    // 2. 创建分组柱状序列
    series = new QBarSeries();
    series->append(setA);
    series->append(setB);

    // 在 QBarSeries 上打开标签
    // series->setLabelsVisible(true);
    // series->setLabelsFormat("@value");
    // series->setLabelsPosition(QAbstractBarSeries::LabelsInsideEnd);


    // 3. 创建图表并添加序列
    chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("暂无数据");
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);



    // 4. X 轴改用 QBarCategoryAxis
    axisX = new QBarCategoryAxis();
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // 5. Y 轴用数值轴
    axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // 6. 绑定到 QChartView
    ui->widget->setChart(chart);
    ui->widget->setRenderHint(QPainter::Antialiasing);
}


void CYCardWidget::changeGraph(c2_graph sig){
    chart->setTitle("客流量/人");
    debugShow(sig);
    if (setA->count() > 0)
        setA->remove(0, setA->count());
    if (setB->count() > 0)
        setB->remove(0, setB->count());
    axisX->clear();

    long long max=-1,b=0;
    int count=0;
    auto* xAxis = qobject_cast<QBarCategoryAxis*>(chart->axisX());
    QFont font("Arial", 9);  // 你可以根据需要调整字体
    xAxis->setLabelsFont(font);  // 设置 X 轴的字体
    switch (sig.kelidu) {
    case 1://年
        for(auto it = system->flow_cydiffer_.year_flow.begin();it != system->flow_cydiffer_.year_flow.end();++it){

            auto &a = it.value();
            if(a.c_to_y>max) max=a.c_to_y;
            if(a.y_to_c>max) max=a.y_to_c;
            setA->append(a.c_to_y);
            setB->append(a.y_to_c);
            axisX->append(QString("%1年").arg(it.key()));

            ++count;
        }
        b=round(max*1.1);
        axisY->setRange(0,b);

        break;

    case 2://月

        for(auto it = system->flow_cydiffer_.month_flow.begin();it != system->flow_cydiffer_.month_flow.end();++it){

            if(it.key().year == sig.year){
                auto &a = it.value();
                if(a.c_to_y>max) max=a.c_to_y;
                if(a.y_to_c>max) max=a.y_to_c;
                setA->append(a.c_to_y);
                setB->append(a.y_to_c);
                axisX->append(QString("%1月").arg(it.key().month));
                ++count;
            }

        }
        b=round(max*1.1);
        axisY->setRange(0,b);

        break;

    case 3://日
        for(auto it = system->flow_cydiffer_.flows.begin();it != system->flow_cydiffer_.flows.end();++it){

            if(it.key().year() == sig.year&&it.key().month() == sig.month){
                auto &a = it.value();
                if(a.c_to_y>max) max=a.c_to_y;
                if(a.y_to_c>max) max=a.y_to_c;
                setA->append(a.c_to_y);
                setB->append(a.y_to_c);
                axisX->append(QString::number(it.key().day()));
                ++count;
            }

        }
        b=round(max*1.1);
        axisY->setRange(0,b);


        if(count>25){
            QFont font("Arial", 5);  // 你可以根据需要调整字体
            xAxis->setLabelsFont(font);  // 设置 X 轴的字体
        }
        else if(count>20&&count<=25){
            QFont font("Arial", 6);  // 你可以根据需要调整字体
            xAxis->setLabelsFont(font);  // 设置 X 轴的字体


        }


        break;
    }

    // 1. 删除上次画的标签
    for (auto *it : m_barLabels) {
        chart->scene()->removeItem(it);
        delete it;
    }
    m_barLabels.clear();
    //qDebug()<<count;
    if(count<25){


        // 2. 重绘每根柱子的文本
        int setCount = series->barSets().count();
        double bw     = series->barWidth();
        auto  sets    = series->barSets();
        const qreal padding = 2;  // 每碰撞一次就上移的额外距离

        for (int si = 0; si < setCount; ++si) {
            QBarSet *bs = sets.at(si);
            for (int ci = 0; ci < bs->count(); ++ci) {
                qreal value = bs->at(ci);

                // —— 2.1. 自定义 X 轴逻辑 ——
                double xLog = ci + (si - (setCount - 1) / 2.0) * bw;
                if (si == 0)          xLog = ci + (si - (setCount - 1) / 4.0) * bw;
                else if (si == 1)     xLog = ci + (si - (setCount - 1) / 1.33) * bw;

                // —— 2.2. 转到 scene 坐标 ——
                QPointF pt = chart->mapToPosition(QPointF(xLog, value), series);

                // —— 2.3. 新建文本项，初始放在柱顶正上方 2px ——
                auto *label = chart->scene()->addSimpleText(QString::number(value));
                label->setBrush(Qt::black);
                QRectF br = label->boundingRect();
                QPointF pos(pt.x() - br.width()/2,
                            pt.y() - br.height() - 2);
                QRectF rect(pos, br.size());

                // —— 2.4. 碰撞检测 & 向上平移 ——
                bool collision;
                do {
                    collision = false;
                    for (auto *prev : m_barLabels) {
                        if (prev->sceneBoundingRect().intersects(rect)) {
                            rect.translate(0, (br.height() + padding - 5));
                            collision = true;
                            break;
                        }
                    }
                } while (collision);

                // —— 2.5. 最终定位 & 存储 ——
                label->setPos(rect.topLeft());
                m_barLabels.append(label);
            }
        }

    }


    ui->widget->update();
}

void CYCardWidget::debugShow(c2_graph sig){
    // 输出结构体内容，使用中文描述
    qDebug() << "收到信号：";



    QString keliduText;
    switch (sig.kelidu) {
    case 1: keliduText = "年"; break;
    case 2: keliduText = "月"; break;
    case 3: keliduText = "日"; break;
    default: keliduText = "未知"; break;
    }

    qDebug() << "颗粒度：" << keliduText;
    qDebug() << "年份：" << sig.year;
    qDebug() << "月份：" << sig.month;
}

void CYCardWidget::setfont(){
    int font1 = QFontDatabase::addApplicationFont(":/font/syHei-Regular.otf");
    if(font1 == -1){
        qDebug()<<"syHei-Regular.otf加载失败！";
    }
    else{
        QString syHei_Regular = QFontDatabase::applicationFontFamilies(font1).at(0);
        ui->kelidu->setFont      (QFont(syHei_Regular,13));
        ui->year->setFont        (QFont(syHei_Regular,13));
        ui->month->setFont       (QFont(syHei_Regular,13));
    }
}
