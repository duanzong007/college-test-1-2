#ifndef CARD3_H
#define CARD3_H

#include <QWidget>
#include "core_system.h"
#include <QFontDatabase>
#include "predictor.h"
namespace Ui {
class card3;
}

class card3 : public QWidget
{
    Q_OBJECT

public:
    explicit card3(QWidget *parent = nullptr, core_system* s = nullptr);
    ~card3();

private:
    Ui::card3 *ui;
    core_system* system = nullptr;
    void setfont();
};

#endif // CARD3_H
