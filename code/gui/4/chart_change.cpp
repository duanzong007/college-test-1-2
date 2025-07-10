#include "chart_change.h"
#include <QEvent>
#include <QMouseEvent>

chart_change::chart_change(QObject *parent)
    : QObject(parent)
{}

void chart_change::registerTable(
    QTableWidget* table,
    QWidget* fileManagerWidget,
    QWidget* unsavedWarningWidget,
    QWidget* defaultWidget,
    QWidget* balance_2,
    QLabel*  label)  // 新增参数
{
    if (!table || !fileManagerWidget || !unsavedWarningWidget) return;

    QString tableType;
    if (table->objectName() == "lineTable") tableType = "lines";
    else if (table->objectName() == "trainTable") tableType = "trains";
    else if (table->objectName() == "stationTable") tableType = "stations";
    else if (table->objectName() == "orderTable") tableType = "orders";

    // 存储映射关系
    m_fileManagerToTable[fileManagerWidget] = table;
    m_tableToWarningWidget[table] = unsavedWarningWidget;
    m_tableToDefaultWidget[table] = defaultWidget;  // 存储默认控件
    m_tableToBalance_2    [table] = balance_2;
    m_tableToLable        [table] = label;
    m_tableToBool         [table] = false;
    // 初始状态：隐藏未保存提示和默认控件
    unsavedWarningWidget->hide();
    if (defaultWidget) defaultWidget->hide();  // 初始隐藏 defaultWidget
    balance_2->hide();
    label->hide();
    // 安装事件过滤器
    fileManagerWidget->installEventFilter(this);

    // 连接单元格修改信号
    connect(table, &QTableWidget::cellChanged, this, [this, table, tableType](int row, int column) {
        if (m_tableToBool[table] && m_tableToWarningWidget.contains(table)) {
            m_tableToWarningWidget[table]->show();
            if (m_tableToDefaultWidget.contains(table) && m_tableToDefaultWidget[table]) {
                m_tableToDefaultWidget[table]->show();
            }
            m_tableToLable[table]->setPixmap(QPixmap(":/images/page3/no.png"));
            // 记录编辑
            QString newValue = table->item(row, column)->text();
            emit cellEdited(table, row, column, newValue, tableType);
        }
    });
}


void chart_change::setEditingEnabled(bool enabled, QWidget* filemanager)
{

    QTableWidget* table = m_fileManagerToTable[filemanager];
    if (m_tableToBool[table] == enabled) return;

    m_tableToBool[table] = enabled;


    table->setEditTriggers(enabled ?
                               QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed :
                               QAbstractItemView::NoEditTriggers);

    if(enabled){
        m_tableToBalance_2[table]->show();
        m_tableToLable[table]->setPixmap(QPixmap(":/images/page3/yes.png"));
        m_tableToLable[table]->setScaledContents(true);
        m_tableToLable[table]->show();

    }
    // 关闭编辑时：隐藏未保存提示和默认控件
    if (!enabled) {
        m_tableToBalance_2[table]->hide();
        m_tableToLable[table]->hide();
        m_tableToWarningWidget[table]->hide();  // 隐藏 unsavedWarningWidget
        if (m_tableToDefaultWidget.contains(table) && m_tableToDefaultWidget[table]) {
            m_tableToDefaultWidget[table]->hide();  // 隐藏 defaultWidget
        }
        tracker->clear();
    }
}

bool chart_change::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        if (auto* widget = qobject_cast<QWidget*>(watched)) {
            if (m_fileManagerToTable.contains(widget)) {
                qDebug() << "Editing state changed: " << !m_tableToBool[m_fileManagerToTable[widget]];
                setEditingEnabled(!m_tableToBool[m_fileManagerToTable[widget]], widget);
                emit load();
                return true; // 事件已处理
            }
        }
    }
    return QObject::eventFilter(watched, event);
}
