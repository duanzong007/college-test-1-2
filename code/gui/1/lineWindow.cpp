#include "linewindow.h"
#include "ui_linewindow.h"

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
lineWindow::lineWindow(p0_fileManager* mgr, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::lineWindow)
    , m_fileMgr(mgr)
{
    setWindowFlag(Qt::FramelessWindowHint);
    // setAttribute(Qt::WA_TranslucentBackground);  // 如需透明再放开

    ui->setupUi(this);

    /* 三颗系统按钮 */
    connect(ui->closeButton, &QToolButton::clicked, this, &QWidget::hide);
    connect(ui->minButton,   &QToolButton::clicked, this, &lineWindow::showMinimized);
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

    /* 若需要阴影，可保留 */
    const MARGINS shadow = {1,1,1,1};
    DwmExtendFrameIntoClientArea(hwnd, &shadow);
#endif

    /* -------- 根据 DPI 适配初始尺寸 -------- */
    constexpr int kBaseW = 500;   // 设计稿逻辑尺寸
    constexpr int kBaseH = 900;

    const qreal dpr = devicePixelRatioF();
    const QSize sz(qRound(kBaseW / dpr), qRound(kBaseH / dpr));
    resize(sz);
    ui->listWidget->setVerticalScrollMode(QListWidget::ScrollPerPixel);


    buildCardPool();                                         // ★ 一次性生成 10 张卡片

    connect(ui->prevBtn, &QPushButton::clicked,
            this, &lineWindow::onPrevPage);
    connect(ui->nextBtn, &QPushButton::clicked,
            this, &lineWindow::onNextPage);

    /* 输入框里按 Enter 触发跳页 */
    connect(ui->pageEdit, &QLineEdit::returnPressed,
            this, &lineWindow::onJumpPage);

    /* 禁止粘贴非数字 */
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
    connect(ui->cancel,&QPushButton::clicked,this,&lineWindow::cancalChoose);
}

lineWindow::~lineWindow()
{
    delete ui;
}

/* ───────── Windows 消息处理 ───────── */
#ifdef Q_OS_WIN
bool lineWindow::nativeEvent(const QByteArray &eventType,
                             void *message,
                             qintptr *result)
{
    if (eventType != "windows_generic_MSG")
        return false;

    MSG *msg = static_cast<MSG *>(message);

    /* 去掉默认非客户区 */
    if (msg->message == WM_NCCALCSIZE) {
        *result = 0;
        return true;
    }

    /* 命中测试：拖动 / 缩放 */
    if (msg->message == WM_NCHITTEST) {
        const qreal dpr = devicePixelRatioF();                  // 缩放系数
        const LONG gx = static_cast<LONG>(GET_X_LPARAM(msg->lParam) / dpr);
        const LONG gy = static_cast<LONG>(GET_Y_LPARAM(msg->lParam) / dpr);

        /* 1) 按钮、标题栏安全区 */
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

        /* 2) 其他区域：系统缩放 / 拖动 */
        RECT rc; GetWindowRect(msg->hwnd, &rc);                 // 物理像素
        const QRectF rcL(QPointF(rc.left   / dpr, rc.top    / dpr),
                         QPointF(rc.right  / dpr, rc.bottom / dpr));

        const int bw = m_borderWidth;     // 逻辑像素
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
void lineWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->pos().y() <= m_titleBarHeight) {
        isMaximized() ? showNormal() : showMaximized();
        event->accept();
        return;
    }
    QMainWindow::mouseDoubleClickEvent(event);
}

/* ───────── 首次显示：居中 ───────── */
void lineWindow::showEvent(QShowEvent *event)
{
    static bool first = true;
    if (first) {
        first = false;
        QRect avail = screen()->availableGeometry();           // 逻辑坐标
        move(avail.center() - rect().center());
    }
    QMainWindow::showEvent(event);     // 保留父类实现
}


// ───── 新增函数：向 QListWidget 塞入一张卡片
void lineWindow::addLineCard(const LineInfo &info)
{
    // 1) 创建 QListWidgetItem 占位
    QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
    item->setFlags(Qt::ItemIsEnabled);          // 不可选中
    item->setSizeHint(QSize(ui->listWidget->width() - 32, 150));

    // 2) 创建真正的卡片 QWidget
    auto *card = new lineCard;
    card->setLineInfo(info);

    // 3) 转发勾选信号（按需处理）
    connect(card, &lineCard::visibilityChanged,
            m_fileMgr, &p0_fileManager::onLineVisibilityChanged,
            Qt::UniqueConnection);


    // 4) 放进列表
    ui->listWidget->setItemWidget(item, card);
}


void lineWindow::refreshLines(const QMap<int, Line>& lines)
{
    m_allLines.clear();
    m_allLines.reserve(lines.size());
    for (const Line& ln : lines)
        m_allLines.append(ln);

    m_currentPage = 0;
    updatePage();                // 填充第一页
}


void lineWindow::buildCardPool()
{
    for (int i = 0; i < m_pageSize; ++i) {
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
        item->setFlags(Qt::ItemIsEnabled);
        item->setSizeHint(QSize(ui->listWidget->width() - 32, 150));

        auto *card = new lineCard;
        ui->listWidget->setItemWidget(item, card);

        /* 连接勾选信号到 fileManager（只连一次） */
        connect(card, &lineCard::visibilityChanged,
                m_fileMgr, &p0_fileManager::onLineVisibilityChanged,
                Qt::UniqueConnection);

        connect(card, &lineCard::visibilityChanged,
                this, [this](int id, bool vis){          // λ 同步本地缓存
                    for (Line &ln : m_allLines)
                        if (ln.id == id) { ln.visible = vis; break; }
                });

        m_cards.append(card);
    }
}

void lineWindow::updatePage()
{
    const int total     = m_allLines.size();
    const int totalPage = qMax(1, (total + m_pageSize - 1) / m_pageSize);

    m_currentPage = qBound(0, m_currentPage, totalPage - 1);

    const int from = m_currentPage * m_pageSize;
    const int to   = qMin(from + m_pageSize, total);

    /* === 把数据写进 10 张卡片 === */
    for (int i = 0; i < m_pageSize; ++i) {
        lineCard *card = m_cards[i];
        if (from + i < to) {
            const Line &ln = m_allLines[from + i];
            LineInfo info{ ln.id, ln.capacityMatch,
                          ln.startStation, ln.endStation,
                          ln.color, ln.visible };
            card->setLineInfo(info);
            card->show();
        } else {
            card->hide();
        }
    }

    /* === 更新分页栏 === */
    ui->prevBtn->setEnabled(m_currentPage > 0);
    ui->nextBtn->setEnabled(m_currentPage + 1 < totalPage);

    /* 页码显示 “当前 / 总” */
    ui->pageEdit->blockSignals(true);                               // 避免 setText 触发 returnPressed
    ui->pageEdit->setText(QString("%1 / %2")
                              .arg(m_currentPage + 1).arg(totalPage));
    ui->pageEdit->blockSignals(false);

    /* 为输入框设置合法范围校验器 (1~totalPage) */
    ui->pageEdit->setValidator(new QIntValidator(1, totalPage, ui->pageEdit));

    /* 若只有一页则隐藏整行分页栏 */
    ui->prevBtn->parentWidget()->setVisible(totalPage > 1);
}


void lineWindow::onPrevPage() { --m_currentPage; updatePage(); }
void lineWindow::onNextPage() { ++m_currentPage; updatePage(); }

void lineWindow::onJumpPage()
{
    QString txt = ui->pageEdit->text().trimmed();
    int slashPos = txt.indexOf('/');
    if (slashPos != -1) txt = txt.left(slashPos);

    bool ok = false;
    int page = txt.toInt(&ok);
    if (ok) {
        m_currentPage = page - 1;   // 1→0 基
        updatePage();
    }
}


void lineWindow::setfont(){
    int font1 = QFontDatabase::addApplicationFont(":/font/syHei-Bold.otf");
    if(font1==-1){
        qDebug()<<"syHei-Bold.otf加载失败！";
    }
    else{
        QString syHei_Bold = QFontDatabase::applicationFontFamilies(font1).at(0);
        ui->prevBtn->setFont  (QFont(syHei_Bold,14));
        ui->nextBtn->setFont  (QFont(syHei_Bold,14));
        ui->pageEdit->setFont (QFont(syHei_Bold,13));
        ui->cancel->setFont   (QFont(syHei_Bold,14));

    }


}
