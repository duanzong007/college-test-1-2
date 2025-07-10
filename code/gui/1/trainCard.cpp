#include "trainCard.h"
#include "ui_trainCard.h"
#include <QGraphicsDropShadowEffect>
trainCard::trainCard(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::trainCard)
{
    ui->setupUi(this);

    // 创建阴影效果
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    // 设置阴影的颜色
    shadowEffect->setColor(QColor(0, 0, 0, 40));  // 黑色，透明度160（0-255）
    // 设置阴影偏移量
    shadowEffect->setOffset(3, 3);  // 水平和垂直偏移量
    // 设置阴影的模糊半径
    shadowEffect->setBlurRadius(5);  // 模糊半径（控制阴影的范围）
    // 应用阴影效果到卡片
    this->setGraphicsEffect(shadowEffect);

    setfont();
}

trainCard::~trainCard()
{
    delete ui;
}

void trainCard::setTrainInfo(const TrainInfo &info)
{
    ui->codevalue    ->setText(info.code);           // 车次号
    ui->idvalue      ->setText(QString::number(info.trainId));  // 列车ID
    ui->peoplevalue  ->setText(QString::number(info.people));    // 当前人数
    ui->capacityvalue->setText(QString::number(info.capacity));  // 额定载荷
    ui->manvalue     ->setText(QString::asprintf("%.1f %%", info.loadRate));  // 列车满载率
    ui->linevalue    ->setText(QString::number(info.lineId)); // 所属线路
    ui->startvalue   ->setText(info.startStation);   // 始发站
    ui->endvalue     ->setText(info.endStation);     // 终点站
}



void trainCard::setfont(){
    int font1 = QFontDatabase::addApplicationFont(":/font/sySong-Regular.otf");
    if(font1==-1){
        qDebug()<<"sySong-Regular.otf加载失败！";
    }
    else{
        QString sySong_Regular = QFontDatabase::applicationFontFamilies(font1).at(0);
        ui->capacitytext-> setFont(QFont(sySong_Regular,14));
        ui->capacityvalue->setFont(QFont(sySong_Regular,14));
        ui->peopletext->   setFont(QFont(sySong_Regular,14));
        ui->peoplevalue->  setFont(QFont(sySong_Regular,14));
        ui->mantext->      setFont(QFont(sySong_Regular,14));
        ui->manvalue->     setFont(QFont(sySong_Regular,14));
        ui->starttext->    setFont(QFont(sySong_Regular,14));
        ui->startvalue->   setFont(QFont(sySong_Regular,14));
        ui->endtext->      setFont(QFont(sySong_Regular,14));
        ui->endvalue->     setFont(QFont(sySong_Regular,14));
        ui->idtext->       setFont(QFont(sySong_Regular,14));
        ui->idvalue->      setFont(QFont(sySong_Regular,14));
    }
    int font2 = QFontDatabase::addApplicationFont(":/font/syHei-Bold.otf");
    if(font2==-1){
        qDebug()<<"syHei-Bold.otf加载失败！";
    }
    else{
        QString syHei_Bold = QFontDatabase::applicationFontFamilies(font2).at(0);

        ui->codetext->  setFont(QFont(syHei_Bold,15));
        ui->codevalue-> setFont(QFont(syHei_Bold,15));
        ui->linetext->  setFont(QFont(syHei_Bold,15));
        ui->linevalue-> setFont(QFont(syHei_Bold,15));
    }

}
