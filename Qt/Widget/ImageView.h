#ifndef __ImageView_h
#define __ImageView_h

#include <QWidget>

class ImageView : public QWidget
{
    Q_OBJECT
public:
    ImageView(QWidget *parent = nullptr);
    ImageView(const QString &path, QWidget *parent = nullptr);
    virtual ~ImageView();

    void reset(const QString &path);

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    QString _path;
    QPixmap _pixmap;
};

#endif
