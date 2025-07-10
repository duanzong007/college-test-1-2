#include "logwindow.h"
#include "gui/1/page0_main.h"          // 保留原公共接口
#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>
#include <windowsx.h>
#include <QScrollBar>
#ifdef _MSC_VER
#pragma comment(lib, "Dwmapi.lib")
#endif
#endif

#include <QToolButton>

/* ───────── 构造 & 析构 ───────── */
logWindow::logWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::logWindow)
{
    /* ---------- 无边框 + 去掉透明背景 ---------- */
    setWindowFlag(Qt::FramelessWindowHint);
    // setAttribute(Qt::WA_TranslucentBackground);  // 取消透明背景

    ui->setupUi(this);

    /* ---------- 三颗系统按钮 ---------- */
    connect(ui->closeButton, &QToolButton::clicked,
            this, &logWindow::hide);
    connect(ui->minButton,   &QToolButton::clicked,
            this, &logWindow::showMinimized);
    connect(ui->maxButton,   &QToolButton::clicked,
            this, [this]{
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

    /* 启用系统阴影 */
    const MARGINS shadow = {1, 1, 1, 1};  // 添加系统阴影效果
    DwmExtendFrameIntoClientArea(hwnd, &shadow);
#endif

    constexpr int kBaseW = 900;   // 设计稿逻辑尺寸（dpr=1.0）
    constexpr int kBaseH = 600;

    const qreal dpr = devicePixelRatioF();          // 1.25 / 1.5 / 2.0…
    const QSize sz(qRound(kBaseW / dpr),
                   qRound(kBaseH / dpr));

    resize(sz); // 初始窗口大小



    // 初始化定时器，设置每秒更新日志
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &logWindow::updateLog);
    timer->start(1000);  // 每秒更新一次日志
    setfont();
    // 初次加载日志
    updateLog();


}

logWindow::~logWindow()
{
    delete ui;
}

/* ───────── Windows 消息处理 ───────── */
#ifdef Q_OS_WIN
bool logWindow::nativeEvent(const QByteArray &eventType,
                            void *message, qintptr *result)
{
    if (eventType != "windows_generic_MSG")
        return false;               // 仅处理普通 Win 消息

    MSG *msg = static_cast<MSG *>(message);

    /* ---------- 去掉默认非客户区 ---------- */
    if (msg->message == WM_NCCALCSIZE) {
        *result = 0;
        return true;                // 整个窗口区域都归 Qt
    }

    /* ---------- 命中测试：拖动 / 缩放 ---------- */
    if (msg->message == WM_NCHITTEST) {

        const qreal dpr = devicePixelRatioF();      // ① 缩放系数
        const LONG gx = static_cast<LONG>(GET_X_LPARAM(msg->lParam) / dpr);
        const LONG gy = static_cast<LONG>(GET_Y_LPARAM(msg->lParam) / dpr);

        /* ---------- ① 三颗按钮 & 安全区 ---------- */
        auto hitOnBtn = [](QWidget *w, LONG px, LONG py, int pad = 2)->bool{
            if (!w) return false;
            QRect r = w->rect().adjusted(-pad, -pad, pad, pad);
            r.moveTopLeft(w->mapToGlobal(r.topLeft()));
            return r.contains(QPoint(px, py));
        };

        if (hitOnBtn(ui->closeButton, gx, gy) ||
            hitOnBtn(ui->minButton,   gx, gy) ||
            hitOnBtn(ui->maxButton,   gx, gy))
            return false;                           // 给 Qt 处理

        QRect safe = ui->btnZone->rect();
        safe.moveTopLeft(ui->btnZone->mapToGlobal(safe.topLeft()));
        if (safe.contains(QPoint(gx, gy))) {
            *result = HTCAPTION;                    // 仅拖动
            return true;
        }

        /* ---------- ② 将窗口矩形转逻辑坐标 ---------- */
        RECT rc;  GetWindowRect(msg->hwnd, &rc);    // 物理像素
        const QRectF rcL(QPointF(rc.left  / dpr, rc.top    / dpr),
                         QPointF(rc.right / dpr, rc.bottom / dpr));

        const int  bw = m_borderWidth;
        const int  th = m_titleBarHeight;

        const bool onLeft   = (gx >= rcL.left()  && gx <  rcL.left()  + bw);
        const bool onRight  = (gx <  rcL.right() && gx >= rcL.right() - bw);
        const bool onTop    = (gy >= rcL.top()   && gy <  rcL.top()   + bw);
        const bool onBottom = (gy <  rcL.bottom()&& gy >= rcL.bottom()- bw);

        *result = HTCLIENT;                         // 默认命中客户区

        if      (onTop && onLeft)      *result = HTTOPLEFT;
        else if (onTop && onRight)     *result = HTTOPRIGHT;
        else if (onBottom && onLeft)   *result = HTBOTTOMLEFT;
        else if (onBottom && onRight)  *result = HTBOTTOMRIGHT;
        else if (onLeft)               *result = HTLEFT;
        else if (onRight)              *result = HTRIGHT;
        else if (onTop)                *result = HTTOP;
        else if (onBottom)             *result = HTBOTTOM;
        else if (gy >= rcL.top() && gy < rcL.top() + th)
            *result = HTCAPTION;                      // 标题栏拖动

        return (*result != HTCLIENT);
    }

    return false;  // 其余消息不拦截
}
#endif

/* ───────── 双击标题栏：最大化 / 还原 ───────── */
void logWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->pos().y() <= m_titleBarHeight) {
        isMaximized() ? showNormal() : showMaximized();
        event->accept();
        return;
    }
    QMainWindow::mouseDoubleClickEvent(event);
}

void logWindow::showEvent(QShowEvent *e)
{
    static bool firstShow = true;
    if (firstShow) {
        firstShow = false;
        QScreen *scr = screen();
        QRect  avail = scr->availableGeometry();
        move(avail.center() - rect().center());
    }
    QMainWindow::showEvent(e);   // 保留父类实现
}


// 更新日志
void logWindow::updateLog()
{
    /* ---------- ① 刷新前：记录滚动状态 ---------- */
    QScrollBar *sb   = ui->plainTextEdit->verticalScrollBar();
    int distance2Bottom = sb->maximum() - sb->value();   // 到底部还有多少像素
    bool wasAtBottom    = (distance2Bottom == 0);        // 之前是不是就在底部

    /* ---------- ② 读文件并刷新正文 ---------- */
    QFile logFile("logs/log_temp.txt");
    if (!logFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "log open fail:" << logFile.errorString();
        return;
    }
    QTextStream in(&logFile);
    ui->plainTextEdit->setPlainText(in.readAll());       // 重新填充

    /* ---------- ③ 刷新后：恢复滚动位置 ---------- */
    if (wasAtBottom) {
        // 原来就在底部 → 继续粘住底部
        sb->setValue(sb->maximum());
    } else {
        // 原来没在底部 → 大概保持离底部的距离
        int newValue = sb->maximum() - distance2Bottom;
        sb->setValue(qMax(0, newValue));
    }
}


void logWindow::setfont(){
    int font1 = QFontDatabase::addApplicationFont(":/font/syHei-Regular.otf");
    if(font1==-1){
        qDebug()<<"syHei-Regular.otf加载失败！";
    }
    else{
        QString syHei_Regular = QFontDatabase::applicationFontFamilies(font1).at(0);
        ui->plainTextEdit->setFont(QFont(syHei_Regular,12));
    }

}

