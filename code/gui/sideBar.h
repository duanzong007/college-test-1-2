#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
#include <QList>
#include "core_system.h"
#include "gui/1/page0_main.h"
#include "gui/2/page1_main.h"
#include "gui/3/page2_main.h"
#include "gui/4/page3_main.h"

QT_BEGIN_NAMESPACE
namespace Ui { class sideBar; }
QT_END_NAMESPACE

namespace Ui {
class sideBar;
}

class sideBar : public QWidget
{
    Q_OBJECT

public:
    explicit sideBar(core_system* sys, QWidget *parent = nullptr);
    ~sideBar();

    page0_main *page0 = nullptr;
    page1_main *page1 = nullptr;
    page2_main *page2 = nullptr;
    page3_main *page3 = nullptr;


protected:
    bool eventFilter(QObject *obj, QEvent *event) override;   // 仍用事件过滤器吃点击

private slots:
    void toggleCollapse();   //< 点击切换
    void onphoto1clicked();
private:
    Ui::sideBar *ui;

    core_system* m_system = nullptr;
    QList<QWidget*> m_buttons;        // 左侧 4 个按钮
    void setSelected(int index);      // 点亮按钮 + 切页
    void applyStyle(QWidget *w, bool selected);
    void setfont();

    void setCollapsed(bool collapse);     //收起功能
    bool m_collapsed = false;


};

#endif // SIDEBAR_H
