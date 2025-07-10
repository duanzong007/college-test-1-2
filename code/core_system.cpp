#include "core_system.h"
#include "algo_code/flow_trainManager.h"
#include <QDebug>


core_system::core_system() {
    qDebug()<<"程序启动！";
}

bool core_system::run(){

    file=new fileManager;
    if(!file->loadAllData())return false;
    if(!trainManager_.convertTrainData    (file->train_file))  return false;
    if(!stationManager_.convertStationData(file->station_file))return false;
    if(!lineManager_.convertLineData      (file->line_file))   return false;
    if(!orderManager_.convertOrderData    (file->order_file))  return false;
    if(!ipManager_.inputIp())                                  return false;

    flow_trainManager_.runFlowManager(lineManager_.lines,orderManager_.orders,stationManager_.stations,trainManager_.trains);
    flow_stationManager_.runFlowManager(orderManager_.orders);
    flow_cydiffer_.runFlowManager(flow_trainManager_.flow_trains);
    data_preload_.runData_preload(&flow_trainManager_,&flow_stationManager_);
    return true;


}

void core_system::refile(){
    delete file;

}

