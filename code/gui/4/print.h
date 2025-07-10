#ifndef PRINT_H
#define PRINT_H

#include <QStyledItemDelegate>
#include <QPixmap>

class print : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit print(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:

private:
    QPixmap m_evenPixmap;
    QPixmap m_oddPixmap;
    QPixmap m_headerPixmap;
};

#endif // PRINT_H
