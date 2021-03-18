//
//  XXyuvView.m
//  XXapp
//
//  Created by VINSON on 2019/5/10.
//  Copyright © 2019 VINSON. All rights reserved.
//

#import "XXyuvView.h"
#import "../XXocDefine.h"

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


@interface XXyuvView()
@property (nonatomic,assign) BOOL isInited;                 // 初始化标记
@property (nonatomic,weak) CAMetalLayer *metalLayer;
//@property (nonatomic,assign) vector_uint2 drawableSize;     // 可绘制大小

@property (nonatomic,strong) id<MTLRenderPipelineState> renderPipelineState;    // 渲染管道
@property (nonatomic,strong) id<MTLCommandQueue> commandQueue;                  // 渲染命令队列

@property (nonatomic,assign) CVPixelBufferRef pixelBufferRef;       // 目标数据
@property (nonatomic,assign) CVMetalTextureCacheRef textureCache;   // 纹理缓存
@property (nonatomic, strong) id<MTLBuffer> verticesBuffer;         // 顶点坐标
@property (nonatomic, assign) NSUInteger numOfVertices;             // 顶点坐标数量
@property (nonatomic, strong) id<MTLBuffer> convertMatrixBuffer;    // 转换矩阵
@end

@implementation XXyuvView
- (instancetype)initWithCoder:(NSCoder *)coder{
    self = [super initWithCoder:coder];
    if (self) {
        _isInited = NO;
        _metalLayer = (CAMetalLayer*)self.layer;
    }
    return self;
}

- (void) setData:(NSData*)data Width:(int)width Height:(int)height{
    if (!_isInited) {
        _isInited = [self initMetal];
    }
    if (!_isInited) {
        NSLog(@"[XXyuvView] [%d] failure to init metal", __LINE__);
        return;
    }
    
    const char* bytes = (const char*)data.bytes;
    CVPixelBufferRef pixelBuffer = [self pixelBufferRefFromYuv420Data:bytes Width:width Height:height];
    [self renderWithPixelBuffer:pixelBuffer];
}
- (void) setImageBufferRef:(CVImageBufferRef)bufferRef{
    if (!_isInited) {
        _isInited = [self initMetal];
    }
    if (!_isInited) {
        NSLog(@"[XXyuvView] [%d] failure to init metal", __LINE__);
        return;
    }
    
    [self renderWithPixelBuffer:bufferRef];
}
- (void) clear{
    
}

- (BOOL) initMetal{
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    _commandQueue = [device newCommandQueue];
    id<MTLLibrary> library  = [device newDefaultLibrary];
    if (nil == library) {
        NSLog(@"[XXyuvView] [%d] could not find anyone file with .metal", __LINE__);
        return NO;
    }
    
    id<MTLFunction> vertexFunction      = [library newFunctionWithName:VERTEX_FUNCTION_NAME];
    id<MTLFunction> fragmentFunction    = [library newFunctionWithName:FRAGMENT_FUNCTION_NAME];
    if (nil == vertexFunction || nil == fragmentFunction) {
        return NO;
    }
    
    MTLRenderPipelineDescriptor *descriptor     = [[MTLRenderPipelineDescriptor alloc] init];
    descriptor.vertexFunction                   = vertexFunction;
    descriptor.fragmentFunction                 = fragmentFunction;
    descriptor.colorAttachments[0].pixelFormat  = self.colorPixelFormat; // 设置颜色格式
    
    // 创建图形渲染管道，耗性能操作不宜频繁调用
    _renderPipelineState    = [device newRenderPipelineStateWithDescriptor:descriptor error:NULL];
    // CommandQueue是渲染指令队列，保证渲染指令有序地提交到GPU
    _commandQueue           = [device newCommandQueue];
    
    // 创建顶点缓存
    _verticesBuffer = [device newBufferWithBytes:quadVertices length:sizeof(quadVertices) options:MTLResourceStorageModeShared];
    _numOfVertices  = sizeof(quadVertices) / sizeof(Vertex);
    
    // 创建转换矩阵缓存
    _convertMatrixBuffer = [device newBufferWithBytes:&convertMatrix length:sizeof(ConvertMatrix) options:MTLResourceStorageModeShared];
    
    self.device = device;
    self.framebufferOnly = NO;
    self.autoResizeDrawable = NO;
    return YES;
}
- (void) renderWithPixelBuffer:(CVPixelBufferRef)buffer {
    if (!_isInited || nil == buffer) { return; }
    
    // 每次渲染都要单独创建一个CommandBuffer
    id<MTLCommandBuffer> commandBuffer              = [_commandQueue commandBuffer];
    MTLRenderPassDescriptor *renderPassDescriptor   = self.currentRenderPassDescriptor; // MTLRenderPassDescriptor描述一系列attachments的值，类似GL的FrameBuffer；同时也用来创建MTLRenderCommandEncoder
    if (nil == renderPassDescriptor) {
        return;
    }
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.0, 0.5, 0.5, 1.0f);                                   // 设置默认颜色
    
    id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];            // 编码绘制指令的Encoder
    [renderEncoder setViewport:(MTLViewport){0.0, 0.0, self.drawableSize.width, self.drawableSize.height, -1.0, 1.0 }];   // 设置显示区域
    [renderEncoder setRenderPipelineState:_renderPipelineState];                                                                    // 设置渲染管道，以保证顶点和片元两个shader会被调用
    [renderEncoder setVertexBuffer:_verticesBuffer offset:0 atIndex:0];          // 设置顶点缓存

    [self renderPixelBufferRefToEncoder:renderEncoder PixelBufferRef:_pixelBufferRef];
    [renderEncoder setFragmentBuffer:_convertMatrixBuffer offset:0 atIndex:0];   // 设置转换矩阵
    [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:_numOfVertices]; // 绘制
    
    [renderEncoder endEncoding];                            // 结束
    [commandBuffer presentDrawable:self.currentDrawable];   // 显示
    [commandBuffer commit];                                 // 提交；
}
- (CVPixelBufferRef) pixelBufferRefFromYuv420Data:(const char*)data Width:(size_t)width Height:(size_t)height{
    NSDictionary *pixelBufferAttributes = [[NSDictionary alloc] init]; //[NSDictionary dictionaryWithObjectsAndKeys:
                                           //                                           [NSDictionary dictionary],kCVPixelBufferIOSurfacePropertiesKey,
                                           //nil];
    
    CVPixelBufferRef pixelBuffer;
    CVPixelBufferCreate(NULL, width, height, kCVPixelFormatType_420YpCbCr8PlanarFullRange, (__bridge CFDictionaryRef)(pixelBufferAttributes), &pixelBuffer);
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    
    size_t d = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
    const char* src = data;
    unsigned char* dst = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
    
    for (unsigned int rIdx = 0; rIdx < height; ++rIdx, dst += d, src += width) {
        memcpy(dst, src, width);
    }
    
    d = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);
    dst = (unsigned char *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
    height = height >> 1;
    for (unsigned int rIdx = 0; rIdx < height; ++rIdx, dst += d, src += width) {
        memcpy(dst, src, width);
    }
    
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    return pixelBuffer;
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
@end
