#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gui/1/page0_main.h"
#include "gui/4/page3_main.h"
#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>
#include <windowsx.h>
#ifdef _MSC_VER
#pragma comment(lib, "Dwmapi.lib")
#endif
#endif

#include <QToolButton>

/* ───────── 构造 & 析构 ───────── */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // 无边框 + 透明背景
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    ui->setupUi(this);

    /* 三颗系统按钮 */
    connect(ui->closeButton, &QToolButton::clicked, this, &MainWindow::close);
    connect(ui->minButton,   &QToolButton::clicked, this, &MainWindow::showMinimized);
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
#endif

    constexpr int kBaseW = 1500;   // 你设计稿的逻辑尺寸（基准 dpr = 1.0）
    constexpr int kBaseH =  900;

    const qreal dpr = devicePixelRatioF();  // 例如 1.25 / 1.5 / 2.0
    const QSize sz(qRound(kBaseW / dpr), qRound(kBaseH / dpr));

    resize(sz);                 // 设置初始尺寸

}

MainWindow::~MainWindow()
{
    delete ui;
}

/* ───────── Windows 消息处理 ───────── */
#ifdef Q_OS_WIN
bool MainWindow::nativeEvent(const QByteArray &eventType,
                             void *message,
                             qintptr *result)
{
    if (eventType != "windows_generic_MSG")
        return false;   // 只处理普通 Win 消息

    MSG *msg = static_cast<MSG *>(message);

    /* ---------- 去掉默认非客户区 ---------- */
    if (msg->message == WM_NCCALCSIZE) {
        *result = 0;
        return true;    // 整个窗口区域都归 Qt
    }

    /* ---------- 命中测试：拖动 / 缩放 ---------- */
    if (msg->message == WM_NCHITTEST) {

        const qreal dpr = devicePixelRatioF();                 // ① 缩放系数
        const LONG gx = static_cast<LONG>(GET_X_LPARAM(msg->lParam) / dpr);
        const LONG gy = static_cast<LONG>(GET_Y_LPARAM(msg->lParam) / dpr);

        /* ---------- ① 三颗按钮 & 安全区，逻辑坐标比较 ---------- */
        auto hitOnBtn = [](QWidget *w, LONG px, LONG py, int pad = 2) -> bool {
            if (!w) return false;
            QRect r = w->rect().adjusted(-pad, -pad, pad, pad);
            r.moveTopLeft(w->mapToGlobal(r.topLeft()));
            return r.contains(QPoint(px, py));
        };

        if (hitOnBtn(ui->closeButton, gx, gy) ||
            hitOnBtn(ui->minButton,   gx, gy) ||
            hitOnBtn(ui->maxButton,   gx, gy))
            return false;                                     // 让 Qt 处理

        QRect safe = ui->btnZone->rect();
        safe.moveTopLeft(ui->btnZone->mapToGlobal(safe.topLeft()));
        if (safe.contains(QPoint(gx, gy))) {
            *result = HTCAPTION;                              // 仅拖动
            return true;
        }

        /* ---------- ② 先把 rc 整个转换为“逻辑像素矩形” ---------- */
        RECT rc;  GetWindowRect(msg->hwnd, &rc);              // 物理像素
        const QRectF rcL( QPointF(rc.left   / dpr, rc.top    / dpr),
                         QPointF(rc.right  / dpr, rc.bottom / dpr) );

        const int  bw = m_borderWidth;                        // 逻辑值
        const int  th = m_titleBarHeight;

        const bool onLeft   = (gx >= rcL.left()  && gx <  rcL.left()  + bw);
        const bool onRight  = (gx <  rcL.right() && gx >= rcL.right() - bw);
        const bool onTop    = (gy >= rcL.top()   && gy <  rcL.top()   + bw);
        const bool onBottom = (gy <  rcL.bottom()&& gy >= rcL.bottom()- bw);

        *result = HTCLIENT;                                   // 默认命中

        if      (onTop && onLeft)      *result = HTTOPLEFT;
        else if (onTop && onRight)     *result = HTTOPRIGHT;
        else if (onBottom && onLeft)   *result = HTBOTTOMLEFT;
        else if (onBottom && onRight)  *result = HTBOTTOMRIGHT;
        else if (onLeft)               *result = HTLEFT;
        else if (onRight)              *result = HTRIGHT;
        else if (onTop)                *result = HTTOP;
        else if (onBottom)             *result = HTBOTTOM;
        else if (gy >= rcL.top() && gy < rcL.top() + th)
            *result = HTCAPTION;                              // 标题栏拖动

        return (*result != HTCLIENT);
    }

    return false;  // 其余消息不拦截
}
#endif

/* ───────── 双击标题栏：最大化 / 还原 ───────── */
void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->pos().y() <= m_titleBarHeight) {
        isMaximized() ? showNormal() : showMaximized();
        event->accept();
        return;
    }
    QMainWindow::mouseDoubleClickEvent(event);
}

void MainWindow::showEvent(QShowEvent *e)
{
    static bool firstShow = true;
    if (firstShow) {
        firstShow = false;

        QScreen *scr = screen();
        QRect avail  = scr->availableGeometry();
        move(avail.center() - rect().center());
    }
    QMainWindow::showEvent(e);   // 保留父类实现
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if (auto page1 = findChild<page0_main*>()) {
        page1->closeSubWindows();      // ★ 只调用公共接口
    }
    if (auto page3 = findChild<page3_main*>()) {
        page3->closeSubWindows();      // ★ 只调用公共接口
    }
    QMainWindow::closeEvent(event);
}
