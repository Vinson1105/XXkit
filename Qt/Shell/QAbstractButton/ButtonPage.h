#ifndef BUTTONPAGE_H
#define BUTTONPAGE_H

#include "ShellBase.h"
#include "../XXqtDefine.h"
#include <QLabel>
#include <QAbstractButton>

class ButtonPage : public ShellBase
{
    Q_OBJECT
public:
    explicit ButtonPage(QObject *parent = nullptr);

    XX_KEY(kFirstPage)
    XX_KEY(kPreviousPage)
    XX_KEY(kNextPage)
    XX_KEY(kLastPage)
    XX_KEY(kPageIndicator)
    void shell(QAbstractButton *first, QAbstractButton *previous, QLabel *page, QAbstractButton *next, QAbstractButton *last);

    XX_KEY(kReset)
        XX_KEY(kResetParamCount)    // 必须
        XX_KEY(kResetParamCountPerPage) // 必须
        XX_KEY(kResetParamPageIndex)    // 可缺省，缺省时重置为0
    XX_KEY(kGetCurrentPageIndex)
    XX_KEY(kGetPageCount)
    XX_KEY(kGetCountPerPage)
    QVariant action(const QString &action, const QVariant &param = QVariant());
    void reset(int count, int countPerPage, int pageIndex = 0);
    int getCurrentPageIndex();
    int getPageCount();
    int getCountPerPage();

    // sigEvent
    XX_KEY(kSigCurrentPageChanged)
        XX_KEY(kPreviousIndex)
        XX_KEY(kCurrentIndex)

protected:
    void resetTarget(const QString &name, QObject *target);

private:
    QAbstractButton *first();
    QAbstractButton *previous();
    QLabel *page();
    QAbstractButton *next();
    QAbstractButton *last();

    void setPageIndex(int index);

private slots:
    void button_firstPage_onClicked();
    void button_previousPage_onClicked();
    void button_nextPage_onClicked();
    void button_lastPage_onClicked();

private:
    int _pageIndex;
    int _count;
    int _countPerPage;
    int _pageCount;

};

#endif // BUTTONPAGE_H
