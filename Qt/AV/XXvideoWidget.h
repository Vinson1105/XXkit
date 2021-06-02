#ifndef XXVIDEOWIDGET_H
#define XXVIDEOWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>

#include <QMutex>
#include <QMutexLocker>

class XXvideoWidget : public QOpenGLWidget, protected QOpenGLFunctions{
    Q_OBJECT
public:
    XXvideoWidget(QWidget *parent = nullptr);
    ~XXvideoWidget();

    void yuv(qint8 *raw, const QSize &videoSize);
    void yuv(qint8 **yuv, const QSize &videoSize);
    void yuv(qint8 *y, qint8 *u, qint8 *v, const QSize &videoSize);
	void reset();
    void render(bool enable);

    bool get(qint8 **yuv, QSize &videoSize);
    bool get(qint8 *yuv, QSize &videoSize);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    void setYuvBuffer(qint8 *yuv[], const QSize &videoSize);
    bool getYuvBuffer(qint8 *yuv[], QSize &videoSize);
    bool getYuvBuffer(qint8 *yuv, QSize &videoSize);
    void freeYuvBuffer();
    QRect adjustRect(const QSize &size, const QRect &at);

private:
    // 分辨率
    QSize _videoSize;

    // yuv数据锁
    QMutex _yuvBufferMutex;
    // 存放外部传递的yuv数据缓存
    qint8 *_yuvBuffer[3];
    // 是否有新数据，主要是针对_yuvBuffer
    bool _isHasData;

    // yuv纹理数据位置
    GLint _yuvTextureUniform[3];
    // yuv纹理对象id
    GLuint _yuvID[3];
    // yuv纹理对象
    QOpenGLTexture *_yuvTexture[3];
    // 顶点、纹理着色器
    QOpenGLShader *_vshader, *_fshader;
    // 着色器程序
    QOpenGLShaderProgram *_shaderProgram;

    // 渲染定时器
    QTimer *_renderTimer;
    // 缩放比例
    qreal _pixelRatio;
};

#endif // XXVIDEOWIDGET_H
