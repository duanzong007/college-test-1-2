#include "ip_check.h"

ip_check::ip_check() {}

bool ip_check::inputIp(){
    QFile file("ip.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件ip.csv";
        return false;
    }
    qDebug()<<"开始载入经纬度信息";

    QByteArray byteArray = file.readAll();
    QString content = QString::fromLocal8Bit(byteArray);
    QStringList lines = content.split("\n", Qt::SkipEmptyParts);


    for (auto& line : lines) {
        QStringList cols = line.split(',', Qt::KeepEmptyParts);
        if (cols.size() < 5) {
            qWarning() << "列数不足:" << line;
            continue;
        }

        bool ok = false;
        const int station_id_ = cols[0].trimmed().toInt(&ok);
        if (!ok) {
            qWarning() << "站点ID不是整数:" << cols[0];
            continue;
        }
        const QString station_name_      = cols[1].trimmed();
        const QString station_telecode_  = cols[2].trimmed();
        const QString wei_               = cols[4].trimmed();
        const QString jing_              = cols[3].trimmed();
        double jing1,wei1;
        if(wei_==""||jing_==""){
            jing1=-1; wei1=-1;
        }
        else{
            jing1=jing_.toDouble();
            wei1=wei_.toDouble();
        }
        // 将数据存入
        ip a(station_id_,station_name_,station_telecode_,jing1,wei1);
        ips[station_id_]=a;
    }
    file.close();  // 显式关闭文件
    qDebug()<<"经纬度信息载入成功";
    return true;



}


void ip_check::qdebugShow() const
{
    qDebug() << "=== 开始打印 ips 内容 ===";
    for (auto it = ips.constBegin(); it != ips.constEnd(); ++it) {
        const ip &item = it.value();
        qDebug()
            << QString("站点ID：%1").arg(item.station_id)
            << QString("名称：%1").arg(item.station_name)
            << QString("电报码：%1").arg(item.station_telecode)
            << QString("经度：%1").arg(item.jing)
            << QString("纬度：%1").arg(item.wei);
    }
    qDebug() << QString("=== 打印结束，共 %1 条记录 ===").arg(ips.size());
}
