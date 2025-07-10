// print.cpp
#include "print.h"
#include <QPainter>
#include <QStyleOptionHeader>
#include <QHeaderView>
#include <QDebug>
#include <QApplication>

print::print(QObject *parent)
    : QStyledItemDelegate(parent),
    m_evenPixmap   (":/images/sideBar/black0.png"),
    m_oddPixmap    (":/images/sideBar/black2.png"),
    m_headerPixmap (":/images/sideBar/black3.png")
{
    // 检查图片是否成功加载
    if (m_evenPixmap.isNull())
        qDebug() << "警告: 偶数行图片加载失败!";
    if (m_oddPixmap.isNull())
        qDebug() << "警告: 奇数行图片加载失败!";
    if (m_headerPixmap.isNull())
        qDebug() << "警告: 表头图片加载失败!";
}

void print::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QStyleOptionHeader *headerOption = qstyleoption_cast<const QStyleOptionHeader*>(&option);

    if (!headerOption) {
        // 普通单元格绘制（原有逻辑）
        painter->save();

        if (index.row() % 2 == 0) {
            if (!m_evenPixmap.isNull()) {
                painter->drawPixmap(option.rect, m_evenPixmap.scaled(
                                                     option.rect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            }
        } else {
            if (!m_oddPixmap.isNull()) {
                painter->drawPixmap(option.rect, m_oddPixmap.scaled(
                                                     option.rect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            }
        }

        painter->restore();
        QStyledItemDelegate::paint(painter, option, index);
    }
}
