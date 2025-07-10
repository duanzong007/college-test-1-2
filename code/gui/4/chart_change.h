#ifndef CHART_CHANGE_H
#define CHART_CHANGE_H

#include <QObject>
#include <QTableWidget>
#include <QHash>
#include <QWidget>
#include <QSet>
#include <QMap>
#include <QLabel>
#include "tableedittracker.h"
class chart_change : public QObject
{
    Q_OBJECT
public:
    explicit chart_change(QObject *parent = nullptr);

    // 注册表格和管理组件
    void registerTable(QTableWidget* table,
                       QWidget* fileManagerWidget,
                       QWidget* unsavedWarningWidget,
                       QWidget* defaultWidget = nullptr,
                       QWidget* balance_2 = nullptr,
                       QLabel*  label = nullptr);

    // 编辑状态控制
    void setEditingEnabled(bool enabled, QWidget*);
    bool isEditing() const;
    // 提交修改到 core_system
    bool commitChanges();
    void setTabletracker(TableEditTracker* tracker_){tracker = tracker_;}
signals:
    void cellEdited(QTableWidget* table, int row, int column, const QString& newValue, const QString& tableType);
    void load();
protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    TableEditTracker* tracker = nullptr;
    QHash<QWidget*, QTableWidget*> m_fileManagerToTable;  // 按钮 -> 表格
    QHash<QTableWidget*, QWidget*> m_tableToWarningWidget; // 表格 -> 未保存提示
    QHash<QTableWidget*, QWidget*> m_tableToDefaultWidget; //占位符
    QHash<QTableWidget*, QWidget*> m_tableToBalance_2;
    QHash<QTableWidget*, QLabel*>  m_tableToLable;
    QHash<QTableWidget*, bool>     m_tableToBool;
};

#endif // CHART_CHANGE_H
