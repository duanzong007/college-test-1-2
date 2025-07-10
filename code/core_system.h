#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H
#include "filemanager.h"
#include "file_code/trainManager.h"
#include "file_code/stationManager.h"
#include "file_code/lineManager.h"
#include "file_code/orderManager.h"
#include "algo_code/flow_trainManager.h"
#include "algo_code/flow_stationManager.h"
#include "algo_code/flow_cydiffer.h"
#include "algo_code/ip_check.h"
#include "algo_code/data_preload.h"
#include <memory>

class core_system
{
public:
    fileManager* file;
    trainManager trainManager_;
    stationManager stationManager_;
    lineManager lineManager_;
    orderManager orderManager_;
    ip_check ipManager_;

    flow_trainManager flow_trainManager_;
    flow_stationManager flow_stationManager_;
    flow_cydiffer flow_cydiffer_;
    data_preload data_preload_;


    core_system();
    bool run();
    void refile();


};

#endif // CORE_SYSTEM_H
