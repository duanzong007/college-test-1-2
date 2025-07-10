#include "dis_page.h"

dis_page::dis_page(QObject *parent) : QObject(parent),
    currentPage(1), totalPages(1), total(0), pageSize1(1000), pageSize2(200), pageLineEdit(nullptr) {}


void dis_page::setTotalItems(int total1){

    total=total1;
    if(total>20000)
    {
        totalPages=std::ceil(total / static_cast<double>(pageSize1));
    }
    else
    {
        totalPages=std::ceil(total / static_cast<double>(pageSize2));
    }
    currentPage = qMin(currentPage, totalPages);  // 确保 currentPage 不超过总页数
    //emit pageChanged(currentPage);
    if (pageLineEdit) {
        pageLineEdit->setText(pageDisplayText());
    }
}

void dis_page::setCurrentPage(int page) {
    if (page >= 1 && page <= totalPages) {
        currentPage = page;
        emit pageChanged(currentPage); // 触发信号
    }
}

QString dis_page::pageDisplayText() const
{
    return QString("%1/%2页").arg(currentPage).arg(totalPages);
}

void dis_page::setPageLineEdit(QLineEdit *lineEdit)
{
    pageLineEdit = lineEdit;

    if (pageLineEdit) {
        // 初始显示
        pageLineEdit->setText(pageDisplayText());
        pageLineEdit->setAlignment(Qt::AlignCenter);//居中
        pageLineEdit->setValidator(new QIntValidator(1, 10000, this)); // 只允许数字输入
        pageLineEdit->installEventFilter(this); // 安装事件过滤器
        // 连接信号槽，处理用户输入
        connect(pageLineEdit, &QLineEdit::returnPressed, this,&dis_page::onPageLineEditReturnPressed);
    }
}

bool dis_page::eventFilter(QObject *watched, QEvent *event) {
    if (watched == pageLineEdit) {
        if (event->type() == QEvent::FocusIn) {
            // 获得焦点时进入编辑模式
            m_editMode = true;
            pageLineEdit->clear();
            return true;
        }
        else if (event->type() == QEvent::FocusOut) {
            // 失去焦点时恢复显示页码
            if (m_editMode) {
                m_editMode = false;
                pageLineEdit->setText(pageDisplayText());
            }
            return true;
        }
    }
    return QObject::eventFilter(watched, event);
}

void dis_page::onPageLineEditReturnPressed()
{
    if (!pageLineEdit) return;
    bool ok;
    int page = pageLineEdit->text().toInt(&ok);

    if (ok && page >= 1 && page <= totalPages) {
        setCurrentPage(page);
    } else {
        QToolTip::showText(pageLineEdit->mapToGlobal(QPoint(0, -20)),
                           QString("请输入 1~%1 之间的页码").arg(totalPages),
                           pageLineEdit, QRect(), 1000);
        pageLineEdit->setText(QString::number(currentPage));
    }

    // 退出编辑模式
    m_editMode = false;
    pageLineEdit->clearFocus();
    pageLineEdit->setText(pageDisplayText());
}

void dis_page::nextPage()
{
    if(currentPage < totalPages) {
        currentPage++;
        setCurrentPage(currentPage);
    }
}

void dis_page::prevPage()
{
    if(currentPage > 1) {
        currentPage--;
        emit pageChanged(currentPage);
    }
}

void dis_page::gotoPage(int page)
{
    if(page >= 1 && page <= totalPages) {
        setCurrentPage(page);
    }
}
int dis_page::currentPageSize() const
{
    return total > 20000 ? pageSize1 : pageSize2;
}
