#include "flow_cydiffer.h"


void flow_cydiffer::runFlowManager(const QMap<QDate, QMap<int, flow_train>> &flow_trains){


    qDebug()<<"开始构建flow_cydiffer";
    flows.clear();

    for(auto it = flow_trains.begin();it!= flow_trains.end();it++){
        const QDate &date = it.key();
        cy_flow a;a.c_to_y=0;a.y_to_c=0;
        flows[date]=a;
        const QMap <int,flow_train> &trains = it.value();
        for(auto &train:trains){

            if(train.line_id==68){
                if(train.direction==Forward){
                    for(auto &k :train.stations){
                        if(k.seq_in_line==1){
                            flows[date].c_to_y+=k.people;
                            break;
                        }
                    }
                }
                else{
                    for(auto &k :train.stations){
                        if(k.seq_in_line==3){
                            flows[date].y_to_c+=k.people;
                            break;
                        }
                    }
                }
            }


            else if(train.line_id==39){
                if(train.direction==Forward){

                    int stseq=0;
                    for(auto &k :train.stations){
                        if(k.seq_in_line<28&&k.seq_in_line>stseq){
                            stseq=k.seq_in_line;
                        }
                    }
                    for(auto &k :train.stations){
                        if(k.seq_in_line==stseq){
                            flows[date].c_to_y+=k.people;
                            break;
                        }
                    }
                }
                else{
                    int stseq=10000;
                    for(auto &k :train.stations){
                        if(k.seq_in_line>26&&k.seq_in_line<stseq){
                            stseq=k.seq_in_line;
                        }
                    }
                    for(auto &k :train.stations){
                        if(k.seq_in_line==stseq){
                            flows[date].y_to_c+=k.people;
                            break;
                        }
                    }
                }
            }


            else if(train.line_id==68){
                if(train.direction==Forward){

                    int stseq=0;
                    for(auto &k :train.stations){
                        if(k.seq_in_line<39&&k.seq_in_line>stseq){
                            stseq=k.seq_in_line;
                        }
                    }
                    for(auto &k :train.stations){
                        if(k.seq_in_line==stseq){
                            flows[date].c_to_y+=k.people;
                            break;
                        }
                    }
                }
                else{
                    int stseq=10000;
                    for(auto &k :train.stations){
                        if(k.seq_in_line>37&&k.seq_in_line<stseq){
                            stseq=k.seq_in_line;
                        }
                    }
                    for(auto &k :train.stations){
                        if(k.seq_in_line==stseq){
                            flows[date].y_to_c+=k.people;
                            break;
                        }
                    }
                }
            }
        }
    }
    set_month_flow();
    set_year_flow();
    qDebug()<<"flow_cydiffer构建完成";

}

void flow_cydiffer::set_month_flow() {
    month_flow.clear();  // 清空原来的月份流量数据

    for (auto it = flows.begin(); it != flows.end(); ++it) {
        const QDate &date = it.key();
        const cy_flow &flow = it.value();

        cy_date monthKey;
        monthKey.year = date.year();
        monthKey.month = date.month();

        if (!month_flow.contains(monthKey)) {
            // 如果该月份的数据还没有，初始化为0
            cy_flow emptyFlow = {0, 0};
            month_flow[monthKey] = emptyFlow;
        }

        // 聚合流量数据
        month_flow[monthKey].c_to_y += flow.c_to_y;
        month_flow[monthKey].y_to_c += flow.y_to_c;
    }

}

void flow_cydiffer::set_year_flow() {
    year_flow.clear();  // 清空原来的年份流量数据

    for (auto it = flows.begin(); it != flows.end(); ++it) {
        const QDate &date = it.key();
        const cy_flow &flow = it.value();

        int yearKey = date.year();  // 提取年份

        if (!year_flow.contains(yearKey)) {
            // 如果该年份的数据还没有，初始化为0
            cy_flow emptyFlow = {0, 0};
            year_flow[yearKey] = emptyFlow;
        }

        // 聚合流量数据
        year_flow[yearKey].c_to_y += flow.c_to_y;
        year_flow[yearKey].y_to_c += flow.y_to_c;
    }

}

void flow_cydiffer::qdebugShow() const {
    // 遍历 flows 中的每个日期和流量数据
    for (auto dateIt = flows.cbegin(); dateIt != flows.cend(); ++dateIt) {
        const QDate &date = dateIt.key();
        const cy_flow &flow = dateIt.value();

        // 输出日期和对应的流量信息
        qDebug() << "日期:" << date.toString("yyyy-MM-dd")
                 << " c_to_y:" << flow.c_to_y
                 << " y_to_c:" << flow.y_to_c;
    }
}

