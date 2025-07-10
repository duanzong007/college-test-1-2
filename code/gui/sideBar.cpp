#include "sideBar.h"
#include "ui_sideBar.h"
#include <QEvent>
#include <QStyle>
#include <QFontDatabase>
#include <QDebug>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QRandomGenerator>
#include <algorithm>  // for std::max


sideBar::sideBar(core_system* sys, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::sideBar)
    , m_system(sys)
{
    ui->setupUi(this);
    /* 1. 收集按钮 */
    m_buttons << ui->lineButton
              << ui->trafficButton
              << ui->dataAnalysisButton
              << ui->infoManagementButton;

    /* 2. 初始化按钮属性/事件过滤器 */
    for (auto *w : m_buttons) {
        w->installEventFilter(this);
    }

    /* 3. 默认选中第一页 */
    setSelected(0);

    setfont();

    ui->toggleBtn->setCheckable(true);  // 让按钮能“勾选”
    ui->toggleBtn->setChecked(false);   // 默认是展开状态（<）
    ui->photo1->installEventFilter(this); // 安装事件过滤器
    /* 连接折叠按钮 */
    connect(ui->toggleBtn, &QPushButton::clicked,
            this, &sideBar::toggleCollapse);



    page0 = new page0_main(ui->page0, m_system);
    ui->page0_layout->addWidget(page0);

    page1 = new page1_main(ui->page1, m_system);
    ui->page1_layout->addWidget(page1);

    page2 = new page2_main(ui->page2, m_system);
    ui->page2_layout->addWidget(page2);

    page3 = new page3_main(ui->page3, m_system);
    ui->page3_layout->addWidget(page3);

    ui->smallphoto->setVisible(false);

    // 在 sideBar 的构造函数中连接 photo1 的点击事件
    //connect(ui->photo1, &QWidget::mousePressEvent, this, &sideBar::on_photo1_clicked);

}

sideBar::~sideBar()
{
    delete ui;
}



void sideBar::onphoto1clicked() {
    // 创建爱心标签
    QLabel *heart = new QLabel(this);
    QPixmap heartPixmap(":/images/sideBar/heart.png");

    if (heartPixmap.isNull()) {
        qDebug() << "Heart image not loaded";  // 确认图片是否加载成功
    }

    // 随机调整图片大小，保持比例
    heartPixmap = heartPixmap.scaled(28, 28, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    heart->setPixmap(heartPixmap);  // 设置爱心图片
    heart->setAlignment(Qt::AlignCenter);
    heart->resize(heartPixmap.size());  // 调整 QLabel 的大小来适应图片

    // 获取 photo1 相对于 sideBar 的位置
    QPoint photo1Pos = ui->photo1->mapTo(this, QPoint(0, 0));

    // 随机左右偏移：设置一个随机的X轴偏移，范围为±20像素
    int randomXOffset = QRandomGenerator::global()->bounded(-20, 21);  // 随机偏移 -20 到 +20

    // 计算 heart 的起始位置，确保它从 photo1 的中心开始，并加上左右偏移
    heart->move(photo1Pos.x() + ui->photo1->width() / 2 - heart->width() / 2 + randomXOffset,
                photo1Pos.y() + ui->photo1->height() / 2 - heart->height() / 2 - 20);

    // 确保 QLabel 添加到正确的父控件，并强制显示
    heart->setParent(this);
    heart->show();
    heart->raise();  // 确保它显示在最上层

    // 设置透明度效果，使爱心消失
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(heart);
    heart->setGraphicsEffect(opacityEffect);

    // 透明度动画
    QPropertyAnimation *fadeOutAnimation = new QPropertyAnimation(opacityEffect, "opacity");
    fadeOutAnimation->setDuration(1000);  // 持续1秒
    fadeOutAnimation->setStartValue(1);
    fadeOutAnimation->setEndValue(0);

    // 随机飘动高度：设置一个随机的飘动高度，范围在 80 到 120 像素之间
    int randomHeightOffset = QRandomGenerator::global()->bounded(80, 121); // 随机飘动的高度（80 到 120 像素）

    // 位置动画
    QPropertyAnimation *moveUpAnimation = new QPropertyAnimation(heart, "pos");
    moveUpAnimation->setDuration(1000);
    moveUpAnimation->setStartValue(heart->pos());
    moveUpAnimation->setEndValue(heart->pos() + QPoint(0, -randomHeightOffset));  // 向上移动随机高度

    // 动画调试
    moveUpAnimation->start(QPropertyAnimation::DeleteWhenStopped);
    fadeOutAnimation->start(QPropertyAnimation::DeleteWhenStopped);

    // 动画结束后删除爱心控件
    connect(fadeOutAnimation, &QPropertyAnimation::finished, heart, &QLabel::deleteLater);
}


void sideBar::setfont(){
    int font1 = QFontDatabase::addApplicationFont(":/font/syHei-Medium.otf");
    if(font1==-1){
        qDebug()<<"syHei-Medium.otf加载失败！";
    }
    else{
        QString syHei_Medium = QFontDatabase::applicationFontFamilies(font1).at(0);
        ui->text1->setFont(QFont(syHei_Medium,15));
        ui->text2->setFont(QFont(syHei_Medium,15));
        ui->text3->setFont(QFont(syHei_Medium,15));
        ui->text4->setFont(QFont(syHei_Medium,15));
    }
    int font2 = QFontDatabase::addApplicationFont(":/font/syHei-Light.otf");
    if(font2==-1){
        qDebug()<<"syHei-Light.otf加载失败！";
    }
    else{
        QString syHei_Light = QFontDatabase::applicationFontFamilies(font2).at(0);
        ui->label->setFont(QFont(syHei_Light,11));

    }

}

/* ========= 内部工具：刷选中 + 切页 =========== */
void sideBar::setSelected(int index)
{
    for (int i = 0; i < m_buttons.size(); ++i) {
        applyStyle(m_buttons[i], i == index);
    }
    /* 关键：切换 barContent(QStackedWidget) 当前页 */
    ui->barContent->setCurrentIndex(index);
}

void sideBar::applyStyle(QWidget *w, bool selected)
{
    w->setProperty("selected", selected);
    w->style()->unpolish(w);
    w->style()->polish(w);
    w->update();
}

/* ========= 事件过滤器：检测 MouseButtonPress =========== */
bool sideBar::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->photo1 && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            onphoto1clicked();  // 执行点击时的逻辑
            return true;  // 吃掉事件，防止传递
        }
    }
    if (event->type() == QEvent::MouseButtonPress) {
        int idx = m_buttons.indexOf(static_cast<QWidget*>(obj));
        if (idx != -1)
            setSelected(idx);    // 内部完成“按钮高亮 + 切 QStackedWidget”
        return true;             // 吃掉点击
    }
    return QWidget::eventFilter(obj, event);
}


void sideBar::toggleCollapse()
{
    setCollapsed(!m_collapsed);
}

void sideBar::setCollapsed(bool collapse)
{
    if (collapse == m_collapsed) return;          // 状态没变

    m_collapsed = collapse;

    /* 1) 动画侧边栏宽度（对 bar 而不是 whole sideBar）*/
    auto *ani = new QPropertyAnimation(ui->bar, "maximumWidth", this);
    ani->setDuration(200);
    ani->setEasingCurve(QEasingCurve::OutCubic);
    ani->setStartValue(ui->bar->maximumWidth());
    ani->setEndValue(collapse ? 80 : 200);
    ani->start(QAbstractAnimation::DeleteWhenStopped);

    /* 2) 同时隐藏 / 显示文字 */
    const bool showText = !collapse;
    ui->text1->setVisible(showText);
    ui->text2->setVisible(showText);
    ui->text3->setVisible(showText);
    ui->text4->setVisible(showText);
    ui->label->setVisible(showText);
    ui->photo->setVisible(showText);
    ui->smallphoto->setVisible(collapse);

    /* 3) 换按钮图标方向 */
    ui->toggleBtn->setChecked(collapse);
}

