#ifndef PAGE__MAIN_H
#define PAGE__MAIN_H

#include <QWidget>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QFontDatabase>

#include "gui/1/p0_timeManager.h"
#include "core_system.h"
#include "lineWindow.h"
#include "trainWindow.h"
#include "gui/1/p0_fileManager.h"
#include "mapManager.h"
namespace Ui {
class page0_main;
}

class page0_main : public QWidget
{
    Q_OBJECT

public:
    explicit page0_main(QWidget *parent = nullptr, core_system* s = nullptr);
    ~page0_main();
    void closeSubWindows();

private:
    Ui::page0_main *ui;
    QWebEngineView *webView;

    // 声明时间管理器指针
    p0_timeManager* timeMgr;
    core_system *system;
    lineWindow* line = nullptr;
    trainWindow* train = nullptr;
    p0_fileManager* fileMgr = nullptr;
    mapManager* mapMgr = nullptr;

    void setfont();

private slots:
    void onUserPickedTime(const QDateTime& dt);
    void showLineWindow();
    void showTrainWindow();


};

#endif // PAGE__MAIN_H
