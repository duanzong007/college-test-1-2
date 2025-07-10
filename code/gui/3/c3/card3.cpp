#include "card3.h"
#include "ui_card3.h"
#include <QGraphicsDropShadowEffect>

#include <QStandardItemModel>
card3::card3(QWidget *parent, core_system* s)
    : QWidget(parent)
    , ui(new Ui::card3)
    , system(s)
{
    ui->setupUi(this);

    QGraphicsDropShadowEffect *shadowEffect1 = new QGraphicsDropShadowEffect(this);
    shadowEffect1->setColor(QColor(0, 0, 0, 40));  // 黑色，透明度160（0-255）
    shadowEffect1->setOffset(3, 3);  // 水平和垂直偏移量
    shadowEffect1->setBlurRadius(4);  // 模糊半径
    ui->leftcard->setGraphicsEffect(shadowEffect1);
    QGraphicsDropShadowEffect *shadowEffect2 = new QGraphicsDropShadowEffect(this);
    shadowEffect2->setColor(QColor(0, 0, 0, 40));  // 黑色，透明度160（0-255）
    shadowEffect2->setOffset(3, 3);  // 水平和垂直偏移量
    shadowEffect2->setBlurRadius(4);  // 模糊半径
    ui->rightcard->setGraphicsEffect(shadowEffect2);
    // 为每个控件创建独立的阴影效果
    QGraphicsDropShadowEffect *shadowEffect3 = new QGraphicsDropShadowEffect(this);
    shadowEffect3->setColor(QColor(0, 0, 0, 30));  // 黑色，透明度160（0-255）
    shadowEffect3->setOffset(2, 2);  // 水平和垂直偏移量
    shadowEffect3->setBlurRadius(2);  // 模糊半径
    ui->confirm->setGraphicsEffect(shadowEffect3);
    setfont();
    new predictor(this, system);


}

card3::~card3()
{
    delete ui;
}


void card3::setfont(){
    // 加载字体并应用到控件
    int font1 = QFontDatabase::addApplicationFont(":/font/syHei-Medium.otf");
    if(font1 == -1){
        qDebug()<<"syHei-Medium.otf加载失败！";
    }
    else{
        QString syHei_Medium = QFontDatabase::applicationFontFamilies(font1).at(0);
        ui->main_label->setFont(QFont(syHei_Medium, 16));
    }
    // 加载字体并应用到控件
    int font2 = QFontDatabase::addApplicationFont(":/font/syHei-Regular.otf");
    if(font2 == -1){
        qDebug()<<"syHei-Regular.otf加载失败！";
    }
    else{
        QString syHei_Regular = QFontDatabase::applicationFontFamilies(font2).at(0);
        ui->confirm   ->setFont(QFont(syHei_Regular, 15));
        ui->station   ->setFont(QFont(syHei_Regular, 13));
        ui->hour      ->setFont(QFont(syHei_Regular, 13));
        ui->chooseTime->setFont(QFont(syHei_Regular, 13));
        ui->aimTime   ->setFont(QFont(syHei_Regular, 13));
    }



}
