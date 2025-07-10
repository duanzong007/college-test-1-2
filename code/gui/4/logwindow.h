#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QShowEvent>
#include <QMainWindow>
#include <QMouseEvent>
#include <QScreen>
#include "ui_logwindow.h"
#include "core_system.h"
QT_BEGIN_NAMESPACE
namespace Ui { class logWindow; }
QT_END_NAMESPACE

class logWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit logWindow(QWidget *parent = nullptr);
    ~logWindow() override;
    Ui::logWindow* getUi() { return ui; }

protected:
#ifdef Q_OS_WIN
    // Qt6 / Qt5 共用签名（Qt6 用 qintptr*）
    bool nativeEvent(const QByteArray &eventType, void *message,
                     qintptr *result) override;
#endif
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    Ui::logWindow *ui;
    const int m_borderWidth    = 8;   // 缩放热区
    const int m_titleBarHeight = 35;  // 自定义标题栏高度
    void updateLog();
    void setfont();
};

#endif // LOGWINDOW_H
