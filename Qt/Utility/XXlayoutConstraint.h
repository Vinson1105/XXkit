#ifndef XXlayoutConstraint_h
#define XXlayoutConstraint_h

/*
 * 暂时只是支持单对单的约束，并需要注意约束有先后顺序；
*/

#include <QObject>
#include <QEvent>
#include <QWidget>

class XXlayoutConstraint : public QObject
{
    Q_OBJECT
public:
    enum Attribute {Top,Left,Right,Bottom,CenterX,CenterY,};
    /**
     * item是需要修改的widget，toItem是参照的widget，只能参照同一方向；
     */
    static void constraint(QWidget *item, Attribute attr1, QWidget *toItem, Attribute attr2, int constant);
    XXlayoutConstraint(QWidget *item, Attribute attr1, QWidget *toItem, Attribute attr2, int constant);
    virtual ~XXlayoutConstraint();

    void action(bool enable);

private:
    bool eventFilter(QObject *watched, QEvent *event);
    void reset();
    void updateItem();

    void updateItemTopToTop();
    void updateItemTopToCenterY();
    void updateItemTopToBottom();

    void updateItemLeftToLeft();
    void updateItemLeftToCenterX();
    void updateItemLeftToRight();

    void updateItemRightToLeft();
    void updateItemRightToCenterX();
    void updateItemRightToRight();

    void updateItemBottomToTop();
    void updateItemBottomToCenterY();
    void updateItemBottomToBottom();

    void updateItemCenterXToLeft();
    void updateItemCenterXToCenterX();
    void updateItemCenterXToRight();

    void updateItemCenterYToTop();
    void updateItemCenterYToCenterY();
    void updateItemCenterYToBottom();

private slots:
    void onDestroyed(QObject *obj);

private:
    QWidget *_item;
    Attribute _attr1;
    QWidget *_toItem;
    Attribute _attr2;
    int _constant;
    bool _isInvaild;
    bool _isActived;
};

#endif
