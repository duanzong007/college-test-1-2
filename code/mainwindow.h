#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QShowEvent>
#include <QMainWindow>
#include <QMouseEvent>
#include <QScreen>
#include "ui_mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    Ui::MainWindow* getUi() { return ui; }

protected:
#ifdef Q_OS_WIN
    // Qt6 / Qt5 共用签名（Qt6 用 qintptr*）
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
#endif
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
private:
    Ui::MainWindow *ui;
    const int m_borderWidth    = 8;   // 缩放热区
    const int m_titleBarHeight = 35;  // 自定义标题栏高度
};

#endif // MAINWINDOW_H
