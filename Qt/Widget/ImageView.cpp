#include "ImageView.h"
#include <QResizeEvent>
#include <QPaintEvent>
#include <QPainter>
#include <DebugDef.h>

ImageView::ImageView(QWidget *parent)
    : QWidget(parent)
    , _path("")
    , _pixmap(QPixmap()){

}
ImageView::ImageView(const QString &path, QWidget *parent)
    : QWidget(parent)
    , _path(path)
    , _pixmap(QPixmap(path)){
}
ImageView::~ImageView(){
}

void ImageView::reset(const QString &path){
    _pixmap = QPixmap(path);
    this->repaint();
}

void ImageView::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);
}
void ImageView::paintEvent(QPaintEvent *event){
    if(_pixmap.isNull()){
        QWidget::paintEvent(event);
        return;
    }

    QSize targetSize = this->size();
    QPixmap pixmapAfterScale = _pixmap.scaled(targetSize, Qt::AspectRatioMode::KeepAspectRatio);
    qreal x = (targetSize.width() - pixmapAfterScale.width()) * 0.5;
    qreal y = (targetSize.height() - pixmapAfterScale.height()) * 0.5;
    QPainter painter(this);
    painter.drawPixmap(QPointF(x,y),pixmapAfterScale);
}
