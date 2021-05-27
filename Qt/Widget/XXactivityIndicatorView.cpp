#include "XXactivityIndicatorView.h"
#include <QPainter>

XXactivityIndicatorView::XXactivityIndicatorView(QWidget *parent)
    : QWidget(parent)
    , _itemPadding(10)
    , _itemMargin(10)
    , _itemSize(4,15)
    , _itemCount(8)
    , _activeItemCount(4)
    , _normalColor(153, 153, 153)
    , _activeColor(Qt::white)
    , _timerID(0)
    , _activeIndex(0){
    start();
}
XXactivityIndicatorView::~XXactivityIndicatorView(){
    stop();
}


void XXactivityIndicatorView::paintEvent(QPaintEvent *event){
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);

    // 绘制一下背景
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawRect(event->rect());

    // 移动到中心区域
    qreal bgWidth = event->rect().size().width();
    qreal bgHeight = event->rect().size().height();
    painter.translate(event->rect().center());

    // 绘制基本的item
    qreal itemWidth = _itemSize.width();
    qreal itemHeight = _itemSize.height() > 0 ? _itemSize.height() : bgHeight*0.5-_itemPadding-_itemMargin;

    painter.setPen(Qt::NoPen);
    painter.setBrush(_normalColor);
    QRectF itemRect(-itemWidth*0.5,-_itemPadding-itemHeight,itemWidth,itemHeight);
    qreal step = 360.0/_itemCount;
    for (int index=0; index<_itemCount; index++) {
        painter.rotate(step);
        painter.drawRect(itemRect);
    }

    // 绘制激活项
    qreal startAlpha = 0.2;
    qreal endAlpha = 1;
    qreal stepAlpha = (endAlpha-startAlpha)/(_activeItemCount-1);
    for (int index=0; index<_activeItemCount; index++) {
        if(0 == index){
            painter.rotate(step*(_activeIndex-3));
        }
        else{
            painter.rotate(step);
        }

        QColor color = _activeColor;
        color.setAlphaF(startAlpha + index*stepAlpha);
        painter.setBrush(color);
        painter.drawRect(itemRect);
    }
}
void XXactivityIndicatorView::timerEvent(QTimerEvent *event){
    if(_timerID > 0 && event->timerId() == _timerID){
        if(++_activeIndex >=_itemCount){
            _activeIndex = 0;
        }
        update();
    }
    QWidget::timerEvent(event);
}

void XXactivityIndicatorView::start(){
    if(_timerID>0){
        return;
    }
    _timerID = startTimer(40);
    _activeIndex = 0;
}
void XXactivityIndicatorView::stop(){
    if(_timerID<=0){
        return;
    }
    killTimer(_timerID);
    _timerID = 0;
    _activeIndex = 0;
}
