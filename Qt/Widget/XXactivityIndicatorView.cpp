#include "XXactivityIndicatorView.h"
#include <QPainter>

const char * const XXactivityIndicatorView::kItemPadding = "itemPadding";
const char * const XXactivityIndicatorView::kItemMargin = "itemMargin";
const char * const XXactivityIndicatorView::kItemSize = "itemSize";
const char * const XXactivityIndicatorView::kItemCount = "itemCount";
const char * const XXactivityIndicatorView::kActiveItemCount = "activeItemCount";

const char * const XXactivityIndicatorView::kNormalColor = "normalColor";
const char * const XXactivityIndicatorView::kActiveColor = "activeColor";
const char * const XXactivityIndicatorView::kBgColor = "bgColor";
const char * const XXactivityIndicatorView::kBgRadius = "bgRadius";

#define IsNumber(x) (x.type()==QVariant::Int||x.type()==QVariant::Double)
#define IsSize(x) (x.type()==QVariant::Size||x.type()==QVariant::SizeF)

XXactivityIndicatorView::XXactivityIndicatorView(QWidget *parent)
    : QWidget(parent)
    , _itemPadding(10)
    , _itemMargin(10)
    , _itemSize(4,15)
    , _itemCount(8)
    , _activeItemCount(4)
    , _normalColor(153, 153, 153)
    , _activeColor(Qt::white)
    , _bgColor(0,0,0,127)
    , _bgRadius(20.0)
    , _timerID(0)
    , _activeIndex(0)
    , _isFullAtParent(false){
    this->setAttribute(Qt::WA_StyledBackground);
    this->setFixedSize(100,100);
    QWidget::hide();
}
XXactivityIndicatorView::~XXactivityIndicatorView(){
    stop();
}

void XXactivityIndicatorView::config(const QVariantMap &param){
    if(param.contains(kItemPadding) && param[kItemPadding].canConvert<qreal>()){
        _itemPadding = param[kItemPadding].toDouble();
    }
    if(param.contains(kItemMargin) && param[kItemMargin].canConvert<qreal>()){
        _itemMargin = param[kItemMargin].toDouble();
    }
    if(param.contains(kItemSize) && param[kItemSize].canConvert<QSizeF>()){
        _itemSize = param[kItemSize].toSizeF();
    }
    if(param.contains(kItemCount) && param[kItemCount].canConvert<int>()){
        _itemCount = param[kItemCount].toInt();
    }
    if(param.contains(kActiveItemCount) && param[kActiveItemCount].canConvert<int>()){
        _activeItemCount = param[kActiveItemCount].toInt();
    }

    if(param.contains(kNormalColor) && param[kNormalColor].canConvert<QColor>()){
        _normalColor = param[kNormalColor].value<QColor>();
    }
    if(param.contains(kActiveColor) && param[kActiveColor].canConvert<QColor>()){
        _activeColor = param[kActiveColor].value<QColor>();
    }
    if(param.contains(kBgColor) && param[kBgColor].canConvert<QColor>()){
        _bgColor = param[kBgColor].value<QColor>();
    }
}

void XXactivityIndicatorView::show(){
    auto *widget = parentWidget();
    if(nullptr != widget){
        if(_isFullAtParent){
            this->setGeometry(widget->geometry());
        }
        else{
            QPoint center = widget->rect().center();
            QPoint leftTop(center.x()-this->size().width()/2,center.y()-this->size().height()/2);
            move(leftTop);
        }
    }

    start();
    QWidget::show();
}
void XXactivityIndicatorView::hide(){
    QWidget::hide();
    stop();
}

void XXactivityIndicatorView::paintEvent(QPaintEvent *event){
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);

    // 绘制一下背景
    painter.setPen(Qt::NoPen);
    painter.setBrush(_bgColor);
    painter.drawRoundedRect(event->rect(),_bgRadius,_bgRadius);

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
