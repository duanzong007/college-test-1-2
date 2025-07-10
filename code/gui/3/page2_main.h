#ifndef PAGE2_MAIN_H
#define PAGE2_MAIN_H

#include <QWidget>
#include "core_system.h"
#include "gui/3/c1/card1.h"
#include "gui/3/c2/card2.h"
#include "gui/3/c3/card3.h"

namespace Ui {
class page2_main;
}

class page2_main : public QWidget
{
    Q_OBJECT

public:
    explicit page2_main(QWidget *parent = nullptr, core_system* s = nullptr);
    ~page2_main();

private:
    Ui::page2_main *ui;
    core_system* system;
    card1* c1 = nullptr;
    card2* c2 = nullptr;
    card3* c3 = nullptr;

};

#endif // PAGE2_MAIN_H
