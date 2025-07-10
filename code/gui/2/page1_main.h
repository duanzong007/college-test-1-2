#ifndef PAGE1_MAIN_H
#define PAGE1_MAIN_H

#include <QWidget>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QFontDatabase>
#include <QWebEngineView>
#include <QWebEnginePage>
#include "core_system.h"
#include "Page1TimeManager.h"
#include "p1_fileManager.h"
#include "p1_mapManager.h"
namespace Ui {
class page1_main;
}

class page1_main : public QWidget
{
    Q_OBJECT

public:
    explicit page1_main(QWidget *parent = nullptr, core_system* s = nullptr);
    ~page1_main();


private:
    Ui::page1_main    *ui;
    QWebEngineView    *webView;
    Page1TimeManager  *timeMgr;
    p1_fileManager    *fileMgr;
    core_system       *system;
    p1_mapManager     *mapMgr;

    void setfont();
private slots:
    void onUserPickedTime(const QDateTime& dt);
};

#endif // PAGE1_MAIN_H
