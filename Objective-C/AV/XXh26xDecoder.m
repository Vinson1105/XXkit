#import "XXh26xDecoder.h"
#import "../../C/XXnaluSpliter.h"

//#define USING_DECODETIME_MARKET
#define DEFAULT_FRAME_BUFFER_SIZE   512*1024

#define OUTPUT_YUV420   /// 是否输出yuv420格式
#ifdef OUTPUT_YUV420
#define DEFAULT_PIXEL_FORMAT        kCVPixelFormatType_420YpCbCr8Planar
#else
#define DEFAULT_PIXEL_FORMAT        kCVPixelFormatType_24RGB
#endif

#pragma mark - C函数声明
static void decodeOutputCallBack(void *decompressionOutputRefCon,
                                 void *sourceFrameRefCon,
                                 OSStatus status,
                                 VTDecodeInfoFlags infoFlags,
                                 CVImageBufferRef pixelBuffer,
                                 CMTime presentationTimeStamp,
                                 CMTime presentationDuration);

#
#pragma mark - 类: <XXh26xDecoder>
#
@interface XXh26xDecoder()
@property (nonatomic,assign) BOOL isInited;     /// 是否已初始化
@property (nonatomic,assign) BOOL isH264;       /// 是否为h264
@property (nonatomic,assign) XXnaluSpliterHandle spliterHandle; /// nalu拆分工具

@property (nonatomic,assign) CMVideoFormatDescriptionRef descriptionRef;    /// 解码器格式描述对象
@property (nonatomic,assign) VTDecompressionSessionRef sessionRef;          /// 解码会话对象
@property (nonatomic,assign) CVPixelBufferRef pixelBufferRef;               /// 解码输出buffer

@property (nonatomic,strong) NSData *vps;           /// 数据中vps（h265特有）
@property (nonatomic,strong) NSData *sps;           /// 数据中sps
@property (nonatomic,strong) NSData *pps;           /// 数据中pps
@property (nonatomic,assign) uint8_t *frameBuffer;
@end
@implementation XXh26xDecoder
#pragma mark - 构建析构
+ (XXh26xDecoder*)decoderWithH264:(BOOL)h264{
    return [[XXh26xDecoder alloc] initWithH264:h264];
}
- (instancetype) initWithH264:(BOOL)h264{
    self = [super init];
    if (self) {
        _isInited       = NO;
        _isH264         = h264;
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
    _spliterHandle = NULL;
    
    if(nil != _pixelBufferRef)  { CFRelease(_pixelBufferRef);   _pixelBufferRef = nil; }
    if(nil != _descriptionRef)  { CFRelease(_descriptionRef);   _descriptionRef = nil; }
    if(nil != _sessionRef)      { CFRelease(_sessionRef);       _sessionRef = nil; }
    
    free(_frameBuffer);
    _frameBuffer = NULL;
}

#pragma mark - 公共接口: <解码>
- (CVPixelBufferRef)decodeData:(uint8_t*)data length:(int)length{
#ifdef USING_DECODETIME_MARKET
    CFAbsoluteTime startTime = CFAbsoluteTimeGetCurrent();
#endif
    
    /// 数据有效性判断
    if (NULL == data || length <= 0) {
        NSLog(@"[XXh26xDecorder] 错误 (LINE:%d).", __LINE__);
        return nil;
    }
    
    /// nalu分割
    int naluCount = xxnaluSpliter_split(_spliterHandle, data, length, _isH264);
    if (naluCount <= 0) {
        NSLog(@"[XXh26xDecorder] 错误 (LINE:%d).", __LINE__);
        return nil;
    }
    
    /// 解码器初始化
    if (!_isInited) {
        if([self initDecode]){
            _isInited = YES;
        }
        else{
            NSLog(@"[XXh26xDecorder] 错误 (LINE:%d).", __LINE__);
            return nil;
        }
    }
    
    /// 取出I帧或者P帧
    uint32_t frameLength = 0;
    [self mediaDataUsingSpliterHandle:_spliterHandle Buffer:_frameBuffer Length:&frameLength];
    if (0 == frameLength) {
        NSLog(@"[XXh26xDecorder] 错误 (LINE:%d).", __LINE__);
        return nil;
    }
    
    /// 将原始数据转换到【CMSampleBufferRef】
    CMSampleBufferRef sampleBufferRef = nil;
    if(![self toSampleBufferRef:&sampleBufferRef FrameData:_frameBuffer Length:frameLength DescriptionRef:_descriptionRef]){
        NSLog(@"[XXh26xDecorder] 错误 (LINE:%d).", __LINE__);
        return nil;
    }
    
    /// 执行解码，现使用的同步解码，在回调中对【self.pixelBufferRef】进行赋值，故在本函数可以直接返回【self.pixelBufferRef】
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
            case kVTInvalidSessionErr:      [self resetDecoder]; break;
            case kVTVideoDecoderBadDataErr: break;
            default:break;
        }
        NSLog(@"[XXh26xDecorder] 错误 status:%d (LINE:%d).", (int)status, __LINE__);
        return nil;
    }
#ifdef USING_DECODETIME_MARKET
    CFAbsoluteTime timeAfterDecode = CFAbsoluteTimeGetCurrent();
    NSLog(@"[XXh26xDecoder] Length:%d Decode:%.3f.", length, timeAfterDecode-startTime);
#endif
    return _pixelBufferRef;
}
- (BOOL)decodeSrcData:(uint8_t*)srcData srcLength:(int)srcLength destBuffer:(uint8_t*)destBuffer destLength:(int*)destLength{
    CVPixelBufferRef bufferRef = [self decodeData:srcData length:srcLength];
    if (nil == bufferRef) {
        return NO;
    }
#ifdef OUTPUT_YUV420
    *destLength = [self yuv420PixelBuffer:bufferRef toBuffer:destBuffer];
#else
    *destLength = [self rgb24PixelBuffer:bufferRef toBuffer:destBuffer];
#endif
    return YES;
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

#pragma mark - 私有函数: <初始化解码器> <创建解码器描述文件> <创建解码器会话> <采样样本转换>
- (void)resetDecoder{
    if(!_isInited)  return;
    _vps = nil; _sps = nil; _pps = nil; _isInited = NO;
}
- (BOOL)initDecode{
    [self mediaInfoSpliterHandle:_spliterHandle vps:&_vps sps:&_sps pps:&_pps];
    if ((!_isH264 && nil == _vps) || nil == _sps || nil == _pps) {
        NSLog(@"[XXh26xDecorder] 错误 %d %d %d (LINE:%d).", (!_isH264 && nil == _vps), nil == _sps, nil == _pps, __LINE__);
        return NO;
    }
    
    if(nil != _descriptionRef){ CFRelease(_descriptionRef); _descriptionRef = nil; }
    _descriptionRef = [self createDescriptionWithVps:_vps sps:_sps pps:_pps h264:_isH264];
    if (nil == _descriptionRef) {
        NSLog(@"[XXh26xDecorder] 错误 (LINE:%d).", __LINE__);
        return NO;
    }
    
    if(nil != _sessionRef){ CFRelease(_sessionRef); _sessionRef = nil; }
    _sessionRef = [self createSessionUsingDescriptionRef:_descriptionRef PixelFormatType:DEFAULT_PIXEL_FORMAT OutputCallback:decodeOutputCallBack];
    if (nil == _sessionRef) {
        CFRelease(_descriptionRef);
        _descriptionRef = nil;
        NSLog(@"[XXh26xDecorder] 错误 (LINE:%d).", __LINE__);
        return NO;
    }
    return YES;
}
- (CMVideoFormatDescriptionRef) createDescriptionWithVps:(NSData*)vps
                                                     sps:(NSData*)sps
                                                     pps:(NSData*)pps
                                                    h264:(BOOL)isH264{
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
        NSLog(@"[XXh26xDecorder] 错误,系统版本过低 (LINE:%d).", __LINE__);
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

#pragma mark - 私有函数: <在nalu中获取vps、sps、pps> <在nalu中获取I帧或者P帧> <yuv420->rawData> <rgb24->rawData>
- (void) mediaInfoSpliterHandle:(XXnaluSpliterHandle)spliterHandle vps:(NSData* __strong*)vps sps:(NSData* __strong*)sps pps:(NSData* __strong*)pps{
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
        NSLog(@"[XXh26xDecorder] 错误 (LINE:%d).", __LINE__);
        *length = 0;
        return;
    }
    
    int len = htonl(dataRefLength);
    memcpy(buffer, &len, 4);
    memcpy(buffer+4, dataRef, dataRefLength);
    *length = dataRefLength + 4;
}
- (int)yuv420PixelBuffer:(CVPixelBufferRef)pixelBuffer toBuffer:(uint8_t*)buffer{
    CVPixelBufferLockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
    
    void *yAddress = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
    void *uAddress = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
    void *vAddress = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 2);
    
    size_t yBytePerRow = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
    size_t uBytePerRow = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);
    size_t vBytePerRow = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 2);
    
    size_t w = CVPixelBufferGetWidth(pixelBuffer);
    size_t h = CVPixelBufferGetHeight(pixelBuffer);
    uint8_t *y, *u, *v;
    
    y = buffer;
    u = y + w * h;
    v = u + w * h / 4;
    for (int i=0; i<h; i++)
        memcpy(y + w * i, yAddress + yBytePerRow * i, w);
    for (int i=0; i<h/2; i++)
        memcpy(u + w / 2 * i, uAddress + uBytePerRow * i, w / 2);
    for (int i=0; i<h/2; i++)
        memcpy(v + w / 2 * i, vAddress + vBytePerRow * i, w / 2);
    
    CVPixelBufferUnlockBaseAddress(pixelBuffer,0);
    return (int)(w * h *3 / 2);
}
- (int)rgb24PixelBuffer:(CVPixelBufferRef)pixelBuffer toBuffer:(int8_t*)buffer{
    CVPixelBufferLockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
    void *dataAddress    = CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
    size_t length        = (int)CVPixelBufferGetDataSize(pixelBuffer);
    memcpy(pixelBuffer, dataAddress, length);
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    return (int)length;
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
