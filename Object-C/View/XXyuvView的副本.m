//
//  XXyuvView.m
//  XXapp
//
//  Created by VINSON on 2019/5/10.
//  Copyright © 2019 VINSON. All rights reserved.
//

#import "XXyuvView.h"
#import "../XXocDefine.h"

#define SHOW_YUV

@interface XXyuvView(){
    id<MTLComputePipelineState> _computePipelineState;
    id<MTLCommandQueue> _commandQueue;
    CVMetalTextureCacheRef _textureCache;
}
@property (nonatomic,assign) BOOL isInitMetal;          // 是否已经初始化
@property (nonatomic,weak) CAMetalLayer *metalLayer;
@property (nonatomic,assign) int screenWidth;           // 屏幕宽度
@property (nonatomic,assign) int screenHeight;          // 屏幕高度

@property (nonatomic,assign) size_t currentPixelWidth;         // 当前数据的宽度
@property (nonatomic,assign) size_t currentPixelHeight;        // 当前数据的高度
@end

@implementation XXyuvView
- (instancetype)initWithCoder:(NSCoder *)coder{
    self = [super initWithCoder:coder];
    if (self) {
        _isInitMetal = NO;
        _metalLayer = (CAMetalLayer*)self.layer;
        _screenWidth = XXOC_SCREEN_WIDTH;
        _screenHeight = XXOC_SCREEN_HEIGHT;
        _currentPixelWidth = 0;
        _currentPixelHeight = 0;
    }
    return self;
}

- (void) setData:(NSData*)data Width:(int)width Height:(int)height{
    if (!_isInitMetal) {
        _isInitMetal = [self initMetal];
    }
    if (!_isInitMetal) {
        NSLog(@"[XXyuvView] [%d] failure to init metal", __LINE__);
        return;
    }
    
    const char* bytes = (const char*)data.bytes;
    CVPixelBufferRef pixelBuffer = [self pixelBufferRefFromYuv420Data:bytes Width:width Height:height];
    [self renderWithPixelBuffer:pixelBuffer];
}
- (void) setImageBufferRef:(CVImageBufferRef)bufferRef{
    if (!_isInitMetal) {
        _isInitMetal = [self initMetal];
    }
    if (!_isInitMetal) {
        NSLog(@"[XXyuvView] [%d] failure to init metal", __LINE__);
        return;
    }
    
    size_t pixelWidth = CVPixelBufferGetWidth(bufferRef);
    size_t pixelHeight = CVPixelBufferGetHeight(bufferRef);
    if (pixelWidth != _currentPixelWidth || pixelHeight != _currentPixelHeight) {
        _currentPixelWidth = pixelWidth;
        _currentPixelHeight = pixelHeight;
        self.drawableSize = CGSizeMake(pixelWidth, pixelHeight);
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
    
    /*这里使用的是计算管线，需要在metal文件中定义kernel关键字的yuvToRGB方法
     还可以使用渲染管线[device newRenderPipelineStateWithDescriptor:error:]
     需要创建MTLRenderPipelineDescriptor然后对vertexFunction和fragmentFunction进行赋值，分别对应的metal文件中vertex关键字的顶点着色器和fragment关键字的片段着色器
     */
#ifdef SHOW_YUV
    id<MTLFunction> function = [library newFunctionWithName:@"yuv_rgb"];
#else
    id<MTLFunction> function = [library newFunctionWithName:@"gamma_filter"];
#endif
    NSError * error = NULL;
    _computePipelineState = [device newComputePipelineStateWithFunction:function error:&error];
    CVReturn ret = CVMetalTextureCacheCreate(kCFAllocatorDefault, nil, device, nil, &_textureCache);
    if (ret != kCVReturnSuccess) {
        NSLog(@"[XXyuvView] [%d] fail to create texture cache", __LINE__);
        return NO;
    }
    
    self.device = device;
    self.framebufferOnly = NO;
    self.autoResizeDrawable = NO;
    return YES;
}
- (void)renderWithPixelBuffer:(CVPixelBufferRef)buffer {
    if (buffer == nil) return;
    CVMetalTextureRef y_texture ;
    //获取pixelbuffer中y数据的宽和高，然后创建包含y数据的MetalTexture，注意pixelformat为MTLPixelFormatR8Unorm
    size_t y_width = CVPixelBufferGetWidthOfPlane(buffer, 0);
    size_t y_height = CVPixelBufferGetHeightOfPlane(buffer, 0);
    CVReturn ret = CVMetalTextureCacheCreateTextureFromImage(kCFAllocatorDefault, _textureCache, buffer, nil, MTLPixelFormatR8Unorm, y_width, y_height, 0, &y_texture);
    if (ret != kCVReturnSuccess) {
        NSLog(@"[XXyuvView] [%d] fail to create texture cache", __LINE__);
        return;
    }
    
    id<MTLTexture> y_inputTexture = CVMetalTextureGetTexture(y_texture);
    if (y_inputTexture == nil) {
        NSLog(@"[XXyuvView] [%d] fail to create metal texture", __LINE__);
        return;
    }
    
    CVMetalTextureRef uv_texture ;
    //获取pixelbuffer中uv数据的宽和高，然后创建包含uv数据的MetalTexture，注意pixelformat为MTLPixelFormatRG8Unorm
    size_t uv_width = CVPixelBufferGetWidthOfPlane(buffer, 1);
    size_t uv_height = CVPixelBufferGetHeightOfPlane(buffer, 1);
    ret = CVMetalTextureCacheCreateTextureFromImage(kCFAllocatorDefault, _textureCache, buffer, nil, MTLPixelFormatRG8Unorm, uv_width, uv_height, 1, &uv_texture);
    if (ret != kCVReturnSuccess) {
        NSLog(@"[XXyuvView] [%d] fail to create texture cache", __LINE__);
        return;
    }
    id<MTLTexture> uv_inputTexture = CVMetalTextureGetTexture(uv_texture);
    if (uv_inputTexture == nil) {
        NSLog(@"[XXyuvView] [%d] fail to create metal texture", __LINE__);
        return;
    }
    
    CAMetalLayer * metalLayer           = _metalLayer;//(CAMetalLayer *)self.layer;
    id<CAMetalDrawable> drawable        = metalLayer.nextDrawable;
    id<MTLCommandBuffer> commandBuffer  = [_commandQueue commandBuffer];
    id<MTLComputeCommandEncoder> computeCommandEncoder = [commandBuffer computeCommandEncoder];
    [computeCommandEncoder setComputePipelineState:_computePipelineState];
    //把包含y数据的texture、uv数据的texture和承载渲染图像的texture传入
#ifdef SHOW_YUV
    [computeCommandEncoder setTexture:y_inputTexture atIndex:0];
    [computeCommandEncoder setTexture:uv_inputTexture atIndex:1];
    [computeCommandEncoder setTexture:drawable.texture atIndex:2];
#else
    [computeCommandEncoder setTexture:y_inputTexture atIndex:0];
    [computeCommandEncoder setTexture:drawable.texture atIndex:1];
#endif
    
    MTLSize threadgroupSize = MTLSizeMake(16, 16, 1);
    MTLSize threadgroupCount = MTLSizeMake((y_inputTexture.width / threadgroupSize.width)+1, (y_inputTexture.height/ threadgroupSize.height)+1, 1);
    [computeCommandEncoder dispatchThreadgroups:threadgroupCount threadsPerThreadgroup: threadgroupSize];
    [computeCommandEncoder endEncoding];
    
    [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> _Nonnull cmdBuffer) {
        CVBufferRelease(y_texture);   //销毁texture防止内存泄露
        CVBufferRelease(uv_texture);
    }];
    [commandBuffer presentDrawable:drawable];
    [commandBuffer commit];
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
@end
