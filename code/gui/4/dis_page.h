#ifndef DIS_PAGE_H
#define DIS_PAGE_H

#include <QObject>
#include<QStandardItemModel>
#include<QLineEdit>
#include <QRegularExpression>
#include<QtAlgorithms>
#include<QIntValidator>
#include<QToolTip>
#include <QEvent>

class dis_page : public QObject
{
    Q_OBJECT
public:
    explicit dis_page(QObject *parent = nullptr);
    int currentPageSize() const;
    // 分页控制变量
    int currentPage = 1;         // 当前页码
    int totalPages = 1;          // 总页数
    int total = 0;               // 总数据量
    const int pageSize1 = 1000;
    const int pageSize2 = 100;
    QLineEdit *pageLineEdit; // 页码显示控件

    void setTotalItems(int total);//总页数
    void setCurrentPage(int page);
    void gotoPage(int page);

    bool m_editMode = false; // 标记是否处于编辑状态

    bool eventFilter(QObject *watched, QEvent *event) override; // 事件过滤器

    QString pageDisplayText() const;//页码显示文本
    void setPageLineEdit(QLineEdit *lineEdit);// 设置页码显示控件
    void updatePageDisplay() {
        if (pageLineEdit) {
            pageLineEdit->setText(pageDisplayText());
        }
    }

    void setTotalPages(int pages) {
        totalPages = pages;
        if (pageLineEdit) {
            pageLineEdit->setText(pageDisplayText());
        }
    }

signals:
    void pageChanged(int curRrentPage); // 页码变化信号
public slots:
    void nextPage();
    void prevPage();
    void onPageLineEditReturnPressed();

};

#endif // DIS_PAGE_H
