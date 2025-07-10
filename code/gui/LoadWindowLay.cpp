// loadwindowlay.cpp
#include "loadwindowlay.h"
#include "ui_loadwindowlay.h"
#include <QTextCursor>

LoadWindowLay::LoadWindowLay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoadWindowLay)
{
    ui->setupUi(this);


    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    setAttribute(Qt::WA_TranslucentBackground);

    setfont();

    ui->logTextEdit->setReadOnly(true);
}

LoadWindowLay::~LoadWindowLay()
{
    delete ui;
}

void LoadWindowLay::appendMessage(const QString &msg)
{
    // QPlainTextEdit 专用追加方法
    ui->logTextEdit->appendPlainText(msg);
    // 滚动到底部
    QTextCursor c = ui->logTextEdit->textCursor();
    c.movePosition(QTextCursor::End);
    ui->logTextEdit->setTextCursor(c);
    QCoreApplication::processEvents();
}

void LoadWindowLay::setfont(){
    int font1 = QFontDatabase::addApplicationFont(":/font/syHei-Regular.otf");
    if(font1==-1){
        qDebug()<<"syHei-Regular.otf加载失败！";
    }
    else{
        QString syHei_Regular = QFontDatabase::applicationFontFamilies(font1).at(0);
        ui->logTextEdit->setFont(QFont(syHei_Regular,9));
    }

}
