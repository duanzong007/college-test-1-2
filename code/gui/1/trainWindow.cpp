#include "trainwindow.h"
#include "ui_trainwindow.h"

#ifdef Q_OS_WIN
# include <windows.h>
# include <dwmapi.h>
# include <windowsx.h>
# ifdef _MSC_VER
#  pragma comment(lib, "Dwmapi.lib")
# endif
#endif
#include <QGraphicsDropShadowEffect>
#include <QToolButton>
#include <QScreen>
#include <QFontDatabase>
#include <QScrollBar>

/* ───────── 构造 & 析构 ───────── */
trainWindow::trainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::trainWindow)
{
    setWindowFlag(Qt::FramelessWindowHint);
    // setAttribute(Qt::WA_TranslucentBackground);

    ui->setupUi(this);

    /* 三颗系统按钮 */
    connect(ui->closeButton, &QToolButton::clicked, this, &QWidget::hide);
    connect(ui->minButton,   &QToolButton::clicked, this, &trainWindow::showMinimized);
    connect(ui->maxButton,   &QToolButton::clicked, this, [this]{
#ifdef Q_OS_WIN
        HWND hwnd = reinterpret_cast<HWND>(winId());
        if (isMaximized())
            ::SendMessageW(hwnd, WM_SYSCOMMAND, SC_RESTORE,  0);
        else
            ::SendMessageW(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
#else
        isMaximized() ? showNormal() : showMaximized();
#endif
    });

#ifdef Q_OS_WIN
    /* 保留 WS_CAPTION | WS_THICKFRAME，以恢复系统动画 */
    HWND hwnd = reinterpret_cast<HWND>(winId());
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style |= WS_CAPTION | WS_THICKFRAME;
    SetWindowLong(hwnd, GWL_STYLE, style);

    const MARGINS shadow = {1,1,1,1};
    DwmExtendFrameIntoClientArea(hwnd, &shadow);
#endif

    /* -------- 根据 DPI 适配初始尺寸 -------- */
    constexpr int kBaseW = 450;
    constexpr int kBaseH = 900;

    const qreal dpr = devicePixelRatioF();
    resize(QSize(qRound(kBaseW / dpr), qRound(kBaseH / dpr)));



    ui->listWidget->setVerticalScrollMode(QListWidget::ScrollPerPixel);


    buildCardPool();                                   // ★ 固定 10 张空卡片

    connect(ui->prevBtn, &QPushButton::clicked,
            this, &trainWindow::onPrevPage);
    connect(ui->nextBtn, &QPushButton::clicked,
            this, &trainWindow::onNextPage);
    connect(ui->pageEdit,&QLineEdit::returnPressed,
            this, &trainWindow::onJumpPage);

    /* 让输入框只接受正整数；稍后动态修改范围 */
    ui->pageEdit->setValidator(new QIntValidator(ui->pageEdit));


    // 为每个控件创建独立的阴影效果
    QGraphicsDropShadowEffect *shadowEffect1 = new QGraphicsDropShadowEffect(this);
    shadowEffect1->setColor(QColor(0, 0, 0, 40));  // 黑色，透明度160（0-255）
    shadowEffect1->setOffset(2, 2);  // 水平和垂直偏移量
    shadowEffect1->setBlurRadius(4);  // 模糊半径

    QGraphicsDropShadowEffect *shadowEffect2 = new QGraphicsDropShadowEffect(this);
    shadowEffect2->setColor(QColor(0, 0, 0, 40));  // 黑色，透明度160（0-255）
    shadowEffect2->setOffset(2, 2);  // 水平和垂直偏移量
    shadowEffect2->setBlurRadius(4);  // 模糊半径

    QGraphicsDropShadowEffect *shadowEffect3 = new QGraphicsDropShadowEffect(this);
    shadowEffect3->setColor(QColor(0, 0, 0, 40));  // 黑色，透明度160（0-255）
    shadowEffect3->setOffset(2, 2);  // 水平和垂直偏移量
    shadowEffect3->setBlurRadius(4);  // 模糊半径

    // 应用到不同控件
    ui->prevBtn->setGraphicsEffect(shadowEffect1);
    ui->nextBtn->setGraphicsEffect(shadowEffect2);
    ui->pageEdit->setGraphicsEffect(shadowEffect3);
    setfont();


    ui->listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->listWidget->verticalScrollBar()->setSingleStep(20);
}

trainWindow::~trainWindow()
{
    delete ui;
}

/* ───────── Windows 消息处理 ───────── */
#ifdef Q_OS_WIN
bool trainWindow::nativeEvent(const QByteArray &eventType,
                              void *message,
                              qintptr *result)
{
    if (eventType != "windows_generic_MSG")
        return false;

    MSG *msg = static_cast<MSG *>(message);

    if (msg->message == WM_NCCALCSIZE) {
        *result = 0;
        return true;
    }

    if (msg->message == WM_NCHITTEST) {
        const qreal dpr = devicePixelRatioF();
        const LONG gx = static_cast<LONG>(GET_X_LPARAM(msg->lParam) / dpr);
        const LONG gy = static_cast<LONG>(GET_Y_LPARAM(msg->lParam) / dpr);

        auto hitOnBtn = [](QWidget *w, LONG px, LONG py, int pad = 2) -> bool {
            if (!w) return false;
            QRect r = w->rect().adjusted(-pad, -pad, pad, pad);
            r.moveTopLeft(w->mapToGlobal(r.topLeft()));
            return r.contains(QPoint(px, py));
        };
        if (hitOnBtn(ui->closeButton, gx, gy) ||
            hitOnBtn(ui->minButton,   gx, gy) ||
            hitOnBtn(ui->maxButton,   gx, gy))
            return false;

        QRect safeZone = ui->btnZone->rect();
        safeZone.moveTopLeft(ui->btnZone->mapToGlobal(safeZone.topLeft()));
        if (safeZone.contains(QPoint(gx, gy))) {
            *result = HTCAPTION;
            return true;
        }

        RECT rc; GetWindowRect(msg->hwnd, &rc);
        const QRectF rcL(QPointF(rc.left   / dpr, rc.top    / dpr),
                         QPointF(rc.right  / dpr, rc.bottom / dpr));

        const int bw = m_borderWidth;
        const int th = m_titleBarHeight;

        const bool onLeft   = (gx >= rcL.left()  && gx <  rcL.left()  + bw);
        const bool onRight  = (gx <  rcL.right() && gx >= rcL.right() - bw);
        const bool onTop    = (gy >= rcL.top()   && gy <  rcL.top()   + bw);
        const bool onBottom = (gy <  rcL.bottom()&& gy >= rcL.bottom()- bw);

        *result = HTCLIENT;
        if      (onTop && onLeft)      *result = HTTOPLEFT;
        else if (onTop && onRight)     *result = HTTOPRIGHT;
        else if (onBottom && onLeft)   *result = HTBOTTOMLEFT;
        else if (onBottom && onRight)  *result = HTBOTTOMRIGHT;
        else if (onLeft)               *result = HTLEFT;
        else if (onRight)              *result = HTRIGHT;
        else if (onTop)                *result = HTTOP;
        else if (onBottom)             *result = HTBOTTOM;
        else if (gy >= rcL.top() && gy < rcL.top() + th)
            *result = HTCAPTION;

        return (*result != HTCLIENT);
    }

    return false;
}
#endif

/* ───────── 双击标题栏：最大化 / 还原 ───────── */
void trainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->pos().y() <= m_titleBarHeight) {
        isMaximized() ? showNormal() : showMaximized();
        event->accept();
        return;
    }
    QMainWindow::mouseDoubleClickEvent(event);
}


/* ───────── 首次显示：居中 ───────── */
void trainWindow::showEvent(QShowEvent *event)
{
    static bool first = true;
    if (first) {
        first = false;
        QRect avail = screen()->availableGeometry();
        move(avail.center() - rect().center());
    }
    QMainWindow::showEvent(event);
}


void trainWindow::addTrainCard(const TrainInfo &info)
{
    auto *item = new QListWidgetItem(ui->listWidget);
    item->setFlags(Qt::ItemIsEnabled);
    item->setSizeHint(QSize(ui->listWidget->viewport()->width(), 170));

    auto *card = new trainCard;
    card->setTrainInfo(info);

    ui->listWidget->setItemWidget(item, card);
}


void trainWindow::buildCardPool()
{
    for (int i = 0; i < m_pageSize; ++i) {
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
        item->setFlags(Qt::ItemIsEnabled);
        item->setSizeHint(QSize(ui->listWidget->viewport()->width(), 170));

        auto *card = new trainCard;
        ui->listWidget->setItemWidget(item, card);
        m_cards.append(card);
    }
}


void trainWindow::refreshTrains(const QMap<int, Train>& trains)
{
    m_allTrains.clear();
    m_allTrains.reserve(trains.size());
    for (const Train &tr : trains)
        m_allTrains.append(tr);

    m_currentPage = 0;
    updatePage();
}

void trainWindow::updatePage()
{
    const int total     = m_allTrains.size();
    const int totalPage = qMax(1, (total + m_pageSize - 1) / m_pageSize);

    m_currentPage = qBound(0, m_currentPage, totalPage - 1);

    const int from = m_currentPage * m_pageSize;
    const int to   = qMin(from + m_pageSize, total);

    /* === 写入 / 隐藏卡片 === */
    for (int i = 0; i < m_pageSize; ++i) {
        QListWidgetItem *item = ui->listWidget->item(i);   // 对应行
        trainCard       *card = m_cards[i];

        if (from + i < to) {                               // 有数据
            const Train &tr = m_allTrains[from + i];
            TrainInfo info{ tr.code, tr.id,
                           tr.currentPeople, tr.capacity,
                           tr.loadRate*100.0, tr.lineId,
                           tr.startStation, tr.endStation };
            card->setTrainInfo(info);
            item->setHidden(false);                        // 显示整行
        } else {                                           // 本页无数据
            item->setHidden(true);                         // 连行一起藏
        }
    }

    /* === 更新分页栏（同之前） === */
    ui->prevBtn->setEnabled(m_currentPage > 0);
    ui->nextBtn->setEnabled(m_currentPage + 1 < totalPage);

    ui->pageEdit->blockSignals(true);
    ui->pageEdit->setText(QString("%1 / %2")
                              .arg(m_currentPage + 1)
                              .arg(totalPage));
    ui->pageEdit->blockSignals(false);

    ui->pageEdit->setValidator(new QIntValidator(1, totalPage, ui->pageEdit));
    ui->prevBtn->parentWidget()->setVisible(totalPage > 1);
}


void trainWindow::onPrevPage() { --m_currentPage; updatePage(); }
void trainWindow::onNextPage() { ++m_currentPage; updatePage(); }

void trainWindow::onJumpPage()
{
    QString txt = ui->pageEdit->text().trimmed();
    int slash = txt.indexOf('/');
    if (slash != -1) txt = txt.left(slash);

    bool ok = false;
    int page = txt.toInt(&ok);
    if (ok) {
        m_currentPage = page - 1;
        updatePage();
    }
}


void trainWindow::setfont(){
    int font1 = QFontDatabase::addApplicationFont(":/font/syHei-Bold.otf");
    if(font1==-1){
        qDebug()<<"syHei-Bold.otf加载失败！";
    }
    else{
        QString syHei_Bold = QFontDatabase::applicationFontFamilies(font1).at(0);
        ui->prevBtn->setFont  (QFont(syHei_Bold,14));
        ui->nextBtn->setFont  (QFont(syHei_Bold,14));
        ui->pageEdit->setFont (QFont(syHei_Bold,13));

    }


}




