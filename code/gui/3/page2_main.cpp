#include "page2_main.h"
#include "ui_page2_main.h"
#include <QGraphicsDropShadowEffect>
page2_main::page2_main(QWidget *parent, core_system* s)
    : QWidget(parent)
    , ui(new Ui::page2_main)
    , system(s)
{
    ui->setupUi(this);


    QGraphicsDropShadowEffect *shadowEffect1 = new QGraphicsDropShadowEffect(this);
    shadowEffect1->setColor(QColor(0, 0, 0, 40));
    shadowEffect1->setOffset(3, 3);
    shadowEffect1->setBlurRadius(5);
    ui->widget1->setGraphicsEffect(shadowEffect1);

    QGraphicsDropShadowEffect *shadowEffect2 = new QGraphicsDropShadowEffect(this);
    shadowEffect2->setColor(QColor(0, 0, 0, 40));
    shadowEffect2->setOffset(3, 3);
    shadowEffect2->setBlurRadius(5);
    ui->widget2->setGraphicsEffect(shadowEffect2);

    QGraphicsDropShadowEffect *shadowEffect3 = new QGraphicsDropShadowEffect(this);
    shadowEffect3->setColor(QColor(0, 0, 0, 40));
    shadowEffect3->setOffset(3, 3);
    shadowEffect3->setBlurRadius(5);
    ui->widget3->setGraphicsEffect(shadowEffect3);

    c1 = new card1(this, system);
    c2 = new card2(this, system);
    c3 = new card3(this, system);

    ui->layout1->addWidget(c1);
    ui->layout2->addWidget(c2);
    ui->layout3->addWidget(c3);


}

page2_main::~page2_main()
{
    delete ui;
}
