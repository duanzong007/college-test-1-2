#ifndef CARD2_H
#define CARD2_H

#include <QWidget>
#include "core_system.h"
#include "CYCardWidget.h"
namespace Ui {
class card2;
}

class card2 : public QWidget
{
    Q_OBJECT

public:
    explicit card2(QWidget *parent = nullptr, core_system* s = nullptr);
    ~card2();

private:
    Ui::card2 *ui;
    core_system* system = nullptr;
    CYCardWidget* graph1 = nullptr;
    CYCardWidget* graph2 = nullptr;
    void setfont();

private slots:
    void on_duibi_clicked();
};

#endif // CARD2_H
