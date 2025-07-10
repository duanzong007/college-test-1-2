// loadwindowlay.h
#ifndef LOADWINDOWLAY_H
#define LOADWINDOWLAY_H

#include <QWidget>
#include <qfontdatabase>
namespace Ui {
class LoadWindowLay;
}

class LoadWindowLay : public QWidget
{
    Q_OBJECT

public:
    explicit LoadWindowLay(QWidget *parent = nullptr);
    ~LoadWindowLay();

    // 外部调用，用来追加一行日志
    void appendMessage(const QString &msg);

private:
    Ui::LoadWindowLay *ui;
    void setfont();
};

#endif // LOADWINDOWLAY_H
