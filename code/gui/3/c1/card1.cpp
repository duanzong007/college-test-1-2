#include "card1.h"
#include "ui_card1.h"
#include <QFontDatabase>
#include <QGraphicsDropShadowEffect>
card1::card1(QWidget *parent, core_system* s)
    : QWidget(parent)
    , ui(new Ui::card1)
    , system(s)
{
    ui->setupUi(this);

    graph1 = new DataCardWidget(this, system);
    graph2 = new DataCardWidget(this, system);
    ui->layout1->addWidget(graph1);
    ui->layout2->addWidget(graph2);

    // 为每个控件创建独立的阴影效果
    QGraphicsDropShadowEffect *shadowEffect1 = new QGraphicsDropShadowEffect(this);
    shadowEffect1->setColor(QColor(0, 0, 0, 30));  // 黑色，透明度160（0-255）
    shadowEffect1->setOffset(2, 2);  // 水平和垂直偏移量
    shadowEffect1->setBlurRadius(2);  // 模糊半径
     ui->duibi->setGraphicsEffect(shadowEffect1);

    setfont();


    ui->lineshow->setVisible(false);
    ui->graph2->setVisible(false);
}

card1::~card1()
{
    delete ui;
}


void card1::setfont(){

    int font1 = QFontDatabase::addApplicationFont(":/font/syHei-Medium.otf");
    if(font1==-1){
        qDebug()<<"syHei-Medium.otf加载失败！";
    }
    else{
        QString syHei_Medium = QFontDatabase::applicationFontFamilies(font1).at(0);
        ui->label->setFont(QFont(syHei_Medium,16));
    }
    int font2 = QFontDatabase::addApplicationFont(":/font/syHei-Regular.otf");
    if(font1==-1){
        qDebug()<<"syHei-Regular.otf加载失败！";
    }
    else{
        QString syHei_Regular = QFontDatabase::applicationFontFamilies(font2).at(0);
        ui->duibi->setFont(QFont(syHei_Regular,15));
    }

}


void card1::on_duibi_clicked()
{
    // 取反当前可见性
    bool willShow = ! ui->lineshow->isVisible();
    ui->lineshow->setVisible(willShow);
    ui->graph2->setVisible(willShow);
}
