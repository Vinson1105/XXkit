//
//  XXmtkRgbDelegate.m
//  XXapp
//
//  Created by VINSON on 2019/6/14.
//  Copyright © 2019 VINSON. All rights reserved.
//

#import "XXmtkRgbDelegate.h"
#define VERTEX_FUNCTION_NAME @"common_vertex"
#define FRAGMENT_FUNCTION_NAME @"rgb_fragment"

typedef struct{
    vector_float4 position;     // 顶点坐标
    vector_float2 coordinate;   // 纹理坐标
} Vertex;

static const Vertex quadVertices[] =
{   // 顶点坐标，分别是x、y、z、w；    纹理坐标，x、y；
    { {  1.0, -1.0, 0.0, 1.0 },  { 1.f, 1.f } },
    { { -1.0, -1.0, 0.0, 1.0 },  { 0.f, 1.f } },
    { { -1.0,  1.0, 0.0, 1.0 },  { 0.f, 0.f } },
    
    { {  1.0, -1.0, 0.0, 1.0 },  { 1.f, 1.f } },
    { { -1.0,  1.0, 0.0, 1.0 },  { 0.f, 0.f } },
    { {  1.0,  1.0, 0.0, 1.0 },  { 1.f, 0.f } },
};

@interface XXmtkRgbDelegate() <MTKViewDelegate>
@property (nonatomic,assign) BOOL isInited;                 // 初始化标记
@property (nonatomic,weak) MTKView *mtkView;                // 目标MTKView
@property (nonatomic,assign) vector_uint2 drawableSize;     // 可绘制大小

@property (nonatomic,strong) id<MTLRenderPipelineState> renderPipelineState;    // 渲染管道
@property (nonatomic,strong) id<MTLCommandQueue> commandQueue;                  // 渲染命令队列

@property (nonatomic,strong) NSCondition *pixelBufferCondition;
@property (nonatomic,assign) CVPixelBufferRef pixelBufferRef;           // 目标数据
@property (nonatomic,assign) CVMetalTextureCacheRef textureCacheRef;    // 纹理缓存
@property (nonatomic, strong) id<MTLBuffer> verticesBuffer;             // 顶点坐标
@property (nonatomic, assign) NSUInteger numOfVertices;                 // 顶点坐标数量
@end

@implementation XXmtkRgbDelegate
#pragma mark - 构建析构
- (instancetype)initWithMTKView:(MTKView *)view{
    self = [super init];
    if (self) {
        _isInited           = NO;
        _mtkView            = view;
        _mtkView.device     = MTLCreateSystemDefaultDevice();
        _mtkView.delegate   = self;
        _drawableSize       = (vector_uint2){_mtkView.drawableSize.width, _mtkView.drawableSize.height};
        CVMetalTextureCacheCreate(NULL, NULL, _mtkView.device, NULL, &_textureCacheRef);
        
        _pixelBufferRef         = nil;
        _pixelBufferCondition   = [[NSCondition alloc] init];
        
        _isInited = [self initMetal];
    }
    return self;
}
- (void)dealloc{
    if(nil != _pixelBufferRef){
        CVPixelBufferRelease(_pixelBufferRef);
    }
}

- (void) setPixelBufferRef:(CVPixelBufferRef)bufferRef{
    if (nil == _mtkView)    { return; }
    if (!_isInited)         { return; }
    
    [_pixelBufferCondition lock];
    if (nil != _pixelBufferRef) {
        CVPixelBufferRelease(_pixelBufferRef);
    }
    _pixelBufferRef = CVPixelBufferRetain(bufferRef);
    [_pixelBufferCondition unlock];
    NSLog(@"setPixelBufferRef");
}
- (void) clear{
    [_pixelBufferCondition lock];
    if (nil != _pixelBufferRef) {
        CVPixelBufferRelease(_pixelBufferRef);
        _pixelBufferRef = nil;
    }
    [_pixelBufferCondition unlock];
}

#pragma mark - 私有函数: <初始化Metal相关变量> <>
- (BOOL) initMetal{
    // 管道
    id<MTLLibrary> defaultLibrary = [_mtkView.device newDefaultLibrary]; // .metal
    id<MTLFunction> vertexFunction = [defaultLibrary newFunctionWithName:VERTEX_FUNCTION_NAME]; // 顶点shader，vertexShader是函数名
    id<MTLFunction> fragmentFunction = [defaultLibrary newFunctionWithName:FRAGMENT_FUNCTION_NAME]; // 片元shader，samplingShader是函数名

    MTLRenderPipelineDescriptor *pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
    pipelineStateDescriptor.vertexFunction = vertexFunction;
    pipelineStateDescriptor.fragmentFunction = fragmentFunction;
    pipelineStateDescriptor.colorAttachments[0].pixelFormat = self.mtkView.colorPixelFormat;
    _renderPipelineState = [self.mtkView.device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor
                                                                             error:NULL]; // 创建图形渲染管道，耗性能操作不宜频繁调用
    _commandQueue = [self.mtkView.device newCommandQueue]; // CommandQueue是渲染指令队列，保证渲染指令有序地提交到GPU

    // 顶点
    _verticesBuffer = [self.mtkView.device newBufferWithBytes:quadVertices length:sizeof(quadVertices) options:MTLResourceStorageModeShared]; // 创建顶点缓存
    _numOfVertices = sizeof(quadVertices) / sizeof(Vertex); // 顶点个数
    return YES;
}

#pragma mark - 协议函数: <MTKViewDelegate>
- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size {
    _drawableSize = (vector_uint2){size.width, size.height};
}
- (void)drawInMTKView:(MTKView *)view {
    if (!_isInited || nil == _pixelBufferRef) { return; }
    MTLRenderPassDescriptor *renderPassDescriptor = view.currentRenderPassDescriptor;
    if (nil == renderPassDescriptor) {
        return;
    }
    
    id<MTLCommandBuffer> commandBuffer  = [_commandQueue commandBuffer];
    id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];    // 编码绘制指令的Encoder
    [renderEncoder setViewport:(MTLViewport){0.0, 0.0, _drawableSize.x, _drawableSize.y, -1.0, 1.0 }];                      // 设置显示区域
    [renderEncoder setRenderPipelineState:_renderPipelineState];
    
    [_pixelBufferCondition lock];
    [self renderPixelBufferRef:_pixelBufferRef ToEncoder:renderEncoder];
    [_pixelBufferCondition unlock];
    
    [renderEncoder setVertexBuffer:_verticesBuffer offset:0 atIndex:0]; // 设置顶点缓存
    [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:_numOfVertices]; // 绘制
    
    [renderEncoder endEncoding];                            // 结束
    [commandBuffer presentDrawable:view.currentDrawable];   // 显示
    [commandBuffer commit];
}
- (void) renderPixelBufferRef:(CVPixelBufferRef)bufferRef ToEncoder:(id<MTLRenderCommandEncoder>)encoder{
    size_t width    = CVPixelBufferGetWidth(bufferRef);
    size_t height   = CVPixelBufferGetHeight(bufferRef);
    CVMetalTextureRef metalTextureRef   = nil;
    id<MTLTexture> texture              = nil;

    CVReturn status = CVMetalTextureCacheCreateTextureFromImage(nil, _textureCacheRef, _pixelBufferRef, nil, MTLPixelFormatBGRA8Unorm, width, height, 0, &metalTextureRef);
    if(status == kCVReturnSuccess){
        texture = CVMetalTextureGetTexture(metalTextureRef); // 转成Metal用的纹理
        CFRelease(metalTextureRef);
    }
    
    if (nil != texture) {
        [encoder setFragmentTexture:texture atIndex:0]; // 设置纹理
    }
}
@end
