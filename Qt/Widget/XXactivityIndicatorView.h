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

    void config(const QVariantMap &param);

    const static char * const kItemPadding;
    const static char * const kItemMargin;
    const static char * const kItemSize;
    const static char * const kItemCount;
    const static char * const kActiveItemCount;

    const static char * const kNormalColor;
    const static char * const kActiveColor;
    const static char * const kBgColor;
    const static char * const kBgRadius;

    Q_SLOT void show();
    Q_SLOT void hide();

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
    QColor _bgColor;
    qreal _bgRadius;
    int _timerID;
    int _activeIndex;
    /** 是否铺满parentWidget，默认false */
    bool _isFullAtParent;
};

#endif
