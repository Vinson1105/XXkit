#ifndef XXactivityIndicatorView_h
#define XXactivityIndicatorView_h

#include <QWidget>
#include <QPaintEvent>
#include <QTimerEvent>

class XXactivityIndicatorView : public QWidget
{
    Q_OBJECT
public:
    XXactivityIndicatorView(QWidget *parent = nullptr);
    virtual ~XXactivityIndicatorView();


protected:
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);

private:
    void start();
    void stop();

private:
    qreal _itemPadding;
    qreal _itemMargin;
    QSizeF _itemSize;
    qreal _itemCount;
    qreal _activeItemCount;
    QColor _normalColor;
    QColor _activeColor;
    int _timerID;
    int _activeIndex;
};

#endif
