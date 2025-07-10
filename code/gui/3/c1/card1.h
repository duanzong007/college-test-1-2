#ifndef CARD1_H
#define CARD1_H

#include <QWidget>
#include "core_system.h"
#include "DataCardWidget.h"
namespace Ui {
class card1;
}

class card1 : public QWidget
{
    Q_OBJECT

public:
    explicit card1(QWidget *parent = nullptr, core_system* s = nullptr);
    ~card1();

private:
    Ui::card1 *ui;
    core_system* system = nullptr;
    DataCardWidget* graph1 = nullptr;
    DataCardWidget* graph2 = nullptr;
    void setfont();

private slots:
    void on_duibi_clicked();
};

#endif // CARD1_H
