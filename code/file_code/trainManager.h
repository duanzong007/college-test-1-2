#ifndef TRAINMANAGER_H
#define TRAINMANAGER_H

#include <QString>
#include <QVector>
#include "filemanager.h"
class train
{
public:
    int     train_id;         // 列车id
    QString train_number;     // 车次号
    bool    is_in_service;    // 是否在途
    int     train_capacity;   // 列车运量

    train() = default;
    train(int a,QString b,bool c,int d)
        :train_id(a),train_number(b),is_in_service(c),train_capacity(d){}

};

class trainManager{
public:
    QVector<train> trains;

    trainManager();
     //从filemanager中导入数据，如果trains有数据就返回真值
    bool convertTrainData(const QVector<ftrain>& rawFile);
    //qdebug输出trains中所有内容
    void qdebugShow() const;
};

#endif // TRAINMANAGER_H
