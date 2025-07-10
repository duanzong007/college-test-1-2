#include "filemanager.h"
#include <QFile>
#include<QThread>
#include<QSaveFile>
#include<QFileInfo>
#include<QTextStream>
#include<QBuffer>
#include<QStringConverter>
#include <QDebug>
#include <QTextStream>
#include <QCoreApplication>
#include <QThread>
#include <QElapsedTimer>

fileManager::fileManager() {
    station_file.reserve(3800);
    train_file.reserve(800);
    line_file.reserve(12000);
    order_file.reserve(510000);
}
fileManager::~fileManager(){
    qDebug()<<"fileManager内存释放";
}
bool fileManager::inputStation(){
    QFile file("station.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件station.csv";
        return false;
    }
    qDebug()<<"station.csv文件打开";

    QByteArray byteArray = file.readAll();
    QString content = QString::fromLocal8Bit(byteArray);
    QStringList lines = content.split("\n", Qt::SkipEmptyParts);

    QTextStream ts(&file);

    for (auto& line : lines) {
        QStringList cols = line.split(',', Qt::KeepEmptyParts);
        if (cols.size() < 16) {
            qWarning() << "列数不足:" << line;
            continue;
        }

        bool ok = false;
        const int zdid_ = cols[0].trimmed().toInt(&ok);
        if (!ok) {
            qWarning() << "站点ID不是整数:" << cols[0];
            continue;
        }
        const QString xlid_              = cols[3].trimmed();
        const QString ysfsbm_            = cols[6].trimmed();
        const QString zdmc_              = cols[7].trimmed();
        const QString sfty_              = cols[11].trimmed();
        const QString station_code_      = cols[13].trimmed();
        const QString station_telecode_  = cols[14].trimmed();
        const QString station_shortname_ = cols[15].trimmed();
        // 将数据存入 station_file
        station_file.push_back({zdid_, xlid_,ysfsbm_,zdmc_, sfty_,station_code_, station_telecode_, station_shortname_});
    }

    file.close();  // 显式关闭文件
    return true;
}

bool fileManager::inputTrain(){
    QFile file("train.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件train.csv";
        return false;
    }
    qDebug()<<"train.csv文件打开";

    QByteArray byteArray = file.readAll();
    QString content = QString::fromLocal8Bit(byteArray);
    QStringList lines = content.split("\n", Qt::SkipEmptyParts);

    QTextStream ts(&file);

    for (auto& line : lines) {
        QStringList cols = line.split(',', Qt::KeepEmptyParts);
        if (cols.size() < 7) {
            qWarning() << "列数不足:" << line;
            continue;
        }

        bool ok = false;
        const int lcbm_ = cols[0].trimmed().toInt(&ok);
        if (!ok) {
            qWarning() << "列车编码不是整数:" << cols[0];
            continue;
        }
        const QString ysfsbm_  = cols[2].trimmed();
        const QString lcdm_    = cols[3].trimmed();
        const QString cc_      = cols[4].trimmed();
        const QString sfzt_    = cols[5].trimmed();
        const QString lcyn_    = cols[6].trimmed();

        train_file.push_back({lcbm_, ysfsbm_, lcdm_, cc_,sfzt_,lcyn_});
    }
    file.close();  // 显式关闭文件
    return true;
}

bool fileManager::inputLine(){
    QFile file("line.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件line.csv";
        return false;
    }
    qDebug()<<"line.csv文件打开";

    QByteArray byteArray = file.readAll();
    QString content = QString::fromLocal8Bit(byteArray);
    QStringList lines = content.split("\n", Qt::SkipEmptyParts);

    QTextStream ts(&file);

    for (auto& line : lines) {
        QStringList cols = line.split(',', Qt::KeepEmptyParts);
        if (cols.size() < 15) {
            qWarning() << "列数不足:" << line;
            continue;
        }

        bool ok = false;
        const int yyxlbm_ = cols[0].trimmed().toInt(&ok);
        if (!ok) {
            qWarning() << "运行线路编码不是整数:" << cols[0];
            continue;
        }
        const int zdid_ = cols[1].trimmed().toInt(&ok);
        if (!ok) {
            qWarning() << "站点ID不是整数:" << cols[1];
            continue;
        }
        const QString xlzdid_   = cols[3].trimmed();
        const QString Q_zdid_   = cols[4].trimmed();
        const QString yqzdjjl_  = cols[5].trimmed();
        const QString H_zdid_   = cols[6].trimmed();
        const QString sfqszd_   = cols[7].trimmed();
        const QString sfzdzd_   = cols[8].trimmed();
        const QString ysjl_     = cols[12].trimmed();
        const QString xldm_     = cols[13].trimmed();
        const QString sfytk_    = cols[14].trimmed();

        line_file.push_back({yyxlbm_, zdid_, xlzdid_,Q_zdid_, yqzdjjl_, H_zdid_,sfqszd_,sfzdzd_,ysjl_,xldm_,sfytk_});
    }
    file.close(); // 显式关闭文件
    return true;
}

bool fileManager::inputOrder(){
    QFile file("order.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件order.csv";
        return false;
    }
    qDebug()<<"order.csv文件打开";

    QByteArray byteArray = file.readAll();
    QString content = QString::fromLocal8Bit(byteArray);
    QStringList lines = content.split("\n", Qt::SkipEmptyParts);

    QTextStream ts(&file);

    for (auto& line : lines) {
        QCoreApplication::processEvents();
        QStringList cols = line.split(',', Qt::KeepEmptyParts);
        if (cols.size() < 39) {
            qWarning() << "列数不足:" << line;
            continue;
        }

        bool ok = false;
        const int xh_ = cols[0].trimmed().toInt(&ok);
        if (!ok) {
            qWarning() << "序号不是整数:" << cols[0];
            continue;
        }
        const int yyxlbm_ = cols[1].trimmed().toInt(&ok);
        if (!ok) {
            qWarning() << "运营线路编码不是整数:" << cols[1];
            continue;
        }
        const int lcbm_ = cols[2].trimmed().toInt(&ok);
        if (!ok) {
            qWarning() << "列车编码不是整数:" << cols[2];
            continue;
        }
        const int zdid_ = cols[3].trimmed().toInt(&ok);
        if (!ok) {
            qWarning() << "站点ID不是整数:" << cols[3];
            continue;
        }
        const QString yxrq_                  = cols[6].trimmed();
        const QString yxsj_                  = cols[7].trimmed();
        const QString yqdzjjl_               = cols[8].trimmed();
        const QString sfqszd_                = cols[9].trimmed();
        const QString sfzdzd_                = cols[10].trimmed();
        const QString ddsj_                  = cols[11].trimmed();
        const QString cfsj_                  = cols[12].trimmed();
        const QString kll_                   = cols[14].trimmed();
        const QString skl_                   = cols[17].trimmed();
        const QString xkl_                   = cols[18].trimmed();
        const QString ticket_type_           = cols[23].trimmed();
        const QString ticket_price_          = cols[24].trimmed();
        const QString seat_type_code_        = cols[25].trimmed();
        const QString train_coporation_code_ = cols[26].trimmed();
        const QString ksrq_                  = cols[27].trimmed();
        const QString start_station_telecode_= cols[28].trimmed();
        const QString start_station_name_    = cols[29].trimmed();
        const QString end_station_telecode_  = cols[30].trimmed();
        const QString end_station_name_      = cols[31].trimmed();
        const QString train_class_code_      = cols[32].trimmed();
        const QString train_type_code_       = cols[33].trimmed();
        const QString sale_station_telecode_ = cols[34].trimmed();
        const QString limit_station_telecode_= cols[35].trimmed();
        const QString spsj_                  = cols[36].trimmed();
        const QString to_station_telecode_   = cols[37].trimmed();
        const QString shouru_                = cols[38].trimmed();

        order_file.push_back({
            xh_, yyxlbm_, lcbm_, zdid_, yxrq_,
            yxsj_, yqdzjjl_, sfqszd_, sfzdzd_,ddsj_,
            cfsj_, kll_, skl_, xkl_, ticket_type_,
            ticket_price_, seat_type_code_,train_coporation_code_, ksrq_, start_station_telecode_,
            start_station_name_,end_station_telecode_, end_station_name_, train_class_code_, train_type_code_,
            sale_station_telecode_, limit_station_telecode_, spsj_, to_station_telecode_,shouru_
        });


    }

    return true;
}

void fileManager::debugShowStation(){
    for(auto& a:station_file){
        qDebug()<<
            a.zdid<<
            a.xlid<<
            a.ysfsbm<<
            a.zdmc<<
            a.sfty<<
            a.station_code<<
            a.station_telecode<<
            a.station_shortname;
    }
}

void fileManager::debugShowTrain(){
    for(auto& a:train_file){
        qDebug()<<
            a.lcbm<<
            a.ysfsbm<<
            a.lcdm<<
            a.cc<<
            a.sfzt<<
            a.lcyn;
    }

}

void fileManager::debugShowLine(){
    for (auto& a : line_file) {
        qDebug() <<
            a.yyxlbm <<
            a.zdid <<
            a.xlzdid <<
            a.Q_zdid <<
            a.yqzdjjl <<
            a.H_zdid <<
            a.sfqszd <<
            a.sfzdzd <<
            a.ysjl <<
            a.xldm <<
            a.sfytk;
    }
}

void fileManager::debugShowOrder(){
    int i=0;
    for (auto it=order_file.begin();i<10;i++,it++) {
        forder a=*it;
        qDebug() <<
            a.xh <<
            a.yyxlbm <<
            a.lcbm <<
            a.zdid <<
            a.yxrq <<
            a.yxsj <<
            a.yqdzjjl <<
            a.sfqszd <<
            a.sfzdzd <<
            a.ddsj <<
            a.cfsj <<
            a.kll <<
            a.skl <<
            a.xkl <<
            a.ticket_type <<
            a.ticket_price <<
            a.seat_type_code <<
            a.train_coporation_code <<
            a.ksrq <<
            a.start_station_telecode <<
            a.start_station_name <<
            a.end_station_telecode <<
            a.end_station_name <<
            a.train_class_code <<
            a.train_type_code <<
            a.sale_station_telecode <<
            a.limit_station_telecode <<
            a.spsj <<
            a.to_station_telecode <<
            a.shouru;
    }
}

bool fileManager::loadAllData(){
    if(this->inputLine()&&this->inputStation()&&this->inputTrain()&&this->inputOrder()){
        qDebug()<<"所有数据输入成功！";
        return true;

    }
    return false;
}


bool fileManager::saveLines(const QString& tempPath) {
    QString filename = "line.csv"; // 目标文件名
    QString tempFile = "line.tmp"; // 临时文件名
    QString backupFile = filename + ".bak"; // 备份文件名

    // 1. 写入临时文件（使用QSaveFile自动保证原子性）
    {
        QSaveFile file(tempFile); // 使用QSaveFile替代QFile
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qCritical() << "无法创建临时文件:" << file.errorString();
            return false;
        }

        QTextStream out(&file);
         out.setEncoding(QStringConverter::Encoding::System);
        // 表头（与你原始文件完全一致）
        out << "yyxlbm,zdid,,xlzdid,Q_zdid,yqzdjjl,H_zdid,,,,,,ysjl,xldm,sfytk\n";

        // 数据写入（保持你原有的列结构）
        for (const auto& line : line_file) {
            out << line.yyxlbm << ","
                << line.zdid << ","
                << "," // 空列3
                << line.xlzdid << ","
                << line.Q_zdid << ","
                << line.yqzdjjl << ","
                << line.H_zdid << ","
                << "0,0,0,0,0," // 第8-12列
                << "0," // ysjl
                << line.xldm << ","
                << line.sfytk << "\n";
        }

        // 确保所有数据写入磁盘
        if (!file.commit()) {
            qCritical() << "临时文件提交失败:" << file.errorString();
            return false;
        }
    }

    // 2. 备份原文件（如果存在）
    if (QFile::exists(filename)) {
        // 先删除旧备份（如果有）
        if (QFile::exists(backupFile)) {
            int retryCount = 3;
            while (retryCount-- > 0) {
                // 每次循环都创建新的QFile对象
                QFile backup(backupFile);
                if (backup.remove()) break;

                // 现在可以正确获取错误信息
                qDebug() << "删除旧备份失败，剩余重试次数:" << retryCount
                         << "错误:" << backup.errorString();

                if (retryCount > 0) {
                    QThread::msleep(100);
                } else {
                    qCritical() << "无法删除旧备份文件，最终错误:" << backup.errorString();
                    return false;
                }
            }
        }

        // 重命名原文件为备份
        if (!QFile::rename(filename, backupFile)) {
            qCritical() << "备份原文件失败:" ;
            return false;
        }
    }

    // 3. 重命名临时文件为目标文件
    if (!QFile::rename(tempFile, filename)) {
        qCritical() << "文件替换失败，尝试恢复...";

        // 尝试恢复备份
        if (QFile::exists(backupFile)) {
            QFile::rename(backupFile, filename);
        }
        return false;
    }

    // 4. 最终验证
    if (!QFile::exists(filename) || QFile(filename).size() == 0) {
        qCritical() << "文件验证失败";
        if (QFile::exists(backupFile)) {
            QFile::rename(backupFile, filename);
        }
        return false;
    }

    qDebug() << "文件保存成功：" << filename;
    return true;
}

bool fileManager::saveTrains(const QString& tempPath ){
    QString filename = "train.csv";
    QString tempFile = "train.tmp";
    QString backupFile = filename + ".bak";

    {
        QSaveFile file(tempFile);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qCritical() << "无法创建临时文件:" << file.errorString();
            return false;
        }

        QTextStream out(&file);
         out.setEncoding(QStringConverter::Encoding::System);
        // 表头
        out << "lcbm,sxxbm,ysfsbm,lcdm,cc,sfzt,lcyn\n";

        // 数据写入
        for (const auto& train : train_file) {
            out << train.lcbm << ","
                <<"0"<<","
                << "1" << ","
                << train.lcdm << ","
                << train.cc << ","
                << "1" << ","
                << train.lcyn << "\n";
        }

        if (!file.commit()) {
            qCritical() << "临时文件提交失败:" << file.errorString();
            return false;
        }
    }
    // 2. 备份原文件（如果存在）
    if (QFile::exists(filename)) {
        // 先删除旧备份（如果有）
        if (QFile::exists(backupFile)) {
            int retryCount = 3;
            while (retryCount-- > 0) {
                // 每次循环都创建新的QFile对象
                QFile backup(backupFile);
                if (backup.remove()) break;

                // 现在可以正确获取错误信息
                qDebug() << "删除旧备份失败，剩余重试次数:" << retryCount
                         << "错误:" << backup.errorString();

                if (retryCount > 0) {
                    QThread::msleep(100);
                } else {
                    qCritical() << "无法删除旧备份文件，最终错误:" << backup.errorString();
                    return false;
                }
            }
        }

        // 重命名原文件为备份
        if (!QFile::rename(filename, backupFile)) {
            qCritical() << "备份原文件失败:" ;
            return false;
        }
    }

    // 3. 重命名临时文件为目标文件
    if (!QFile::rename(tempFile, filename)) {
        qCritical() << "文件替换失败，尝试恢复...";

        // 尝试恢复备份
        if (QFile::exists(backupFile)) {
            QFile::rename(backupFile, filename);
        }
        return false;
    }

    // 4. 最终验证
    if (!QFile::exists(filename) || QFile(filename).size() == 0) {
        qCritical() << "文件验证失败";
        if (QFile::exists(backupFile)) {
            QFile::rename(backupFile, filename);
        }
        return false;
    }

    qDebug() << "文件保存成功：" << filename;
    return true;
}

bool fileManager::saveStations(const QString& tempPath) {
    QString filename = "station.csv";
    QString tempFile = "station.tmp";
    QString backupFile = filename + ".bak";

    {
        QSaveFile file(tempFile);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qCritical() << "无法创建临时文件:" << file.errorString();
            return false;
        }

        QTextStream out(&file);
         out.setEncoding(QStringConverter::Encoding::System);
        // 表头
        out << "zdid,,,xlid,,,ysfsbm,zdmc,,,,sfty,,station_code,station_telecode,station_shortname\n";

        // 数据写入
        for (const auto& station : station_file) {
            out << station.zdid << ",0,0,"
                << station.xlid << ",0,0,"
                << station.ysfsbm << ","
                << station.zdmc << ",0,0,0,"
                << station.sfty << ",0,"
                << station.station_code << ","
                << station.station_telecode << ","
                << station.station_shortname << "\n";
        }

        if (!file.commit()) {
            qCritical() << "临时文件提交失败:" << file.errorString();
            return false;
        }
    }

    // 2. 备份原文件（如果存在）
    if (QFile::exists(filename)) {
        // 先删除旧备份（如果有）
        if (QFile::exists(backupFile)) {
            int retryCount = 3;
            while (retryCount-- > 0) {
                // 每次循环都创建新的QFile对象
                QFile backup(backupFile);
                if (backup.remove()) break;

                // 现在可以正确获取错误信息
                qDebug() << "删除旧备份失败，剩余重试次数:" << retryCount
                         << "错误:" << backup.errorString();

                if (retryCount > 0) {
                    QThread::msleep(100);
                } else {
                    qCritical() << "无法删除旧备份文件，最终错误:" << backup.errorString();
                    return false;
                }
            }
        }

        // 重命名原文件为备份
        if (!QFile::rename(filename, backupFile)) {
            qCritical() << "备份原文件失败:" ;
            return false;
        }
    }

    // 3. 重命名临时文件为目标文件
    if (!QFile::rename(tempFile, filename)) {
        qCritical() << "文件替换失败，尝试恢复...";

        // 尝试恢复备份
        if (QFile::exists(backupFile)) {
            QFile::rename(backupFile, filename);
        }
        return false;
    }

    // 4. 最终验证
    if (!QFile::exists(filename) || QFile(filename).size() == 0) {
        qCritical() << "文件验证失败";
        if (QFile::exists(backupFile)) {
            QFile::rename(backupFile, filename);
        }
        return false;
    }

    qDebug() << "文件保存成功：" << filename;
    return true;
}


bool fileManager::saveOrders(const QString& filename) {
    QElapsedTimer timer;
    timer.start();
    QString targetFile = "order.csv"; // 强制使用固定文件名
    const QString tempFile = filename + ".tmp";
    const QString backupFile = filename + ".bak";

    // 1. 使用缓冲写入临时文件（大幅减少 I/O 操作）
    {
        QSaveFile file(tempFile);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qCritical() << "无法创建临时文件:" << file.errorString();
            return false;
        }

        QTextStream out(&file);
        out.setEncoding(QStringConverter::System); // ANSI 编码

        // 1.1 预分配缓冲区（减少内存碎片）
        const int batchSize = 10000;  // 每批写入 1万行
        QString buffer;
        buffer.reserve(batchSize * 150);  // 预分配缓冲区（假设每行约150字符）

        // 1.2 写入表头
        out << "xh,yyxlbm,lcbm,zdid,xlzdid,sxxbm,yxrq,yxsj,yqdzjjl,sfqszd,sfzdzd,ddsj,cfsj,jtsj,kll,sxkll,xxkll,skl,xkl,bz,lccfrq,lccsj,zdxh,ticket_type,ticket_price,seat_type_code,train_coporation_code,ksrq,start_station_telecode,start_station_name,end_station_telecode,end_station_name,train_class_code,train_type_code,sale_station_telecode,limit_station_telecode,spsj,to_station_telecode,shouru\n";

        // 1.3 分批写入数据（避免单次循环过长）
        for (const auto& order : order_file) {
            out << order.xh << ","
                << order.yyxlbm << ","
                << order.lcbm << ","
                << order.zdid << ","
                << "NULL" << ","
                << "NULL" << ","
                << order.yxrq << ","
                << order.yxsj << ","
                << order.yqdzjjl << ","
                << order.sfqszd << ","
                << order.sfzdzd << ","
                << order.ddsj << ","
                << order.cfsj << ","
                << "NULL" << ","
                << order.kll << ","
                << "NULL" << ","
                << "NULL" << ","
                << order.skl << ","
                << order.xkl << ","
                << "NULL" << "," << "NULL" << "," << "NULL" << "," << "NULL" << ","
                << order.ticket_type << ","
                << order.ticket_price << ","
                << order.seat_type_code << ","
                << order.train_coporation_code << ","
                << order.ksrq << ","
                << order.start_station_telecode << ","
                << order.start_station_name << ","
                << order.end_station_telecode << ","
                << order.end_station_name << ","
                << order.train_class_code << ","
                << order.train_type_code << ","
                << order.sale_station_telecode << ","
                << order.limit_station_telecode << ","
                << order.spsj << ","
                << order.to_station_telecode << ","
                << order.shouru << "\n";
            QCoreApplication::processEvents();
        }

        if (!file.commit()) {
            qCritical() << "临时文件提交失败:" << file.errorString();
            return false;
        }

        qDebug() << "临时文件写入完成:" << tempFile << "大小:" << QFileInfo(tempFile).size() << "字节";
    }

    // 3. 备份原文件
    if (QFile::exists(targetFile)) {
        // 删除旧备份
        if (QFile::exists(backupFile)) {
            if (!QFile::remove(backupFile)) {
                qCritical() << "无法删除旧备份文件:" << backupFile;
                QFile::remove(tempFile); // 清理临时文件
                return false;
            }
        }

        // 重命名原文件为备份
        if (!QFile::rename(targetFile, backupFile)) {
            qCritical() << "无法备份原文件:" << targetFile << "->" << backupFile;
            QFile::remove(tempFile); // 清理临时文件
            return false;
        }
        qDebug() << "原文件备份成功:" << backupFile;
    }

    // 4. 重命名临时文件为目标文件
    if (!QFile::rename(tempFile, targetFile)) {
        qCritical() << "无法重命名临时文件:" << tempFile << "->" << targetFile;

        // 尝试恢复备份
        if (QFile::exists(backupFile)) {
            QFile::rename(backupFile, targetFile);
            qWarning() << "已恢复备份文件";
        }
        return false;
    }

    qDebug() << "文件保存成功:" << targetFile << "大小:" << QFileInfo(targetFile).size() << "字节";


    return true;
}
