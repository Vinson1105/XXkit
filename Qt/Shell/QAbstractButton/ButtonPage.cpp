#include "ButtonPage.h"
#include "../Utils/XXlogger.h"

XX_KEY_C(ButtonPage,kFirstPage,"firstPage")
XX_KEY_C(ButtonPage,kPreviousPage,"previousPage")
XX_KEY_C(ButtonPage,kNextPage,"nextPage")
XX_KEY_C(ButtonPage,kLastPage,"lastPage")
XX_KEY_C(ButtonPage,kPageIndicator,"pageIndicator")

XX_KEY_C(ButtonPage,kReset,"reset")
XX_KEY_C(ButtonPage,kResetParamCount,"resetParamCount")
XX_KEY_C(ButtonPage,kResetParamCountPerPage,"resetParamCountPerPage")
XX_KEY_C(ButtonPage,kResetParamPageIndex,"resetParamPageIndex")

XX_KEY_C(ButtonPage,kGetCurrentPageIndex,"getCurrentPageIndex")
XX_KEY_C(ButtonPage,kGetPageCount,"getPageCount")
XX_KEY_C(ButtonPage,kGetCountPerPage,"getCountPerPage")

XX_KEY_C(ButtonPage,kSigCurrentPageChanged,"sigCurrentPageIndexChanged")
XX_KEY_C(ButtonPage,kPreviousIndex,"previousIndex")
XX_KEY_C(ButtonPage,kCurrentIndex,"currentIndex")

ButtonPage::ButtonPage(QObject *parent) : ShellBase(parent)
{

}

void ButtonPage::shell(QAbstractButton *first, QAbstractButton *previous, QLabel *page, QAbstractButton *next, QAbstractButton *last){
    TargetMap map;
    if(first) map[kFirstPage] = first;
    if(previous) map[kPreviousPage] = previous;
    if(next) map[kNextPage] = next;
    if(last) map[kLastPage] = last;
    if(page)  map[kPageIndicator] = page;
    ShellBase::shell(map);
}

QVariant ButtonPage::action(const QString &action, const QVariant &param){
    if(action == kReset){
        QVariantMap map = param.toMap();
        int count = map.value(kResetParamCount,0).toInt();
        int countPerPage = map.value(kResetParamCountPerPage,0).toInt();
        int pageIndex = map.value(kResetParamPageIndex,0).toInt();
        reset(count,countPerPage,pageIndex);
    }
    else if(action == kGetCurrentPageIndex){
        return getCurrentPageIndex();
    }
    else if(action == kGetPageCount){
        return getPageCount();
    }
    else if(action == kGetCountPerPage){
        return getCountPerPage();
    }
    else{

    }
    return QVariant();
}
void ButtonPage::reset(int count, int countPerPage, int pageIndex){
    _pageCount = count/countPerPage;
    if(count%countPerPage>0){
        _pageCount++;
    }
    _countPerPage = countPerPage;
    _count = count;
    setPageIndex(pageIndex);
}
int ButtonPage::getCurrentPageIndex(){
    return _pageIndex;
}
int ButtonPage::getPageCount(){
    return _pageCount;
}
int ButtonPage::getCountPerPage(){
    return _countPerPage;
}

void ButtonPage::resetTarget(const QString &name, QObject *target){
    ShellBase::resetTarget(name,target);
    if(name == kFirstPage || name == kPreviousPage || name == kNextPage || name == kLastPage){
        QAbstractButton *button = qobject_cast<QAbstractButton*>(target);
        if(name == kFirstPage) connect(button,&QAbstractButton::clicked,this,&ButtonPage::button_firstPage_onClicked);
        if(name == kPreviousPage) connect(button,&QAbstractButton::clicked,this,&ButtonPage::button_previousPage_onClicked);
        if(name == kNextPage) connect(button,&QAbstractButton::clicked,this,&ButtonPage::button_nextPage_onClicked);
        if(name == kLastPage) connect(button,&QAbstractButton::clicked,this,&ButtonPage::button_lastPage_onClicked);
    }
    else{

    }
}

QAbstractButton *ButtonPage::first(){
    return qobject_cast<QAbstractButton*>(target(kFirstPage));
}
QAbstractButton *ButtonPage::previous(){
    return qobject_cast<QAbstractButton*>(target(kPreviousPage));
}
QLabel *ButtonPage::page(){
    return qobject_cast<QLabel*>(target(kPageIndicator));
}
QAbstractButton *ButtonPage::next(){
    return qobject_cast<QAbstractButton*>(target(kNextPage));
}
QAbstractButton *ButtonPage::last(){
    return qobject_cast<QAbstractButton*>(target(kLastPage));
}

void ButtonPage::setPageIndex(int index){
    if(index>_pageCount){
        return;
    }
    int temp = _pageIndex;
    _pageIndex = index;
    first()->setEnabled(index>0);
    previous()->setEnabled(index>0);
    next()->setEnabled(index>=0&&index+1<_pageCount);
    last()->setEnabled(index>=0&&index+1<_pageCount);
    page()->setText(QString("%1/%2").arg(_pageIndex<0?1:_pageIndex+1).arg(0==_pageCount?1:_pageCount));

    if(temp != index){
        QVariantMap info = {{kPreviousIndex,temp},{kCurrentIndex,index}};
        emit sigEvent(kSigCurrentPageChanged,info);
    }
}

void ButtonPage::button_firstPage_onClicked(){
    if(_pageIndex<0|| _pageCount<=1){
        xxLog("[BUG] invalid param. count:%d countPerPage:%d pageCount:%d pageIndex:%d",_count,_countPerPage,_pageCount,_pageIndex);
        return;
    }
    setPageIndex(0);
}
void ButtonPage::button_previousPage_onClicked(){
    if(_pageIndex<0|| _pageCount<=1){
        xxLog("[BUG] invalid param. count:%d countPerPage:%d pageCount:%d pageIndex:%d",_count,_countPerPage,_pageCount,_pageIndex);
        return;
    }
    setPageIndex(_pageIndex-1);
}
void ButtonPage::button_nextPage_onClicked(){
    if(_pageIndex>=_pageCount|| _pageCount<=1){
        xxLog("[BUG] invalid param. count:%d countPerPage:%d pageCount:%d pageIndex:%d",_count,_countPerPage,_pageCount,_pageIndex);
        return;
    }
    setPageIndex(_pageIndex+1);
}
void ButtonPage::button_lastPage_onClicked(){
    if(_pageIndex>=_pageCount || _pageCount<=1){
        xxLog("[BUG] invalid param. count:%d countPerPage:%d pageCount:%d pageIndex:%d",_count,_countPerPage,_pageCount,_pageIndex);
        return;
    }
    setPageIndex(_pageCount-1);
}
