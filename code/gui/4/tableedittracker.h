#ifndef TABLEEDITTRACKER_H
#define TABLEEDITTRACKER_H

#include <QObject>
#include"filemanager.h"
#include "core_system.h"
#include "file_code/lineManager.h"
#include "file_code/orderManager.h"
#include "file_code/stationmanager.h"
#include "file_code/trainManager.h"

struct lineEditRecord {
    int page;
    int row;
    int col;
    QString newValue;//新值
    QString tableType;  // 表格类型
    int lineId = -1;
    int seqInLine = -1;
};

struct stationEditRecord {
    int page;
    int row;
    int col;
    QString newValue;//新值
    QString tableType;  // 表格类型
    int stationId  = -1;
};

struct trainEditRecord {
    int page;
    int row;
    int col;
    QString newValue;//新值
    QString tableType;  // 表格类型
    int trainId  = -1;
};

struct orderEditRecord {
    int page;
    int row;
    int col;
    QString newValue;//新值
    QString tableType;  // 表格类型
    int orderId = -1;
};

class TableEditTracker : public QObject
{
    Q_OBJECT
public:
    explicit TableEditTracker(core_system* system,QObject *parent = nullptr);

    void recordEdit(int page, int row, int col, const QString& newValue, const QString& tableType);


    bool saveLineChanges();
    bool saveLineFlie();
    bool saveTrainChanges();
    bool saveTrainFile();
    bool saveStationChanges();
    bool saveStationFile();
    bool saveOrderChanges();
    bool saveOrderFile();

    void addRecord(const lineEditRecord& record);
    const QList<lineEditRecord>& getRecords() const;
    void clear();

private:
    lineManager *lineMgr;
    trainManager *trainMgr;
    stationManager *stationMgr;
    orderManager *orderMgr;

    fileManager* fileMgr = nullptr;
    core_system* m_system = nullptr;
    QList<lineEditRecord> m_editRecords;
    QList<trainEditRecord> m_trainEditRecords;
    QList<stationEditRecord> m_stationEditRecords;
    QList<orderEditRecord> m_orderEditRecords;
};

#endif // TABLEEDITTRACKER_H
