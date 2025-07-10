#include "lineCard.h"
#include "ui_lineCard.h"
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QString>

lineCard::lineCard(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::lineCard)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground);

    /* 把复选框变化转发给外部 */
    connect(ui->visibleCheck, &QCheckBox::toggled,
            this, [this](bool on){
                emit visibilityChanged(m_id, on);
            });

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

lineCard::~lineCard()
{
    delete ui;
}

/* ===== 公共接口实现 ===== */

void lineCard::setLineInfo(const LineInfo &info)
{
    m_id = info.id;

    // 屏蔽信号，避免翻页时走 onLineVisibilityChanged
    ui->visibleCheck->blockSignals(true);
    ui->visibleCheck->setChecked(info.visible);
    ui->visibleCheck->blockSignals(false);

    /* 顶部 - 线路 ID 与匹配度 */
    ui->linevalue->setText(QString::number(info.id));
    if (info.matchPercent >= 0.0) {
        // 保留 2 位小数
        ui->peivalue->setText(QStringLiteral("%1 %")
                                  .arg(QString::number(info.matchPercent, 'f', 2)));
    } else {
        ui->peivalue->setText(QStringLiteral("暂无数据"));
    }

    /* 站名 */
    ui->start->setText(info.startStation);
    ui->end->setText(info.endStation);

    /* 主色 & 勾选状态 */
    applyColor(info.color);
}

void lineCard::setColor(const QColor &c)
{
    applyColor(c);
}

void lineCard::setChecked(bool on)
{
    ui->visibleCheck->setChecked(on);
}

/* ===== 私有助手：统一替换主色 ===== */

void lineCard::applyColor(const QColor &c)
{
    const QString col  = c.name(QColor::HexRgb);             // #rrggbb
    const QString colL = QColor(c).lighter(115).name();      // Hover 更亮一点

    /* 圆圈 + 直线 */
    const QString circleCss = QString("background:%1;border-radius:8px;").arg(col);
    ui->circleLeft ->setStyleSheet(circleCss);
    ui->circleRight->setStyleSheet(circleCss);
    ui->lineBar    ->setStyleSheet(QString("background:%1;").arg(col));

}


void lineCard::setfont(){
    int font1 = QFontDatabase::addApplicationFont(":/font/syHei-Regular.otf");
    if(font1==-1){
        qDebug()<<"syHei-Regular.otf加载失败！";
    }
    else{
        QString syHei_Regular = QFontDatabase::applicationFontFamilies(font1).at(0);
        ui->linetext-> setFont(QFont(syHei_Regular,14));
        ui->linevalue->setFont(QFont(syHei_Regular,14));
        ui->peitext->  setFont(QFont(syHei_Regular,14));
        ui->peivalue-> setFont(QFont(syHei_Regular,14));
    }
    int font2 = QFontDatabase::addApplicationFont(":/font/syHei-Bold.otf");
    if(font2==-1){
        qDebug()<<"syHei-Bold.otf加载失败！";
    }
    else{
        QString syHei_Bold = QFontDatabase::applicationFontFamilies(font2).at(0);
        ui->start->    setFont(QFont(syHei_Bold,17));
        ui->end->      setFont(QFont(syHei_Bold,17));
    }



}
