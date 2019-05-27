//
//  XXmtkYuvDelegate.m
//  XXapp
//
//  Created by VINSON on 2019/5/17.
//  Copyright © 2019 VINSON. All rights reserved.
//

#import "XXmtkYuvDelegate.h"
#define VERTEX_FUNCTION_NAME @"yuv_vertex"
#define FRAGMENT_FUNCTION_NAME @"yuv_fragment"

typedef struct{
    vector_float4 position;     // 顶点坐标
    vector_float2 coordinate;   // 纹理坐标
} Vertex;

typedef struct {
    matrix_float3x3 matrix;     // 转换矩阵
    vector_float3 offset;       // 转换偏移
} ConvertMatrix;

static const Vertex quadVertices[] = {
    // 顶点坐标，分别是x、y、z、w；    纹理坐标，x、y；
    { {  1.0, -1.0, 0.0, 1.0 },  { 1.f, 1.f } },
    { { -1.0, -1.0, 0.0, 1.0 },  { 0.f, 1.f } },
    { { -1.0,  1.0, 0.0, 1.0 },  { 0.f, 0.f } },
    
    { {  1.0, -1.0, 0.0, 1.0 },  { 1.f, 1.f } },
    { { -1.0,  1.0, 0.0, 1.0 },  { 0.f, 0.f } },
    { {  1.0,  1.0, 0.0, 1.0 },  { 1.f, 0.f } },
};

static const ConvertMatrix convertMatrix = {
    .matrix = (matrix_float3x3){
        (simd_float3){1.0,    1.0,    1.0},
        (simd_float3){0.0,    -0.343, 1.765},
        (simd_float3){1.4,    -0.711, 0.0},
    },
    .offset = (vector_float3){ -(16.0/255.0), -0.5, -0.5},
};

@interface XXmtkYuvDelegate() <MTKViewDelegate>
@property (nonatomic,assign) BOOL isInited;                 // 初始化标记
@property (nonatomic,weak) MTKView *mtkView;                // 目标MTKView
@property (nonatomic,assign) vector_uint2 drawableSize;     // 可绘制大小

@property (nonatomic,strong) id<MTLRenderPipelineState> renderPipelineState;    // 渲染管道
@property (nonatomic,strong) id<MTLCommandQueue> commandQueue;                  // 渲染命令队列

@property (nonatomic,assign) CVPixelBufferRef pixelBufferRef;       // 目标数据
@property (nonatomic,assign) CVMetalTextureCacheRef textureCache;   // 纹理缓存
@property (nonatomic, strong) id<MTLBuffer> verticesBuffer;         // 顶点坐标
@property (nonatomic, assign) NSUInteger numOfVertices;             // 顶点坐标数量
@property (nonatomic, strong) id<MTLBuffer> convertMatrixBuffer;    // 转换矩阵
@property (nonatomic,strong) NSCondition *pixelBufferCondition;
@end

@implementation XXmtkYuvDelegate
#pragma mark - 构建析构
- (instancetype) initWithMTKView:(MTKView*)view{
    self = [super init];
    if (self) {
        _isInited           = NO;
        _mtkView            = view;
        _mtkView.device     = MTLCreateSystemDefaultDevice();
        _mtkView.delegate   = self;
        _drawableSize       = (vector_uint2){_mtkView.drawableSize.width, _mtkView.drawableSize.height};
        CVMetalTextureCacheCreate(NULL, NULL, _mtkView.device, NULL, &_textureCache);
        //_mtkView.drawableSize   = _mtkView.frame.size;
        
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
    NSLog(@"[setPixelBufferRef] %d", [[NSThread currentThread] isMainThread]);
    if (nil == _mtkView)    { return; }
    if (!_isInited)         { _isInited = [self initMetal];}
    if (!_isInited)         { return; }
    
    [_pixelBufferCondition lock];
    if (nil != _pixelBufferRef) {
        CVPixelBufferRelease(_pixelBufferRef);
    }
    _pixelBufferRef = CVPixelBufferRetain(bufferRef);
    [_pixelBufferCondition unlock];
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
    // 初始化管道和渲染指令队列
    id<MTLLibrary> defaultLibrary = [self.mtkView.device newDefaultLibrary];    // 工程中需要一个.metal文件作为默认的库函数
    if(nil == defaultLibrary){
        return NO;
    }
    id<MTLFunction> vertexFunction      = [defaultLibrary newFunctionWithName:VERTEX_FUNCTION_NAME];
    id<MTLFunction> fragmentFunction    = [defaultLibrary newFunctionWithName:FRAGMENT_FUNCTION_NAME];
    if (nil == vertexFunction || nil == fragmentFunction) {
        return NO;
    }
    
    MTLRenderPipelineDescriptor *descriptor     = [[MTLRenderPipelineDescriptor alloc] init];
    descriptor.vertexFunction                   = vertexFunction;
    descriptor.fragmentFunction                 = fragmentFunction;
    descriptor.colorAttachments[0].pixelFormat  = _mtkView.colorPixelFormat; // 设置颜色格式
    
    // 创建图形渲染管道，耗性能操作不宜频繁调用
    _renderPipelineState    = [_mtkView.device newRenderPipelineStateWithDescriptor:descriptor error:NULL];
    // CommandQueue是渲染指令队列，保证渲染指令有序地提交到GPU
    _commandQueue           = [_mtkView.device newCommandQueue];
    
    // 创建顶点缓存
    _verticesBuffer = [self.mtkView.device newBufferWithBytes:quadVertices length:sizeof(quadVertices) options:MTLResourceStorageModeShared];
    _numOfVertices  = sizeof(quadVertices) / sizeof(Vertex);
    
    // 创建转换矩阵缓存
    _convertMatrixBuffer = [self.mtkView.device newBufferWithBytes:&convertMatrix length:sizeof(ConvertMatrix) options:MTLResourceStorageModeShared];
    return YES;
}
- (void) renderPixelBufferRefToEncoder:(id<MTLRenderCommandEncoder>)encoder PixelBufferRef:(CVPixelBufferRef)pixelBufferRef{
    for (int index = 0; index < 2; index++) {
        id<MTLTexture> texture      = nil;
        size_t width                = CVPixelBufferGetWidthOfPlane(pixelBufferRef, index);
        size_t height               = CVPixelBufferGetHeightOfPlane(pixelBufferRef, index);
        MTLPixelFormat pixelFormat  = 0 == index ? MTLPixelFormatR8Unorm : MTLPixelFormatRG8Unorm;

        CVMetalTextureRef metalTexture = nil; // CoreVideo的Metal纹理
        CVReturn status = CVMetalTextureCacheCreateTextureFromImage(NULL, _textureCache, pixelBufferRef, NULL, pixelFormat, width, height, index, &metalTexture);
        if(status == kCVReturnSuccess){
            texture = CVMetalTextureGetTexture(metalTexture); // 转成Metal用的纹理
            CFRelease(metalTexture);
        }

        if (nil != texture) {
            [encoder setFragmentTexture:texture atIndex:index];
        }
    }
}
- (CVMetalTextureRef) getTextureWidthPixelBufferRef:(CVPixelBufferRef)pixelBufferRef TextureCache:(CVMetalTextureCacheRef)textureCache PlaneIndex:(int)planeIndex{
    size_t width                = CVPixelBufferGetWidthOfPlane(pixelBufferRef, planeIndex);
    size_t height               = CVPixelBufferGetHeightOfPlane(pixelBufferRef, planeIndex);
    MTLPixelFormat pixelFormat  = 0 == planeIndex ? MTLPixelFormatR8Unorm : MTLPixelFormatRG8Unorm;
    
    CVMetalTextureRef metalTexture = nil; // CoreVideo的Metal纹理
    CVReturn status = CVMetalTextureCacheCreateTextureFromImage(NULL, textureCache, pixelBufferRef, NULL, pixelFormat, width, height, planeIndex, &metalTexture);
    if(status != kCVReturnSuccess){
        return nil;
    }
    return metalTexture;
}

#pragma mark - 协议函数: <MTKViewDelegate>
- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size {
    _drawableSize = (vector_uint2){size.width, size.height};
}
- (void)drawInMTKView:(MTKView *)view {
    if (!_isInited || nil == _pixelBufferRef) { return; }

    // 每次渲染都要单独创建一个CommandBuffer
    MTLRenderPassDescriptor *renderPassDescriptor   = view.currentRenderPassDescriptor; // MTLRenderPassDescriptor描述一系列attachments的值，类似GL的FrameBuffer；同时也用来创建MTLRenderCommandEncoder
    if (nil == renderPassDescriptor) {
        return;
    }
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.5, 0.5, 0.5, 1.0f);                           // 设置默认颜色
    
    id<MTLCommandBuffer> commandBuffer              = [_commandQueue commandBuffer];
    id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];    // 编码绘制指令的Encoder
    [renderEncoder setViewport:(MTLViewport){0.0, 0.0, _drawableSize.x, _drawableSize.y, -1.0, 1.0 }];                      // 设置显示区域
    [renderEncoder setRenderPipelineState:_renderPipelineState];                                                            // 设置渲染管道，以保证顶点和片元两个shader会被调用
    
    [_pixelBufferCondition lock];
    [self renderPixelBufferRefToEncoder:renderEncoder PixelBufferRef:_pixelBufferRef];
    [_pixelBufferCondition unlock];
    [renderEncoder setVertexBuffer:_verticesBuffer offset:0 atIndex:0];                                 // 设置顶点缓存
    [renderEncoder setFragmentBuffer:_convertMatrixBuffer offset:0 atIndex:0];                          // 设置转换矩阵
    [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:_numOfVertices];   // 绘制

    [renderEncoder endEncoding];                            // 结束
    [commandBuffer presentDrawable:view.currentDrawable];   // 显示
    [commandBuffer commit];                                 // 提交；
}
@end
