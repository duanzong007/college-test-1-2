#include "page3_main.h"
#include "ui_page3_main.h"
#include <QDebug>
#include "print.h"
#include <QFontDatabase>
#include <QStyle>
#include<QHeaderView>
#include<QtAlgorithms>
#include<QMessageBox>
#include<QGraphicsDropShadowEffect>

// filePath：page3_main.cpp #startLine: 11 #endLine: 255
page3_main::page3_main(QWidget *parent, core_system *sys) :
    QWidget(parent),
    m_system(sys),
    ui(new Ui::page3_main)
{
    ui->setupUi(this);
    connect(ui->log, &QPushButton::clicked, this, &page3_main::showLogWindow);

    m_tableEditTracker = new TableEditTracker(m_system, this);
    m_stackedWidget = ui->charts;
    setshadow();
    // 1. 收集所有标题widget
    m_titles  << ui->linetitle
             << ui->traintitle
             << ui->ordertitle
             << ui->stationtitle;

    for (auto *widget  : m_titles) {
        widget->installEventFilter(this);
        widget->setProperty("selected", false);
    }


    m_dispage = new dis_page(this);//初始化分页器
    m_trainpage = new dis_page(this);
    m_stationpage = new dis_page(this);
    m_linepage = new dis_page(this);



    // 连接信号槽
    connect(m_dispage, &dis_page::pageChanged, this, &page3_main::onPageChanged);
    connect(m_trainpage, &dis_page::pageChanged, this, &page3_main::ontrainChanged);
    connect(m_stationpage, &dis_page::pageChanged, this, &page3_main::onstationChanged);
    connect(m_linepage, &dis_page::pageChanged, this, &page3_main::onlineChanged);

    // 连接按钮到各自分页器
    connect(ui->order_pre, &QPushButton::clicked, m_dispage, &dis_page::prevPage);
    connect(ui->order_next, &QPushButton::clicked, m_dispage, &dis_page::nextPage);
    connect(ui->train_pre, &QPushButton::clicked, m_trainpage, &dis_page::prevPage);
    connect(ui->train_next, &QPushButton::clicked, m_trainpage, &dis_page::nextPage);
    connect(ui->station_pre, &QPushButton::clicked, m_stationpage, &dis_page::prevPage);
    connect(ui->station_next, &QPushButton::clicked, m_stationpage, &dis_page::nextPage);
    connect(ui->line_pre, &QPushButton::clicked, m_linepage, &dis_page::prevPage);
    connect(ui->line_next, &QPushButton::clicked, m_linepage, &dis_page::nextPage);
    // 初始化订单表格
    initOrderTable();
    initTrainTable();
    initStationTable();
    initLineTable();

    if (orderTable) loadOrderData(m_system);
    if (trainTable) loadTrainData(m_system);
    if (stationTable) loadStationData(m_system);
    if (lineTable) loadLineData(m_system);

    //保存设计
    m_editController = new chart_change(this);
    m_editController->setTabletracker(m_tableEditTracker);
    m_editController->registerTable(
        orderTable,
        ui->order_filemanager,  // 文件管理Widget
        ui->order_save_manager,  // 未保存提示Widget
        ui->order_balance,
        ui->order_balance_2,
        ui->order_label
        );

    m_editController->registerTable(
        trainTable,
        ui->train_filemanager,
        ui->train_save_manager,
        ui->train_balance,
        ui->train_balance_2,
        ui->train_label
        );
    m_editController->registerTable(
        lineTable,
        ui->line_filemanager,
        ui->line_save_manager,
        ui->line_balance,
        ui->line_balance_2,
        ui->line_label
        );
    m_editController->registerTable(
        stationTable,
        ui->station_filemanager,
        ui->station_save_manager,
        ui->station_balance,
        ui->station_balance_2,
        ui->station_label
        );

    connect(ui->order_save, &QPushButton::clicked, this, [this]() {
        QTimer::singleShot(100, this, [this]() {
            if (m_tableEditTracker->saveOrderChanges()) {
                if (m_tableEditTracker->saveOrderFile()) {
                    QMessageBox::information(this, "成功", "订单数据保存成功");
                } else {
                    QMessageBox::warning(this, "错误", "订单数据保存失败");
                }
            } else {
                QMessageBox::warning(this, "错误", "没有需要保存的修改");
            }
            m_editController->setEditingEnabled(false, ui->order_filemanager);
        });
        ui->order_balance_2->hide();
        ui->order_label->hide();

    });

    connect(ui->train_save, &QPushButton::clicked, this, [this]() {
        QTimer::singleShot(100, this, [this]() {
            if (m_tableEditTracker->saveTrainChanges()) {
                if (m_tableEditTracker->saveTrainFile()) {
                    QMessageBox::information(this, "成功", "列车数据保存成功");
                } else {
                    QMessageBox::warning(this, "错误", "列车数据保存失败");
                }
            } else {
                QMessageBox::warning(this, "错误", "没有需要保存的修改");
            }
            m_editController->setEditingEnabled(false, ui->train_filemanager);
        });
        ui->train_balance_2->hide();
        ui->train_label->hide();

    });

    connect(ui->station_save, &QPushButton::clicked, this, [this]() {
        QTimer::singleShot(100, this, [this]() {
            if (m_tableEditTracker->saveStationChanges()) {
                if (m_tableEditTracker->saveStationFile()) {
                    QMessageBox::information(this, "成功", "站点数据保存成功");
                } else {
                    QMessageBox::warning(this, "错误", "站点数据保存失败");
                }
            } else {
                QMessageBox::warning(this, "错误", "没有需要保存的修改");
            }
            m_editController->setEditingEnabled(false, ui->station_filemanager);
        });
        ui->station_balance_2->hide();
        ui->station_label->hide();

    });

    connect(ui->line_save, &QPushButton::clicked, this, [this]() {
        // 确保结束所有编辑
        if (lineTable) {
            lineTable->clearFocus();
            lineTable->setCurrentItem(nullptr);
        }
        // 添加延迟确保编辑完全提交
        QTimer::singleShot(100, this, [this]() {
            // 保存修改
            if (m_tableEditTracker->saveLineChanges()) {
                if (m_tableEditTracker->saveLineFlie()) {
                    QMessageBox::information(this, "成功", "线路数据保存成功");
                } else {
                    QMessageBox::warning(this, "错误", "线路数据保存失败");
                }
            } else {
                QMessageBox::warning(this, "错误", "没有需要保存的修改");
            }
            m_editController->setEditingEnabled(false, ui->line_filemanager);
        });

        ui->line_balance_2->hide();
        ui->line_label->hide();

    });


    connect(m_editController, &chart_change::cellEdited, this, [this](QTableWidget* table, int row, int col, const QString& newValue) {
        QString tableType;
        if (table == lineTable) tableType = "lines";
        else if (table == trainTable) tableType = "trains";
        else if (table == stationTable) tableType = "stations";
        else if (table == orderTable) tableType = "orders";

        // 获取当前页数
        int page = 1;
        if (table == lineTable) page = m_linepage->currentPage;
        else if (table == trainTable) page = m_trainpage->currentPage;
        else if (table == stationTable) page = m_stationpage->currentPage;
        else if (table == orderTable) page = m_dispage->currentPage;

        m_tableEditTracker->recordEdit(page, row, col, newValue, tableType);

    });

    m_dispage->setPageLineEdit(ui->order_edit);
    m_trainpage->setPageLineEdit(ui->train_edit);
    m_stationpage->setPageLineEdit(ui->station_edit);
    m_linepage->setPageLineEdit(ui->line_edit);

    //自定义表格交替行

    print *delegate = new print(this);
    if (orderTable) orderTable->setItemDelegate(delegate);
    if (trainTable) trainTable->setItemDelegate(delegate);
    if (stationTable) stationTable->setItemDelegate(delegate);
    if (lineTable) lineTable->setItemDelegate(delegate);

    setFonts();  // 调用字体设置方法

    orderTable->setShowGrid(false);
    orderTable->setFrameShape(QFrame::NoFrame);
    orderTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    orderTable->setStyleSheet(
        R"(
    /* 整张表的外边框 */
    QTableWidget {
        border: none;
        border-left:   1px solid rgba(55,55,55,80);
        border-top:    1px solid rgba(55,55,55,100);
        border-bottom: 1px solid rgba(55,55,55,100);
    }

    /* table 内容单元格分割线 */
    QTableWidget::item {
        color: rgb(55,55,55);
        border: none;
        border-top:   1px solid rgba(55,55,55,100);
        border-right: 1px solid rgba(55,55,55,80);
        padding: 4px;
    }

    /* header 右侧竖线和背景 */
    QHeaderView::section {
        border: none;
        color: rgb(55,55,55);
        background-image: url(:/images/sideBar/black3.png);
        padding: 4px;
    }


)");


    lineTable->setShowGrid(false);
    lineTable->setFrameShape(QFrame::NoFrame);
    lineTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    lineTable->setStyleSheet(
        R"(
    /* 整张表的外边框 */
    QTableWidget {
        border: none;
        border-left:   1px solid rgba(55,55,55,80);
        border-top:    1px solid rgba(55,55,55,100);
        border-bottom: 1px solid rgba(55,55,55,100);
    }

    /* table 内容单元格分割线 */
    QTableWidget::item {
        color: rgb(55,55,55);
        border: none;
        border-top:   1px solid rgba(55,55,55,100);
        border-right: 1px solid rgba(55,55,55,80);
        padding: 4px;
    }

    /* header 右侧竖线和背景 */
    QHeaderView::section {
        border: none;
        color: rgb(55,55,55);
        background-image: url(:/images/sideBar/black3.png);
        padding: 4px;
    }


)");

    stationTable->setShowGrid(false);
    stationTable->setFrameShape(QFrame::NoFrame);
    stationTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    stationTable->setStyleSheet(
        R"(
    /* 整张表的外边框 */
    QTableWidget {
        border: none;
        border-left:   1px solid rgba(55,55,55,80);
        border-top:    1px solid rgba(55,55,55,100);
        border-bottom: 1px solid rgba(55,55,55,100);
    }

    /* table 内容单元格分割线 */
    QTableWidget::item {
        color: rgb(55,55,55);
        border: none;
        border-top:   1px solid rgba(55,55,55,100);
        border-right: 1px solid rgba(55,55,55,80);
        padding: 4px;
    }

    /* header 右侧竖线和背景 */
    QHeaderView::section {
        border: none;
        color: rgb(55,55,55);
        background-image: url(:/images/sideBar/black3.png);
        padding: 4px;
    }


)");

    trainTable->setShowGrid(false);
    trainTable->setFrameShape(QFrame::NoFrame);
    trainTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    trainTable->setStyleSheet(
        R"(
    /* 整张表的外边框 */
    QTableWidget {
        border: none;
        border-left:   1px solid rgba(55,55,55,80);
        border-top:    1px solid rgba(55,55,55,100);
        border-bottom: 1px solid rgba(55,55,55,100);
    }

    /* table 内容单元格分割线 */
    QTableWidget::item {
        color: rgb(55,55,55);
        border: none;
        border-top:   1px solid rgba(55,55,55,100);
        border-right: 1px solid rgba(55,55,55,80);
        padding: 4px;
    }

    /* header 右侧竖线和背景 */
    QHeaderView::section {
        border: none;
        color: rgb(55,55,55);
        background-image: url(:/images/sideBar/black3.png);
        padding: 4px;
    }


)");



    connect(ui->train_pre,  &QPushButton::clicked, this, [this]() {
        m_editController->setEditingEnabled(false, ui->train_filemanager);
    });
    connect(ui->train_next, &QPushButton::clicked, this, [this]() {
        m_editController->setEditingEnabled(false, ui->train_filemanager);
    });
    connect(ui->station_pre,  &QPushButton::clicked, this, [this]() {
        m_editController->setEditingEnabled(false, ui->station_filemanager);
    });
    connect(ui->station_next, &QPushButton::clicked, this, [this]() {
        m_editController->setEditingEnabled(false, ui->station_filemanager);
    });
    connect(ui->order_pre,  &QPushButton::clicked, this, [this]() {
        m_editController->setEditingEnabled(false, ui->order_filemanager);
    });
    connect(ui->order_next, &QPushButton::clicked, this, [this]() {
        m_editController->setEditingEnabled(false, ui->order_filemanager);
    });
    connect(ui->line_pre,  &QPushButton::clicked, this, [this]() {
        m_editController->setEditingEnabled(false, ui->line_filemanager);
    });
    connect(ui->line_next, &QPushButton::clicked, this, [this]() {
        m_editController->setEditingEnabled(false, ui->line_filemanager);
    });

    connect(m_editController, &chart_change::load, this, [this]() {
        loadLineData(m_system);
        loadOrderData(m_system);
        loadStationData(m_system);
        loadTrainData(m_system);
    });

    setSelected(1);

}



//订单表格初始化
void page3_main::initOrderTable()
{
    // 在订单页面创建表格
    QWidget *orderPage = ui->charts->widget(2); // 订单页面索引为2
    if (!orderPage) return;

    QWidget *orderContent = orderPage->findChild<QWidget*>("order_content");
    if (!orderContent) {
        qDebug() << "未找到 order_content 控件";
        return;
    }

    // 创建表格控件
    orderTable = new QTableWidget(orderContent);
    orderTable->setObjectName("orderTable");
    orderTable->verticalHeader()->setVisible(false); // 隐藏行号列

    // 设置表格属性
    orderTable->setColumnCount(10); // 10列
    orderTable->setRowCount(0); // 初始无行
    orderTable->setEditTriggers(QAbstractItemView::NoEditTriggers);//只读
    orderTable->setSelectionBehavior(QAbstractItemView::SelectRows); // 行选择
    orderTable->setAlternatingRowColors(false); // 交替行颜色

    // 设置表头
    QStringList headers;
    headers << "订单ID" << "线路ID" << "列车ID" << "站点ID"
            << "时间" << "上客量" << "下客量" << "票价"
            << "座位类型" << "收入";
    orderTable->setHorizontalHeaderLabels(headers);

    // 自动调整列宽
    orderTable->resizeColumnsToContents();
    // 自动调整行高
    orderTable->resizeRowsToContents();

    orderTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    orderTable->setColumnWidth(4, 200);

    // 其他列平均分配剩余空间
    for (int i = 0; i < 10; ++i) {
        if (i != 4) { // 跳过时间列
            orderTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        }
    }

    // 添加到页面布局
    ui->order_content_layout->addWidget(orderTable);

}

//列车表格初始化
void page3_main::initTrainTable() {
    QWidget *trainPage = ui->charts->widget(1);
    if (!trainPage) return;

    QWidget *trainContent = trainPage->findChild<QWidget*>("train_content");
    if (!trainContent) return;

    trainTable = new QTableWidget(trainContent);
    trainTable->setObjectName("trainTable");
    trainTable->setColumnCount(4);
    trainTable->setRowCount(0);
    trainTable->verticalHeader()->setVisible(false); // 隐藏行号列
    trainTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    trainTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    trainTable->setAlternatingRowColors(false);

    QStringList headers = {"列车编码", "车次号", "是否在途", "列车运量"};
    trainTable->setHorizontalHeaderLabels(headers);
    trainTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->train_content_layout->addWidget(trainTable);

}

// 站点表格初始化
void page3_main::initStationTable() {
    QWidget *stationPage = ui->charts->widget(3);
    if (!stationPage) return;

    QWidget *stationContent = stationPage->findChild<QWidget*>("station_content");
    if (!stationContent) return;

    stationTable = new QTableWidget(stationContent);
    stationTable->setObjectName("stationTable");
    stationTable->setColumnCount(6);
    stationTable->setRowCount(0);
    stationTable->verticalHeader()->setVisible(false); // 隐藏行号列
    stationTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    stationTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    stationTable->setAlternatingRowColors(false);

    QStringList headers = {"站点ID", "站点名称", "是否停运", "车站代码", "车站电报码", "车站简称"};
    stationTable->setHorizontalHeaderLabels(headers);
    stationTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->station_content_layout->addWidget(stationTable);
}

// 线路表格初始化
void page3_main::initLineTable() {
    QWidget *linePage = ui->charts->widget(0);
    if (!linePage) return;

    QWidget *lineContent = linePage->findChild<QWidget*>("line_content");
    if (!lineContent) return;

    lineTable = new QTableWidget(lineContent);
    lineTable->setObjectName("lineTable");
    lineTable->setColumnCount(8);
    lineTable->setRowCount(0);

    lineTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    lineTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    lineTable->setAlternatingRowColors(false);
    lineTable->verticalHeader()->setVisible(false); // 隐藏行号列
    QStringList headers = {"线路编码", "站点ID", "线路站点序号", "上一站", "下一站", "站间距离", "线路代码", "是否停靠"};
    lineTable->setHorizontalHeaderLabels(headers);

    lineTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->line_content_layout->addWidget(lineTable);
}


// 加载列车数据
void page3_main::loadTrainData(core_system* system) {
    if (!trainTable || !system) return;
    trainManager& manager = system->trainManager_;

    const QVector<train>& trains = manager.trains;
    const int total = trains.size();

    // 设置分页信息（不再触发信号）
    m_trainpage->setTotalItems(total);

    const int currentPage = qMax(1, m_trainpage->currentPage);
    const int pageSize = m_trainpage->currentPageSize();
    const int start = (currentPage - 1) * pageSize;
    const int end = qMin(start + pageSize, total);

    trainTable->setRowCount(end-start);
    trainTable->setUpdatesEnabled(false); // 禁用更新提高性能


    for (int i=start,row=0;i<end;i++,row++) {
        const train&t=trains[i];

        trainTable->setItem(row, 0, new QTableWidgetItem(QString::number(t.train_id)));
        trainTable->setItem(row, 1, new QTableWidgetItem(t.train_number));
        trainTable->setItem(row, 2, new QTableWidgetItem(t.is_in_service ? "是" : "否"));
        trainTable->setItem(row, 3, new QTableWidgetItem(QString::number(t.train_capacity)));
    }

    trainTable->setUpdatesEnabled(true);

}

// 加载站点数据
void page3_main::loadStationData(core_system* system) {
    if (!stationTable || !system) return;
    stationManager& manager = system->stationManager_;
    const QVector<station>& stations = manager.stations;
    const int total = stations.size();

    // 设置分页信息（不再触发信号）
    m_stationpage->setTotalItems(total);

    const int currentPage = qMax(1, m_stationpage->currentPage);
    const int pageSize = m_stationpage->currentPageSize();
    const int start = (currentPage - 1) * pageSize;
    const int end = qMin(start + pageSize, total);

    stationTable->setRowCount(end-start);
    stationTable->setUpdatesEnabled(false);
    for (int i=start,row=0;i<end;i++,row++) {
        const station& s = stations[i];

        stationTable->setItem(row, 0, new QTableWidgetItem(QString::number(s.station_id)));
        stationTable->setItem(row, 1, new QTableWidgetItem(s.station_name));
        stationTable->setItem(row, 2, new QTableWidgetItem(s.is_in_service ? "否" : "是")); // 模型中is_in_service表示是否正常运营，此处反转显示
        stationTable->setItem(row, 3, new QTableWidgetItem(s.station_code));
        stationTable->setItem(row, 4, new QTableWidgetItem(s.station_telecode));
        stationTable->setItem(row, 5, new QTableWidgetItem(s.station_shortname));
    }

    stationTable->setUpdatesEnabled(true);
}
// 加载线路数据
void page3_main::loadLineData(core_system* system) {
    if (!lineTable || !system) return;
    lineManager& manager = system->lineManager_;

    // 1. 将数据按线路ID（QMap的key）分组，组内排序
    QVector<QPair<int, line>> sortedLines; // 存储线路ID和对应的line数据
    for (auto it = manager.lines.constBegin(); it != manager.lines.constEnd(); ++it) {
        int lineId = it.key(); // 线路ID来自QMap的key
        QVector<line> linesForId = it.value();
        //排序
        std::sort(linesForId.begin(), linesForId.end(), [](const line& a, const line& b) {
            return a.seq_in_line < b.seq_in_line;
        });
        // 将线路ID和line数据配对存储
        for (const line& l : linesForId) {
            sortedLines.append(qMakePair(lineId, l));
        }
    }

    // 2. 计算总页数（考虑线路ID连续性）
    int totalItems = sortedLines.size();
    int pageSize = m_linepage->currentPageSize(); // 默认200
    int totalPages = 0;
    int currentLineId = -1;
    int itemsInCurrentPage = 0;

    // 第一次遍历：计算总页数
    for (const auto& pair : sortedLines) {
        int lineId = pair.first;
        if (currentLineId != lineId || itemsInCurrentPage >= pageSize) {
            // 新线路或当前页已满，需要新的一页
            totalPages++;
            currentLineId = lineId;
            itemsInCurrentPage = 0;
        }
        itemsInCurrentPage++;
    }

    // 更新分页器的总页数
    m_linepage->setTotalItems(totalItems);
    ui->line_edit->setText(QString("%1/%2").arg(m_linepage->currentPage).arg(totalPages));

    // 3. 获取当前页数据
    int currentPage = m_linepage->currentPage;
    int start = 0;
    int end = 0;
    currentLineId = -1;
    itemsInCurrentPage = 0;
    int pageCounter = 0;

    // 第二次遍历：确定当前页的数据范围
    for (int i = 0; i < sortedLines.size(); ++i) {
        const auto& pair = sortedLines[i];
        int lineId = pair.first;
        if (currentLineId != lineId || itemsInCurrentPage >= pageSize) {
            // 新线路或当前页已满，翻页
            pageCounter++;
            currentLineId = lineId;
            itemsInCurrentPage = 0;
        }
        if (pageCounter == currentPage) {
            if (itemsInCurrentPage == 0) start = i;
            end = i + 1;
        }
        itemsInCurrentPage++;
    }

    // 4. 填充表格
    lineTable->setRowCount(end - start);
    lineTable->setUpdatesEnabled(false);
    for (int i = start, row = 0; i < end; ++i, ++row) {
        const auto& pair = sortedLines[i];
        int lineId = pair.first;
        const line& l = pair.second;

        // 设置表格内容
        lineTable->setItem(row, 0, new QTableWidgetItem(QString::number(lineId))); // 线路ID
        lineTable->setItem(row, 1, new QTableWidgetItem(QString::number(l.station_id)));
        lineTable->setItem(row, 2, new QTableWidgetItem(QString::number(l.seq_in_line)));
        lineTable->setItem(row, 3, new QTableWidgetItem(QString::number(l.prev_station_id)));
        lineTable->setItem(row, 4, new QTableWidgetItem(QString::number(l.next_station_id)));
        lineTable->setItem(row, 5, new QTableWidgetItem(QString::number(l.interstation_distance)));
        lineTable->setItem(row, 6, new QTableWidgetItem(l.line_code));
        lineTable->setItem(row, 7, new QTableWidgetItem(l.is_stop ? "是" : "否"));
    }
    lineTable->setUpdatesEnabled(true);
    m_linepage->setTotalPages(totalPages);
    ui->line_edit->setText(QString("%1/%2").arg(currentPage).arg(totalPages));
}
//加载订单数据
void page3_main::loadOrderData(core_system* system) {
    if (!orderTable || !system) {
        return;
    }

    const orderManager& manager = system->orderManager_;
    const QVector<order>& orders = manager.orders;
    const int total = orders.size();

    // 设置分页信息（不再触发信号）
    m_dispage->setTotalItems(total);

    const int currentPage = qMax(1, m_dispage->currentPage);
    const int pageSize = m_dispage->currentPageSize();
    const int start = (currentPage - 1) * pageSize;
    const int end = qMin(start + pageSize, total);

    orderTable->setRowCount(end-start);
    orderTable->setUpdatesEnabled(false); // 禁用更新提高性能

    for (int i = start, row = 0; i < end; i++, row++) {
        const order& ord = orders[i];

        orderTable->setItem(row, 0, new QTableWidgetItem(QString::number(ord.order_id)));
        orderTable->setItem(row, 1, new QTableWidgetItem(QString::number(ord.line_id)));
        orderTable->setItem(row, 2, new QTableWidgetItem(QString::number(ord.train_id)));
        orderTable->setItem(row, 3, new QTableWidgetItem(QString::number(ord.station_id)));
        orderTable->setItem(row, 4, new QTableWidgetItem(ord.timestamp.toString("yyyy-MM-dd hh:mm")));
        orderTable->setItem(row, 5, new QTableWidgetItem(QString::number(ord.boarding_count)));
        orderTable->setItem(row, 6, new QTableWidgetItem(QString::number(ord.alighting_count)));
        orderTable->setItem(row, 7, new QTableWidgetItem(QString::number(ord.price, 'f', 2)));
        orderTable->setItem(row, 8, new QTableWidgetItem(ord.seat_type_code));
        orderTable->setItem(row, 9, new QTableWidgetItem(QString::number(ord.income, 'f', 2)));
    }

    orderTable->setUpdatesEnabled(true);
}

void page3_main::onPageChanged(int page)
{
    Q_UNUSED(page);
    if (ui->charts->currentIndex() == 2) { // 订单页面
        loadOrderData(m_system);
    }
}

void page3_main::ontrainChanged(int page){
    Q_UNUSED(page);
    if(ui->charts->currentIndex()==1)
    {
        loadTrainData(m_system);
    }

}

void page3_main::onstationChanged(int page){
    Q_UNUSED(page);
    if(ui->charts->currentIndex()==3)
    {
        loadStationData(m_system);
    }

}

void page3_main::onlineChanged(int page){
    Q_UNUSED(page);
    if(ui->charts->currentIndex()==0)
    {
        loadLineData(m_system);
    }
}

page3_main::~page3_main()
{
    delete ui;
    delete m_dispage;
    delete m_trainpage;
    delete m_stationpage;
    delete m_linepage;
}

void page3_main::setFonts()
{
    // 1. 加载Medium字体(表格内容用)
    int fontHeiMedium = QFontDatabase::addApplicationFont(":/font/syHei-Regular.otf");
    if(fontHeiMedium == -1) {
        qDebug() << "syHei-Regular.otf 加载失败！";
    } else {
        QString mediumFamily = QFontDatabase::applicationFontFamilies(fontHeiMedium).at(0);
        QFont mediumFont(mediumFamily, 13);  // 表格内容字号
        QFont mediumFont2(mediumFamily, 16);//标题行的字号
        QFont mediumFont3(mediumFamily, 11);//底部的字号
        setTableFont(mediumFont);
        setbottomfont(mediumFont3);

        ui->text1->setFont(mediumFont2);
        ui->text2->setFont(mediumFont2);
        ui->text3->setFont(mediumFont2);
        ui->text4->setFont(mediumFont2);
        ui->log->setFont(mediumFont2);
    }

    // 2. 先尝试加载Bold字体(表头用)
    int fontHeibold = QFontDatabase::addApplicationFont(":/font/syHei-Medium.otf");
    if(fontHeibold == -1) {
        qDebug() << "syHei-Medium.otf 加载失败！";
    } else {
        QString boldFamily = QFontDatabase::applicationFontFamilies(fontHeibold).at(0);
        QFont boldFont(boldFamily, 14); // 表头字号可以大一些
        setHeaderFont(boldFont);
    }

}

// 设置所有表格内容的字体
void page3_main::setTableFont(const QFont &font)
{
    if (orderTable)   orderTable->setFont(font);
    if (trainTable)   trainTable->setFont(font);
    if (stationTable) stationTable->setFont(font);
    if (lineTable)    lineTable->setFont(font);
}

// 设置所有表头的字体
void page3_main::setHeaderFont(const QFont &font)
{
    if (orderTable){
        orderTable->horizontalHeader()->setFont(font);
    }
    if (trainTable) trainTable->horizontalHeader()->setFont(font);
    if (stationTable) stationTable->horizontalHeader()->setFont(font);
    if (lineTable) lineTable->horizontalHeader()->setFont(font);
}

//底部的字体设置
void page3_main::setbottomfont(const QFont &font){
    ui->order_pre->setFont(font);
    ui->order_next->setFont(font);
    ui->train_pre->setFont(font);
    ui->train_next->setFont(font);
    ui->station_pre->setFont(font);
    ui->station_next->setFont(font);
    ui->line_pre->setFont(font);
    ui->line_next->setFont(font);

    ui->line_save->setFont(font);
    ui->line_label->setFont(font);
    ui->order_save->setFont(font);
    ui->order_label->setFont(font);
    ui->station_save->setFont(font);
    ui->station_label->setFont(font);
    ui->train_save->setFont(font);
    ui->train_label->setFont(font);

    ui->line_edit->setFont(font);
    ui->train_edit->setFont(font);
    ui->station_edit->setFont(font);
    ui->order_edit->setFont(font);

    ui->text6->setFont(font);
    ui->text7->setFont(font);
    ui->text8->setFont(font);
    ui->text9->setFont(font);
}

void page3_main::applyStyle(QWidget *w, bool selected)
{
    w->setProperty("selected", selected);
    w->style()->unpolish(w);
    w->style()->polish(w);
    w->update();
}

// 更新选中状态和切换页面
void page3_main::setSelected(int index)
{
    // 遍历所有导航widget，更新样式
    for (int i = 0; i < m_titles.size(); ++i) {
        applyStyle(m_titles[i], i == index);
    }

    // 切换到对应的页面
    ui->charts->setCurrentIndex(index);
    m_editController->setEditingEnabled(false, ui->line_filemanager);
    m_editController->setEditingEnabled(false, ui->station_filemanager);
    m_editController->setEditingEnabled(false, ui->order_filemanager);
    m_editController->setEditingEnabled(false, ui->train_filemanager);
    // 当切换到订单页时加载数据
    if (m_system) {
        switch (index) {
        case 0: // 线路页面
            if (!m_linesLoaded) loadLineData(m_system);

            break;
        case 1: // 列车页面
            if (!m_trainsLoaded) loadTrainData(m_system);
            break;
        case 2: // 订单页面
            if (!m_ordersLoaded) loadOrderData(m_system);
            break;
        case 3: // 站点页面
            if (!m_stationsLoaded) loadStationData(m_system);
            break;
        }
    }
}

/* ========= 事件过滤器：检测 MouseButtonPress =========== */
bool page3_main::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        int idx = m_titles.indexOf(static_cast<QWidget*>(obj));
        if (idx != -1) {
            setSelected(idx);  // 内部完成样式切换
            return true;       // 吃掉点击事件
        }
    }
    return QWidget::eventFilter(obj, event);

    if (obj == ui->order_edit && event->type() == QEvent::FocusIn) {
        ui->order_edit->selectAll(); // 获得焦点时全选文本
        return true;
    }

    if (obj == ui->train_edit && event->type() == QEvent::FocusIn) {
        ui->train_edit->selectAll(); // 获得焦点时全选文本
        return true;
    }

    if (obj == ui->line_edit && event->type() == QEvent::FocusIn) {
        ui->line_edit->selectAll(); // 获得焦点时全选文本
        return true;
    }

    if (obj == ui->station_edit && event->type() == QEvent::FocusIn) {
        ui->station_edit->selectAll(); // 获得焦点时全选文本
        return true;
    }

    return QWidget::eventFilter(obj, event);
}

void page3_main::setshadow(){

    QList<QWidget*> targets = {
        ui->linetitle,
        ui->line_edit,
        ui->line_filemanager,
        ui->line_pre,
        ui->line_next,
        ui->line_save,
        ui->stationtitle,
        ui->station_edit,
        ui->station_filemanager,
        ui->station_pre,
        ui->station_next,
        ui->station_save,
        ui->ordertitle,
        ui->order_edit,
        ui->order_filemanager,
        ui->order_pre,
        ui->order_next,
        ui->order_save,
        ui->traintitle,
        ui->train_edit,
        ui->train_filemanager,
        ui->train_pre,
        ui->train_next,
        ui->train_save,
        ui->log
    };

    // ② 批量设置阴影
    for (QWidget* w : targets) {
        auto* effect = new QGraphicsDropShadowEffect(this);  // 以窗口为父对象，Qt 会负责回收
        effect->setColor(QColor(0, 0, 0, 30));   // 黑色、30/255 透明
        effect->setOffset(2, 2);                 // 水平 / 垂直偏移
        effect->setBlurRadius(2);                // 模糊半径
        w->setGraphicsEffect(effect);            // 套到控件上
    }


}


void page3_main::closeSubWindows()
{
    if (log)  {
        log->close();
    }
}

void page3_main::showLogWindow()
{
    if (!log) {
        log = new logWindow();
        log->setAttribute(Qt::WA_DeleteOnClose);
    }

    if (log->isMinimized())log->showNormal();
    log->show();
    log->raise();
    log->activateWindow();
}
