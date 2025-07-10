#include "card2.h"
#include "ui_card2.h"
#include <QFontDatabase>
#include <QGraphicsDropShadowEffect>

card2::card2(QWidget *parent, core_system* s)
    : QWidget(parent)
    , ui(new Ui::card2)
    , system(s)
{
    ui->setupUi(this);

    // 初始化两个DataCardWidget
    graph1 = new CYCardWidget(this, system);
    graph2 = new CYCardWidget(this, system);

    // 将图表添加到布局
    ui->layout1->addWidget(graph1);
    ui->layout2->addWidget(graph2);

    // 为每个控件创建独立的阴影效果
    QGraphicsDropShadowEffect *shadowEffect1 = new QGraphicsDropShadowEffect(this);
    shadowEffect1->setColor(QColor(0, 0, 0, 30));  // 黑色，透明度160（0-255）
    shadowEffect1->setOffset(2, 2);  // 水平和垂直偏移量
    shadowEffect1->setBlurRadius(2);  // 模糊半径
    ui->duibi->setGraphicsEffect(shadowEffect1);

    // 设置字体
    setfont();

    // 初始化可见性
    ui->lineshow->setVisible(false);
    ui->graph2->setVisible(false);
}

card2::~card2()
{
    delete ui;
}

void card2::setfont(){
    // 加载字体并应用到控件
    int font1 = QFontDatabase::addApplicationFont(":/font/syHei-Medium.otf");
    if(font1 == -1){
        qDebug()<<"syHei-Medium.otf加载失败！";
    }
    else{
        QString syHei_Medium = QFontDatabase::applicationFontFamilies(font1).at(0);
        ui->label->setFont(QFont(syHei_Medium, 16));
    }

    int font2 = QFontDatabase::addApplicationFont(":/font/syHei-Regular.otf");
    if(font2 == -1){
        qDebug()<<"syHei-Regular.otf加载失败！";
    }
    else{
        QString syHei_Regular = QFontDatabase::applicationFontFamilies(font2).at(0);
        ui->duibi->setFont(QFont(syHei_Regular, 15));
    }
}

void card2::on_duibi_clicked()
{
    // 取反当前可见性
    bool willShow = ! ui->lineshow->isVisible();
    ui->lineshow->setVisible(willShow);
    ui->graph2->setVisible(willShow);
}
