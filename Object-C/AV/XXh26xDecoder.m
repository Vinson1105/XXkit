//
//  XXh26xDecoder.m
//  NVSDK
//
//  Created by VINSON on 2019/7/29.
//  Copyright © 2019 macrovideo. All rights reserved.
//

#import "XXh26xDecoder.h"
#include "XXnaluSpliter.h"

//#define USING_DECODETIME_MARKET
#define DEFAULT_FRAME_BUFFER_SIZE   512*1024
#define DEFAULT_PIXEL_FORMAT        kCVPixelFormatType_24RGB

#pragma mark - C函数声明
static void decodeOutputCallBack(void *decompressionOutputRefCon,
                                 void *sourceFrameRefCon,
                                 OSStatus status,
                                 VTDecodeInfoFlags infoFlags,
                                 CVImageBufferRef pixelBuffer,
                                 CMTime presentationTimeStamp,
                                 CMTime presentationDuration);

#
#pragma mark - 类: <XXdecoderBase>
#
@interface XXdecoderBase()
@property (nonatomic,weak) id<XXdecoderDerived> derivedClass;
@end
@implementation XXdecoderBase
- (instancetype)init{
    self = [super init];
    if (self) {
        _derivedClass = self;
    }
    return self;
}
- (instancetype) initWithName:(NSString*)name{
    self = [self init];
    if (self) {
        _name   = name;
    }
    return self;
}
- (CVPixelBufferRef) decodeData:(uint8_t*)data Length:(int)length{
    if ([_derivedClass respondsToSelector:@selector(toDecodeData:Length:)]) {
        return [_derivedClass toDecodeData:data Length:length];
    }
    return nil;
}
- (BOOL) decodeSrcData:(uint8_t*)srcData SrcLength:(int)srcLength DestBuffer:(uint8_t*)destBuffer DestLength:(int*)destLength{
    if ([_derivedClass respondsToSelector:@selector(toDecodeSrcData:SrcLength:DestBuffer:DestLength:)]) {
        return [_derivedClass toDecodeSrcData:srcData SrcLength:srcLength DestBuffer:destBuffer DestLength:destLength];
    }
    return NO;
}
@end

#
#pragma mark - 类: <XXh26xDecoder>
#
@interface XXh26xDecoder()<XXdecoderDerived>
@property (nonatomic,assign) BOOL isInited;
@property (nonatomic,assign) BOOL isH264;
@property (nonatomic,assign) XXnaluSpliterHandle spliterHandle;

@property (nonatomic,assign) CMVideoFormatDescriptionRef descriptionRef;
@property (nonatomic,assign) VTDecompressionSessionRef sessionRef;
@property (nonatomic,assign) CVPixelBufferRef pixelBufferRef;

@property (nonatomic,strong) NSData *vps;
@property (nonatomic,strong) NSData *sps;
@property (nonatomic,strong) NSData *pps;
@property (nonatomic,assign) uint8_t *frameBuffer;
@end
@implementation XXh26xDecoder
#pragma mark - 构建析构
+(XXh26xDecoder*) decoderWithName:(NSString*)name IsH264:(BOOL)isH264{
    XXh26xDecoder *decoder  = [[XXh26xDecoder alloc] initWithName:name IsH264:isH264];
    return decoder;
}
- (instancetype) initWithName:(NSString*)name IsH264:(BOOL)isH264{
    self = [super initWithName:name];
    if (self) {
        _isInited       = NO;
        _isH264         = isH264;
        _spliterHandle  = xxnaluSpliter_create(10);
        
        _sessionRef     = nil;
        _descriptionRef = nil;
        _pixelBufferRef = nil;
        
        _vps    = nil;
        _sps    = nil;
        _pps    = nil;
        _frameBuffer    = (uint8_t*)malloc(DEFAULT_FRAME_BUFFER_SIZE);
    }
    return self;
}
- (void)dealloc{
    xxnaluSpliter_free(&_spliterHandle);
    if(nil != _pixelBufferRef)  { CFRelease(_pixelBufferRef);   _pixelBufferRef = nil; }
    if(nil != _descriptionRef)  { CFRelease(_descriptionRef);   _descriptionRef = nil; }
    if(nil != _sessionRef)      { CFRelease(_sessionRef);       _sessionRef = nil; }
    free(_frameBuffer);
    _frameBuffer = NULL;
}

#pragma mark - 属性读写: <PixelBufferRef>
- (void)setPixelBufferRef:(CVPixelBufferRef)pixelBufferRef{
    if (nil != _pixelBufferRef) {
        CVPixelBufferRelease(_pixelBufferRef);
    }
    if (nil != pixelBufferRef) {
        _pixelBufferRef = CVPixelBufferRetain(pixelBufferRef);
    }
    else{
        _pixelBufferRef = pixelBufferRef;
    }
}

#pragma mark - 协议函数: <XXdecoderDerived>
- (CVPixelBufferRef)toDecodeData:(uint8_t *)data Length:(int)length{
    return [self decodeData:data Length:length];
}
- (BOOL) toDecodeSrcData:(uint8_t*)srcData SrcLength:(int)srcLength DestBuffer:(uint8_t*)destBuffer DestLength:(int*)destLength{
    CVPixelBufferRef bufferRef = [self decodeData:srcData Length:srcLength];
    if (nil == bufferRef) {
        return NO;
    }

    CVPixelBufferLockBaseAddress(bufferRef, kCVPixelBufferLock_ReadOnly);
    void *dataAddress   = CVPixelBufferGetBaseAddressOfPlane(bufferRef, 0);
    *destLength         = (int)CVPixelBufferGetDataSize(bufferRef);
    memcpy(destBuffer, dataAddress, *destLength);
    CVPixelBufferUnlockBaseAddress(bufferRef,0);
    return YES;
}

#pragma mark - 私有函数: <初始化解码器> <创建解码器描述文件> <创建解码器会话> <采样样本转换> <提取媒体信息(VPS/SPS/PPS)> <提取画面数据(I/P)>
- (BOOL)initDecode{
    [self mediaInfoSpliterHandle:_spliterHandle VPS:&_vps SPS:&_sps PPS:&_pps];
    if ((!_isH264 && nil == _vps) || nil == _sps || nil == _pps) {
        GWXLog(@"[XXh26xDecorder] 错误(%d) %d %d %d", __LINE__, (!_isH264 && nil == _vps), nil == _sps, nil == _pps);
        return NO;
    }
    
    if(nil != _descriptionRef){ CFRelease(_descriptionRef); _descriptionRef = nil; }
    _descriptionRef = [self createDescriptionUsingVPS:_vps SPS:_sps PPS:_pps IsH264:_isH264];
    if (nil == _descriptionRef) {
        GWXLog(@"[XXh26xDecorder] 错误(%d)", __LINE__);
        return NO;
    }
    
    if(nil != _sessionRef){ CFRelease(_sessionRef); _sessionRef = nil; }
    _sessionRef = [self createSessionUsingDescriptionRef:_descriptionRef PixelFormatType:DEFAULT_PIXEL_FORMAT OutputCallback:decodeOutputCallBack];
    if (nil == _sessionRef) {
        CFRelease(_descriptionRef);
        _descriptionRef = nil;
        GWXLog(@"[XXh26xDecorder] 错误(%d)", __LINE__);
        return NO;
    }
    return YES;
}
- (CMVideoFormatDescriptionRef) createDescriptionUsingVPS:(NSData*)vps
                                                      SPS:(NSData*)sps
                                                      PPS:(NSData*)pps
                                                   IsH264:(BOOL)isH264{
    CMVideoFormatDescriptionRef descriptionRef  = nil;
    OSStatus status = -1;
    if (isH264) {
        const uint8_t* const parameterSetPointers[2]    = { sps.bytes, pps.bytes };
        const size_t parameterSetSizes[2]               = { sps.length, pps.length };
        status = CMVideoFormatDescriptionCreateFromH264ParameterSets(kCFAllocatorDefault,
                                                                     2,
                                                                     parameterSetPointers,
                                                                     parameterSetSizes,
                                                                     4,
                                                                     &descriptionRef);
    }
    else if(@available(iOS 11.0, *)){
        const uint8_t* const parameterSetPointers[3]    = { vps.bytes, sps.bytes, pps.bytes };
        const size_t parameterSetSizes[3]               = { vps.length, sps.length, pps.length };
        status = CMVideoFormatDescriptionCreateFromHEVCParameterSets(kCFAllocatorDefault,
                                                                     3,
                                                                     parameterSetPointers,
                                                                     parameterSetSizes,
                                                                     4,
                                                                     NULL,
                                                                     &descriptionRef);
    }
    else{
    }
    return noErr == status ? descriptionRef : nil;
}
- (VTDecompressionSessionRef) createSessionUsingDescriptionRef:(CMVideoFormatDescriptionRef)descriptionRef
                                                   PixelFormatType:(int)pixelFormatType
                                                    OutputCallback:(VTDecompressionOutputCallback)outputCallBack{
    CFDictionaryRef dictRef = NULL;
    int32_t value           = pixelFormatType;
    const void *keys[]      = { kCVPixelBufferPixelFormatTypeKey };
    const void *values[]    = { CFNumberCreate(NULL, kCFNumberSInt32Type, &value) };
    dictRef                 = CFDictionaryCreate(NULL, keys, values, 1, NULL, NULL);
    
    VTDecompressionOutputCallbackRecord callBackRecord;
    callBackRecord.decompressionOutputCallback  = outputCallBack;
    callBackRecord.decompressionOutputRefCon    = NULL;
    
    VTDecompressionSessionRef sessionRef = nil;
    OSStatus status = VTDecompressionSessionCreate(kCFAllocatorDefault,
                                                   descriptionRef,
                                                   NULL,
                                                   dictRef,
                                                   &callBackRecord,
                                                   &sessionRef);
    CFRelease(dictRef);
    return noErr == status ? sessionRef : nil;
}
- (BOOL) toSampleBufferRef:(CMSampleBufferRef*)bufferRef FrameData:(uint8_t*)frameData Length:(int)length DescriptionRef:(CMVideoFormatDescriptionRef)descriptionRef{
    CMBlockBufferRef blockBufferRef = NULL;
    OSStatus status  = CMBlockBufferCreateWithMemoryBlock(kCFAllocatorDefault,
                                                          frameData,
                                                          length,
                                                          kCFAllocatorNull,
                                                          NULL,
                                                          0,
                                                          length,
                                                          0,
                                                          &blockBufferRef);
    if (kCMBlockBufferNoErr != status) {
        return NO;
    }
    
    
    CMSampleBufferRef sampleBufferRef   = NULL;
    const size_t sampleSizeArray[]      = { length };
    status = CMSampleBufferCreateReady(kCFAllocatorDefault,
                                       blockBufferRef,
                                       descriptionRef ,
                                       1,
                                       0,
                                       NULL,
                                       1,
                                       sampleSizeArray,
                                       &sampleBufferRef);
    if (kCMBlockBufferNoErr != status) {
        CFRelease(blockBufferRef);
        return NO;
    }
    *bufferRef = (CMSampleBufferRef)CFRetain(sampleBufferRef);
    CFRelease(blockBufferRef);
    CFRelease(sampleBufferRef);
    return YES;
}
- (void) mediaInfoSpliterHandle:(XXnaluSpliterHandle)spliterHandle VPS:(NSData* __strong*)vps SPS:(NSData* __strong*)sps PPS:(NSData* __strong*)pps{
    uint8_t *dataRef    = NULL;
    uint32_t dataLength = 0;
    xxnaluSpliter_vpsRef(spliterHandle, &dataRef, &dataLength, false);
    *vps = (NULL == dataRef || 0 == dataLength) ? nil : [NSData dataWithBytes:dataRef length:dataLength];
    
    dataRef     = NULL;
    dataLength  = 0;
    xxnaluSpliter_spsRef(spliterHandle, &dataRef, &dataLength, false);
    *sps = (NULL == dataRef || 0 == dataLength) ? nil : [NSData dataWithBytes:dataRef length:dataLength];
    
    dataRef     = NULL;
    dataLength  = 0;
    xxnaluSpliter_ppsRef(spliterHandle, &dataRef, &dataLength, false);
    *pps = (NULL == dataRef || 0 == dataLength) ? nil : [NSData dataWithBytes:dataRef length:dataLength];
}
- (void) mediaDataUsingSpliterHandle:(XXnaluSpliterHandle)spliterHandle Buffer:(uint8_t*)buffer Length:(uint32_t*)length{
    uint8_t *dataRef        = NULL;
    uint32_t dataRefLength  = 0;
    
    xxnaluSpliter_iframeRef(_spliterHandle, &dataRef, &dataRefLength, false);
    if (NULL == dataRef || 0 == dataRefLength) {
        xxnaluSpliter_pframeRef(_spliterHandle, &dataRef, &dataRefLength, false);
    }
    if (NULL == dataRef || 0 == dataRefLength) {
        GWXLog(@"[XXh26xDecorder] 错误(%d)", __LINE__);
        *length = 0;
        return;
    }
    
    int len = htonl(dataRefLength);
    memcpy(buffer, &len, 4);
    memcpy(buffer+4, dataRef, dataRefLength);
    *length = dataRefLength + 4;
}

#pragma mark - 私有函数: <解码>
- (CVPixelBufferRef) decodeData:(uint8_t*)data Length:(int)length{
#ifdef USING_DECODETIME_MARKET
    CFAbsoluteTime startTime = CFAbsoluteTimeGetCurrent();
#endif
    if (NULL == data || length <= 0) {
        GWXLog(@"[XXh26xDecorder] 错误(%d)", __LINE__);
        return nil;
    }
    
    int naluCount = xxnaluSpliter_split(_spliterHandle, data, length, _isH264);
    if (naluCount <= 0) {
        GWXLog(@"[XXh26xDecorder] 错误(%d)", __LINE__);
        return nil;
    }
    
    if (!_isInited) {
        if([self initDecode]){
            _isInited = YES;
        }
        else{
            GWXLog(@"[XXh26xDecorder] 错误(%d)", __LINE__);
            return nil;
        }
    }
    
    uint32_t frameLength = 0;
    [self mediaDataUsingSpliterHandle:_spliterHandle Buffer:_frameBuffer Length:&frameLength];
    if (0 == frameLength) {
        GWXLog(@"[XXh26xDecorder] 错误(%d)", __LINE__);
        return nil;
    }
    CMSampleBufferRef sampleBufferRef = nil;
    if(![self toSampleBufferRef:&sampleBufferRef FrameData:_frameBuffer Length:frameLength DescriptionRef:_descriptionRef]){
        GWXLog(@"[XXh26xDecorder] 错误(%d)", __LINE__);
        return nil;
    }
    
    VTDecodeFrameFlags flags = 0;
    VTDecodeInfoFlags flagOut = 0;
    OSStatus status = VTDecompressionSessionDecodeFrame(_sessionRef,
                                                        sampleBufferRef,
                                                        flags,
                                                        (__bridge void*)(self),
                                                        &flagOut);
    
    CFRelease(sampleBufferRef);
    if (noErr != status) {
        switch (status) {
            case kVTInvalidSessionErr:      break;
            case kVTVideoDecoderBadDataErr: break;
            default:break;
        }
        GWXLog(@"[XXh26xDecorder] 错误(%d)", __LINE__);
        return nil;
    }
#ifdef USING_DECODETIME_MARKET
    CFAbsoluteTime timeAfterDecode  = CFAbsoluteTimeGetCurrent();
    GWXLog(@"[XXh26xDecoder] Length:%d Decode:%.3f", length, timeAfterDecode-startTime);
#endif
    return _pixelBufferRef;
}
@end

#pragma mark - C回调函数: <解码回调>
static void decodeOutputCallBack(void *decompressionOutputRefCon,
                                 void *sourceFrameRefCon,
                                 OSStatus status,
                                 VTDecodeInfoFlags infoFlags,
                                 CVImageBufferRef imageBufferRef,
                                 CMTime presentationTimeStamp,
                                 CMTime presentationDuration){
    XXh26xDecoder *decoder = (__bridge XXh26xDecoder*)(sourceFrameRefCon);
    [decoder setPixelBufferRef:imageBufferRef];
}
