#include "XXvideoWidget.h"
#include <QDebug>
#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>

static const char *vstring =
        "attribute vec4 vertexIn;\
        attribute vec2 textureIn;\
        varying vec2 textureOut;\
        void main(void)\
        {\
           gl_Position = vertexIn;\
           textureOut = textureIn;\
        }";

static const char *fstring =
        "varying vec2 textureOut; \
        uniform sampler2D tex_y; \
        uniform sampler2D tex_u;\
        uniform sampler2D tex_v;\
        void main(void)\
        {\
           vec3 yuv;\
           vec3 rgb;\
           yuv.x = texture2D(tex_y, textureOut).r;\
           yuv.y = texture2D(tex_u, textureOut).r - 0.5;\
           yuv.z = texture2D(tex_v, textureOut).r - 0.5;\
           rgb = mat3( 1,       1,         1,\
                       0,       -0.39465,  2.03211,\
                       1.13983, -0.58060,  0) * yuv;\
           gl_FragColor = vec4(rgb, 1);\
        }";
#define kYUVBufferSizeMax 4096*4096

XXvideoWidget::XXvideoWidget(QWidget *parent)
    : QOpenGLWidget (parent)
{
	_isHasData = false;
    for (int index=0; index<3; index++) {
        _yuvBuffer[index]   = (qint8*)malloc(kYUVBufferSizeMax);
		memset(_yuvBuffer[index], 0, kYUVBufferSizeMax);
        _yuvID[index]       = 0;
        _yuvTexture[index]  = nullptr;
    }
    _vshader = _fshader = nullptr;
    _shaderProgram = nullptr;

    _renderTimer = new QTimer(this);
    //_renderTimer->setInterval(20);
    //_renderTimer->start();
    //connect(_renderTimer, SIGNAL(timeout(void)), this, SLOT(update(void)));

    _pixelRatio = QApplication::desktop()->devicePixelRatioF();

}
XXvideoWidget::~XXvideoWidget(){
    freeYuvBuffer();

    for (int index=0; index<3; index++) {
        if(nullptr != _yuvTexture[index]){
            delete  _yuvTexture[index];
            _yuvTexture[index] = nullptr;
        }
    }

    if(_vshader){ delete _vshader; _vshader = nullptr; }
    if(_fshader){ delete _fshader; _fshader = nullptr; }
    if(_shaderProgram){ delete _shaderProgram; _shaderProgram = nullptr; }
}

void XXvideoWidget::yuv(qint8 *raw, const QSize &videoSize){
    qint8 *y = raw;
    qint8 *u = y + videoSize.width()*videoSize.height();
    qint8 *v = u + videoSize.width()*videoSize.height()/4;

    qint8 *yuv[] = {y,u,v};
    setYuvBuffer(yuv,videoSize);
}
void XXvideoWidget::yuv(qint8 **yuv, const QSize &videoSize){
    setYuvBuffer(yuv,videoSize);
}
void XXvideoWidget::yuv(qint8 *y, qint8 *u, qint8 *v, const QSize &videoSize){
    qint8 *yuv[] = {y,u,v};
    setYuvBuffer(yuv,videoSize);
}
void XXvideoWidget::reset() {
	_isHasData = false;
	this->update();
}
void XXvideoWidget::render(bool enable){
    if(enable){
        _renderTimer->start();
    }
    else{
        _renderTimer->stop();
    }
}


bool XXvideoWidget::get(qint8 **yuv, QSize &videoSize){
    return getYuvBuffer(yuv,videoSize);
}
bool XXvideoWidget::get(qint8 *yuv, QSize &videoSize){
    return getYuvBuffer(yuv,videoSize);
}

void XXvideoWidget::initializeGL(){
#define kVertexInLocation 3
#define kTextureInLocation 4

    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    _vshader = new QOpenGLShader(QOpenGLShader::Vertex,this);
    _vshader->compileSourceCode(vstring);

    _fshader = new QOpenGLShader(QOpenGLShader::Fragment,this);
    _fshader->compileSourceCode(fstring);

    //创建着色器程序容器
    _shaderProgram = new QOpenGLShaderProgram;
    //将片段着色器添加到程序容器
	if (!_shaderProgram->addShader(_fshader)) {
		qDebug() << __FUNCTION__ << __LINE__ << "error";
	}
    //将顶点着色器添加到程序容器
	if (!_shaderProgram->addShader(_vshader)) {
		qDebug() << __FUNCTION__ << __LINE__ << "error";
	}
    //绑定属性vertexIn到指定位置ATTRIB_VERTEX,该属性在顶点着色源码其中有声明
    _shaderProgram->bindAttributeLocation("vertexIn", kVertexInLocation);
    //绑定属性textureIn到指定位置ATTRIB_TEXTURE,该属性在顶点着色源码其中有声明
    _shaderProgram->bindAttributeLocation("textureIn", kTextureInLocation);
    //链接所有所有添入到的着色器程序
    _shaderProgram->link();
    //激活所有链接
    _shaderProgram->bind();
    //读取着色器中的数据变量tex_y, tex_u, tex_v的位置,这些变量的声明可以在
    //片段着色器源码中可以看到
    _yuvTextureUniform[0] = _shaderProgram->uniformLocation("tex_y");
    _yuvTextureUniform[1] = _shaderProgram->uniformLocation("tex_u");
    _yuvTextureUniform[2] = _shaderProgram->uniformLocation("tex_v");

    // 顶点矩阵 - 渲染区域
    static const GLfloat vertexVertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f, 1.0f,
        1.0f, 1.0f,
    };
    // 纹理矩阵 - 纹理渲染区域
    static const GLfloat textureVertices[] = {
        0.0f,  1.0f,
        1.0f,  1.0f,
        0.0f,  0.0f,
        1.0f,  0.0f,
    };
    //设置属性ATTRIB_VERTEX的顶点矩阵值以及格式
    glVertexAttribPointer(kVertexInLocation, 2, GL_FLOAT, 0, 0, vertexVertices);
    //设置属性ATTRIB_TEXTURE的纹理矩阵值以及格式
    glVertexAttribPointer(kTextureInLocation, 2, GL_FLOAT, 0, 0, textureVertices);

    //启用ATTRIB_VERTEX属性的数据,默认是关闭的
    glEnableVertexAttribArray(kVertexInLocation);
    //启用ATTRIB_TEXTURE属性的数据,默认是关闭的
    glEnableVertexAttribArray(kTextureInLocation);
    //分别创建y,u,v纹理对象并保存分量的索引值
    for (int index=0; index<3; index++) {
        _yuvTexture[index] = new QOpenGLTexture(QOpenGLTexture::Target2D);
        _yuvTexture[index]->create();
        _yuvID[index] = _yuvTexture[index]->textureId();
    }
}
void XXvideoWidget::resizeGL(int w, int h){
    glViewport(0, 0, (GLint)w*_pixelRatio, (GLint)h*_pixelRatio);
}
void XXvideoWidget::paintGL(){
    QMutexLocker locker(&_yuvBufferMutex);

    // 底层黑
    glViewport(0, 0, this->width()*_pixelRatio, this->height()*_pixelRatio);
    glClearColor(0,0,0,0);
    // 如果没有数据，则直接渲染一个黑屏
    if(!_isHasData){
        return;
    }

    // 通过分辨率调整渲染区域
    auto rect = adjustRect(_videoSize,this->rect());
    glViewport(rect.left()*_pixelRatio, rect.top()*_pixelRatio, rect.size().width()*_pixelRatio, rect.size().height()*_pixelRatio);

    //加载y数据纹理
    //激活纹理单元GL_TEXTURE0
    glActiveTexture(GL_TEXTURE0);
    //使用来自y数据生成纹理
    glBindTexture(GL_TEXTURE_2D, _yuvID[0]);
    //使用内存中m_pBufYuv420p数据创建真正的y数据纹理
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, _videoSize.width(), _videoSize.height(), 0, GL_RED, GL_UNSIGNED_BYTE, _yuvBuffer[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //加载u数据纹理
    glActiveTexture(GL_TEXTURE1);//激活纹理单元GL_TEXTURE1
    glBindTexture(GL_TEXTURE_2D, _yuvID[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, _videoSize.width()/2, _videoSize.height()/2, 0, GL_RED, GL_UNSIGNED_BYTE, _yuvBuffer[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //加载v数据纹理
    glActiveTexture(GL_TEXTURE2);//激活纹理单元GL_TEXTURE2
    glBindTexture(GL_TEXTURE_2D, _yuvID[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, _videoSize.width()/2, _videoSize.height()/2, 0, GL_RED, GL_UNSIGNED_BYTE, _yuvBuffer[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //指定纹理要使用新值 只能用0,1,2等表示纹理单元的索引，这是opengl不人性化的地方
    //0对应纹理单元GL_TEXTURE0，1对应纹理单元GL_TEXTURE1，2对应纹理的单元GL_TEXTURE2
    glUniform1i(_yuvTextureUniform[0], 0);
    //指定u纹理要使用新值
    glUniform1i(_yuvTextureUniform[1], 1);
    //指定v纹理要使用新值
    glUniform1i(_yuvTextureUniform[2], 2);
    //使用顶点数组方式绘制图形
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void XXvideoWidget::setYuvBuffer(qint8 *yuv[], const QSize &videoSize){
    QMutexLocker locker(&_yuvBufferMutex);
    for (int index=0; index<3; index++) {
        if(nullptr == _yuvBuffer[index]){
           return;
        }

        int length = videoSize.width()*videoSize.height();
        memcpy(_yuvBuffer[index],yuv[index],0==index?length:(length/4));
    }
    _isHasData = true;
    _videoSize = videoSize;
    this->update();
}
bool XXvideoWidget::getYuvBuffer(qint8 *yuv[], QSize &videoSize){
    QMutexLocker locker(&_yuvBufferMutex);
    for(int index=0; index<3; index++){
        if(nullptr == yuv[index] || nullptr == _yuvBuffer[index]){
            return false;
        }

        int length = _videoSize.width()*_videoSize.height();
        memcpy(yuv[index],_yuvBuffer[index],0==index?length:(length/4));
    }
    videoSize = _videoSize;
    return true;
}
bool XXvideoWidget::getYuvBuffer(qint8 *yuv, QSize &videoSize){
    QMutexLocker locker(&_yuvBufferMutex);
    int offset = 0;
    for(int index=0; index<3; index++){
        if(nullptr == _yuvBuffer[index]){
            return false;
        }

        int length = _videoSize.width()*_videoSize.height();
        int plength = 0==index?length:(length/4);
        memcpy(yuv+offset,_yuvBuffer[index],plength);
        offset += plength;
    }
    videoSize = _videoSize;
    return true;
}
void XXvideoWidget::freeYuvBuffer(){
    QMutexLocker locker(&_yuvBufferMutex);
    for (int index=0; index<3; index++) {
        if(nullptr == _yuvBuffer[index]){
           return;
        }
        free(_yuvBuffer[index]);
        _yuvBuffer[index] = nullptr;
		_isHasData = false;
    }
}
QRect XXvideoWidget::adjustRect(const QSize &size, const QRect &at){
    qreal ratio = (qreal)size.width()/(qreal)size.height();

	int widthAfterRatio = at.height() * ratio;
    if(widthAfterRatio <= at.width()){
        int dx = (at.width() - widthAfterRatio) / 2;
        return QRect(at.x()+dx,at.y(),widthAfterRatio,at.height());
    }

    int heightAfterRatio = at.width() / ratio;
    if(heightAfterRatio <= at.height()){
        int dy = (at.height() - heightAfterRatio) / 2;
        return QRect(at.x(),at.y()+dy,at.width(),heightAfterRatio);
    }

	QRect rect(0, 0, 50, 50);
	rect.moveCenter(at.center());
    return rect;
}
