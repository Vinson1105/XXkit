#include "XXlayoutConstraint.h"

#define ANCHOR(attr1,attr2) (_attr1==attr1 && _attr2==attr2)

void XXlayoutConstraint::constraint(QWidget *item, Attribute attr1, QWidget *toItem, Attribute attr2, int constant){
    new XXlayoutConstraint(item,attr1,toItem,attr2,constant);
}
XXlayoutConstraint::XXlayoutConstraint(QWidget *item, Attribute attr1, QWidget *toItem, Attribute attr2, int constant)
    : QObject (item){
    _item = item;
    _attr1 = attr1;
    _toItem = toItem;
    _attr2 = attr2;
    _constant = constant;

    if(
            /** 水平/垂直方向不能依赖垂直/水平方向 */
            ((Top==attr1 || Bottom==attr1 || CenterY==attr1) && (Left==attr1 || Right==attr1 || CenterX==attr1))
            ||
            /** 必须要有parentWidget */
            (nullptr ==_item->parentWidget())
            ||
            /** 必须要有parentWidget */
            (nullptr ==toItem->parentWidget())
            ||
            /** 两者需要是直系或者同系 */
            (_item->parentWidget()!=toItem && _item->parentWidget()!=_toItem->parentWidget())){
        _isInvaild = true;
        _isActived = false;
    }
    else{
        /**
          FIXME: 当item更改时没有对toItem更改，当加上时需要注意递归的问题。
         */
        _toItem->installEventFilter(this);
        _item->installEventFilter(this);
        connect(_toItem, SIGNAL(destroyed(QObject*)),this,SLOT(onDestroyed(QObject*)));
        connect(_item, SIGNAL(destroyed(QObject*)),this,SLOT(onDestroyed(QObject*)));
        _isActived = true;
        _isInvaild = false;
    }
}
XXlayoutConstraint::~XXlayoutConstraint(){
    reset();
}

void XXlayoutConstraint::action(bool enable){
    if(_isActived == enable || _isInvaild){
        return;
    }
    if(enable){
        _toItem->installEventFilter(this);
    }
    else{
        _toItem->removeEventFilter(this);
    }
}

bool XXlayoutConstraint::eventFilter(QObject *watched, QEvent *event){
    if(watched == _toItem){
        if(QEvent::DeferredDelete == event->type()){
            reset();
        }
        else if(QEvent::Move == event->type()){
            updateItem();
        }
        else if(QEvent::Resize == event->type()){
            updateItem();
        }
        else{

        }
    }
    else if(watched == _item){
        if(QEvent::DeferredDelete == event->type()){
            reset();
        }
    }
    else{

    }
    return QObject::eventFilter(watched,event);
}
void XXlayoutConstraint::reset(){
    if(nullptr != _toItem){
        _toItem->removeEventFilter(this);
    }
    if(nullptr != _item){
        _item->removeEventFilter(this);
    }
    _toItem = nullptr;
    _item = nullptr;
}
void XXlayoutConstraint::updateItem(){
    if(nullptr==_toItem || nullptr==_item){
        return;
    }

    QRect itemRect = _item->geometry();
    QRect toItemRect = _item->parentWidget()==_toItem ? _toItem->rect() : _toItem->geometry();

    /** Top */
    if(ANCHOR(Top,Top)){
        itemRect = {itemRect.left(),toItemRect.top()+_constant,itemRect.width(),itemRect.height()};
    }
    else if(ANCHOR(Top,CenterY)){
        itemRect = {itemRect.left(),toItemRect.center().y()+_constant,itemRect.width(),itemRect.height()};
    }
    else if(ANCHOR(Top,Bottom)){
        itemRect = {itemRect.left(),toItemRect.bottom()+_constant,itemRect.width(),itemRect.height()};
    }

    /** CenterY */
    else if(ANCHOR(CenterY,Top)){
        itemRect = {itemRect.left(),toItemRect.top()+_constant-(itemRect.height()/2),itemRect.width(),itemRect.height()};
    }
    else if(ANCHOR(CenterY,CenterY)){
        itemRect = {itemRect.left(),toItemRect.center().y()+_constant-(itemRect.height()/2),itemRect.width(),itemRect.height()};
    }
    else if(ANCHOR(CenterY,Bottom)){
        itemRect = {itemRect.left(),toItemRect.bottom()+_constant-(itemRect.height()/2),itemRect.width(),itemRect.height()};
    }

    /** Bottom */
    if(ANCHOR(Bottom,Top)){
        itemRect = {itemRect.left(),toItemRect.top()+_constant-itemRect.height(),itemRect.width(),itemRect.height()};
    }
    else if(ANCHOR(Bottom,CenterY)){
        itemRect = {itemRect.left(),toItemRect.center().y()+_constant-itemRect.height(),itemRect.width(),itemRect.height()};
    }
    else if(ANCHOR(Bottom,Bottom)){
        itemRect = {itemRect.left(),toItemRect.bottom()+_constant-itemRect.height(),itemRect.width(),itemRect.height()};
    }

    /** Left */
    if(ANCHOR(Left,Left)){
        itemRect = {toItemRect.left()+_constant,itemRect.top(),itemRect.width(),itemRect.height()};
    }
    else if(ANCHOR(Left,CenterX)){
        itemRect = {toItemRect.center().x()+_constant,itemRect.top(),itemRect.width(),itemRect.height()};
    }
    else if(ANCHOR(Left,Right)){
        itemRect = {toItemRect.right()+_constant,itemRect.top(),itemRect.width(),itemRect.height()};
    }

    /** CenterX */
    else if(ANCHOR(CenterX,Left)){
        itemRect = {toItemRect.left()+_constant-(itemRect.width()/2),itemRect.top(),itemRect.width(),itemRect.height()};
    }
    else if(ANCHOR(CenterX,CenterX)){
        itemRect = {toItemRect.center().x()+_constant-(itemRect.width()/2),itemRect.top(),itemRect.width(),itemRect.height()};
    }
    else if(ANCHOR(CenterX,Right)){
        itemRect = {toItemRect.right()+_constant-(itemRect.width()/2),itemRect.top(),itemRect.width(),itemRect.height()};
    }

    /** Right */
    if(ANCHOR(Right,Left)){
        itemRect = {toItemRect.left()+_constant-itemRect.width(),itemRect.top(),itemRect.width(),itemRect.height()};
    }
    else if(ANCHOR(Right,CenterX)){
        itemRect = {toItemRect.left()+_constant-itemRect.width(),itemRect.top(),itemRect.width(),itemRect.height()};
    }
    else if(ANCHOR(Right,Right)){
        itemRect = {toItemRect.right()+_constant-itemRect.width(),itemRect.top(),itemRect.width(),itemRect.height()};
    }

    _item->setGeometry(itemRect);
}

void XXlayoutConstraint::updateItemTopToTop(){
    QRect itemRect = _item->geometry();
    QRect toItemRect = _toItem->geometry();

    itemRect.setTop(toItemRect.y()+_constant);
    _item->setGeometry(itemRect);
}
void XXlayoutConstraint::updateItemTopToCenterY(){
    QRect itemRect = _item->geometry();
    QRect toItemRect = _toItem->geometry();

    itemRect.setTop(toItemRect.center().y()+_constant);
    _item->setGeometry(itemRect);
}
void XXlayoutConstraint::updateItemTopToBottom(){
    QRect itemRect = _item->geometry();
    QRect toItemRect = _toItem->geometry();

    itemRect.setTop(toItemRect.bottom()+_constant);
    _item->setGeometry(itemRect);
}

void XXlayoutConstraint::onDestroyed(QObject *obj){
    Q_UNUSED(obj);
    reset();
}
