#import "XXh26xEncoder.h"
#import <AVFoundation/AVFoundation.h>
#import <VideoToolbox/VideoToolbox.h>
#import <CoreFoundation/CoreFoundation.h>
#import <CoreVideo/CoreVideo.h>

#define kTimeScale 90000

static void compressionOutputCallback(void *outputCallbackRefCon, void *sourceFrameRefCon, OSStatus status, VTEncodeInfoFlags infoFlags, CMSampleBufferRef sampleBuffer);

@interface XXh26xEncoder()
@property (nonatomic,assign) VTCompressionSessionRef sessionRef;
@end

@implementation XXh26xEncoder
#pragma mark - <Init/Dealloc>
+ (nullable XXh26xEncoder*)encoderWithFormat:(XXvideoFormat*)format{
    return [[XXh26xEncoder alloc] initWithFormat:format];
}
- (instancetype)initWithFormat:(XXvideoFormat*)format{
    self = [super init];
    if (self) {
        /// format的有效性检查，空指针 || （不是h265 && 不是h264）
        if(nil == format ||
           (format.codecType!=kCMVideoCodecType_HEVC && format.codecType!=kCMVideoCodecType_H264)){
            return nil;
        }
        _sessionRef = [self initSession];
        if(nil == _sessionRef){
            return nil;
        }
    }
    return self;
}
- (void)dealloc{
    if(_sessionRef){
        VTCompressionSessionInvalidate(_sessionRef);
        CFRelease(_sessionRef);
    }
}
- (nullable VTCompressionSessionRef)initSession{
    /// 创建session
    VTCompressionSessionRef session = nil;
    OSStatus status = VTCompressionSessionCreate(NULL, _format.width, _format.height, _format.codecType,
                                                 NULL,NULL,NULL, compressionOutputCallback,
                                                 (__bridge void *)(self), &session);
    if(noErr != status){
        return nil;
    }
    
    /// 设置码率，平均码率
    status &= VTSessionSetProperty(session, kVTCompressionPropertyKey_AverageBitRate, (__bridge CFTypeRef)@(_format.bitRate));
    
    /// 设置Profile
    if(kCMVideoCodecType_HEVC == _format.codecType){
        /// 265
        status &= VTSessionSetProperty(session, kVTCompressionPropertyKey_ProfileLevel, kVTProfileLevel_HEVC_Main_AutoLevel);
    }
    else {
        /// 264
        status &= VTSessionSetProperty(session, kVTCompressionPropertyKey_ProfileLevel, kVTProfileLevel_H264_Baseline_AutoLevel);
    }
    
    /// 设置实时编码
    status &= VTSessionSetProperty(session, kVTCompressionPropertyKey_RealTime, kCFBooleanTrue);
    
    /// 设置是否允许生成B帧
    status &= VTSessionSetProperty(session, kVTCompressionPropertyKey_AllowFrameReordering, kCFBooleanFalse);
    
    /// 设置gop
    status &= VTSessionSetProperty(session, kVTCompressionPropertyKey_MaxKeyFrameInterval, (__bridge CFTypeRef)@(_format.frameRate));
    
    /// 设置session的帧率
    status &= VTSessionSetProperty(session, kVTCompressionPropertyKey_ExpectedFrameRate, (__bridge CFTypeRef)@(_format.frameRate));
    if (status != noErr) {
        VTCompressionSessionInvalidate(session);
        CFRelease(session);
        return nil;
    }
    
    return session;
}

#pragma mark - <编码>
- (BOOL)encode:(CVImageBufferRef)buffer timestamp:(NSTimeInterval)timestamp{
    // pts
    CMTime pts = CMTimeMake((timestamp*kTimeScale), kTimeScale);
    // 设置是否为I帧
    //NSDictionary *frameProperties = @{(__bridge NSString *)kVTEncodeFrameOptionKey_ForceKeyFrame: @(forceKeyFrame)};
    // 自定义指针
    void *voidSelf = (__bridge void*)self;
    VTCompressionSessionEncodeFrame(_sessionRef, buffer, pts, kCMTimeInvalid, nil, voidSelf, nil);
}

//- (BOOL) start{
//    if (_isRunning) {
//        return NO;
//    }
//
//    OSStatus status;
//    if (nil == _sessionRef) {
//        // 创建session
//        VTCompressionSessionRef session = nil;
//        status = VTCompressionSessionCreate(NULL,
//                                            _width,
//                                            _height,
//                                            _isH265 ? kCMVideoCodecType_HEVC : kCMVideoCodecType_H264,
//                                            NULL,
//                                            NULL,
//                                            NULL,
//                                            compressionOutputCallback,
//                                            (__bridge void *)(self), &session);
//        if (status != noErr) {
//            return NO;
//        }
//
//        // 设置码率
//        status &= VTSessionSetProperty(session, kVTCompressionPropertyKey_AverageBitRate, (__bridge CFTypeRef)@(_bitRate));
//        // 设置profile
//        if (_isH265) {
//            status &= VTSessionSetProperty(session, kVTCompressionPropertyKey_ProfileLevel, kVTProfileLevel_HEVC_Main_AutoLevel);
//        }
//        else{
//            status &= VTSessionSetProperty(session, kVTCompressionPropertyKey_ProfileLevel, kVTProfileLevel_H264_Baseline_AutoLevel);
//        }
//        // 设置实时编码
//        status &= VTSessionSetProperty(session, kVTCompressionPropertyKey_RealTime, kCFBooleanTrue);
//        // 设置是否允许生成B帧
//        status &= VTSessionSetProperty(session, kVTCompressionPropertyKey_AllowFrameReordering, kCFBooleanFalse);
//        // 设置gop
//        status &= VTSessionSetProperty(session, kVTCompressionPropertyKey_MaxKeyFrameInterval, (__bridge CFTypeRef)@(_frameRate));
//        // 设置session的帧率
//        status &= VTSessionSetProperty(session, kVTCompressionPropertyKey_ExpectedFrameRate, (__bridge CFTypeRef)@(_frameRate));
//        if (status != noErr) {
//            VTCompressionSessionInvalidate(session);
//            return NO;
//        }
//
//        _sessionRef = session;
//    }
//
//    // 编码器准备编码
//    status = VTCompressionSessionPrepareToEncodeFrames(_sessionRef);
//    if (status != noErr) {
//        return NO;
//    }
//
//    _isRunning = YES;
//    return YES;
//}
//- (BOOL) encode:(CVImageBufferRef)buffer Timestamp:(NSTimeInterval)timstamp{
//    if (!_isRunning || nil == _sessionRef) {
//        return NO;
//    }
//    // pts
//    CMTime pts = CMTimeMake(timstamp * kTimeScale, kTimeScale);
//    // 设置是否为I帧
//    //NSDictionary *frameProperties = @{(__bridge NSString *)kVTEncodeFrameOptionKey_ForceKeyFrame: @(forceKeyFrame)};
//    // 自定义指针
//    void *voidSelf = (__bridge void*)self;
//    printf("[%.3f] [Encode] PTS:%.3f(开始编码) (线程ID:%p)\n", (double)clock()/(CLOCKS_PER_SEC/10), timstamp, pthread_self());
//    //NSLog(@"[Encode] PTS:%.3f(开始编码) (线程ID:%p)", timstamp, pthread_self());
//    VTCompressionSessionEncodeFrame(_sessionRef, buffer, pts, kCMTimeInvalid, nil, voidSelf, nil);
//    return YES;
//}
//- (void) stop{
//    if (!_isRunning || nil == _sessionRef) {
//        return;
//    }
//    VTCompressionSessionCompleteFrames(_sessionRef, kCMTimeInvalid);
//    _isRunning = NO;
//}
//- (void) configParam:(id)param{
//    NSDictionary *dictionary = param;
//    _width      = [dictionary[PARAM_KEY_WIDTH] intValue];
//    _height     = [dictionary[PARAM_KEY_HEIGHT] intValue];
//    _frameRate  = [dictionary[PARAM_KEY_FRAMERATE] intValue];
//    _bitRate    = [dictionary[PARAM_KEY_BITRATE] intValue];
//    _isH265     = [dictionary[PARAM_KEY_IS_H265] boolValue];
//}

#pragma mark - <XVCC格式转到AnnexB格式、将frame数据/VPS/SPS/PPS整合成一个数据包(以0x00000001分割)>
- (NSArray<NSData*>*) toAnnexBWithXVCC:(NSData*)xvcc{
    /**
     将XVCC格式转换到AnnexB格式，XVCC：4字节的数据长度（不包含自己的4字节）+数据
     */
    if (nil == xvcc || xvcc.length < 5) {
        return nil;
    }

    NSMutableArray *array   = [[NSMutableArray alloc] init];
    const char *bytes       = (const char*)xvcc.bytes;
    NSUInteger offsetLength = 0;
    NSUInteger totalLength  = xvcc.length;
    
    while (offsetLength < totalLength - 4) {
        /// 读取slice的长度
        uint32_t naluLength;
        memcpy(&naluLength, bytes+offsetLength, 4);
        /// 大端模式->本机字节序
        naluLength = CFSwapInt32BigToHost(naluLength);
        /// 读取数据
        NSData *nalu = [NSData dataWithBytes:bytes + offsetLength + 4 length:naluLength];
        /// 加入列表
        [array addObject:nalu];
        /// 设置offsetLength
        offsetLength += naluLength + 4;
    }
    
    return array;
}
- (NSData*) toStream:(NSArray<NSData*>*)content vps:(NSData*)vps sps:(NSData*)sps pps:(NSData*)pps{
    static Byte startCode[] = {0x00, 0x00, 0x00, 0x01};
    
    NSMutableData *stream = [[NSMutableData alloc] init];
    if (nil != vps) {
        [stream appendBytes:startCode length:4];
        [stream appendData:vps];
    }
    
    if (nil != sps) {
        [stream appendBytes:startCode length:4];
        [stream appendData:sps];
    }
    
    if (nil != pps) {
        [stream appendBytes:startCode length:4];
        [stream appendData:pps];
    }
    
    if (nil != content) {
        for (NSData *data in content) {
            [stream appendBytes:startCode length:4];
            [stream appendData:data];
        }
    }
    return stream;
}

#pragma mark - <SampleBuffer一些参数、数据获取，如帧数据、VPS、SPS、PPS等>
- (NSData*)getContentFromSampleBufferRef:(CMSampleBufferRef)sampleBufferRef{
    CMBlockBufferRef blockBufferRef = CMSampleBufferGetDataBuffer(sampleBufferRef);
    if (nil == blockBufferRef) {
        return nil;
    }
    
    size_t length = CMBlockBufferGetDataLength(blockBufferRef);
    if (length <= 0) {
        return nil;
    }
    
    Byte buffer[length];
    CMBlockBufferCopyDataBytes(blockBufferRef, 0, length, buffer);
    return [NSData dataWithBytes:buffer length:length];
}
- (void)getParamaterSetWithBufferRef:(CMSampleBufferRef)bufferRef vps:(NSData**)vps sps:(NSData**)sps pps:(NSData**)pps{
    
    const uint8_t *bytes    = nil;
    size_t size             = 0;
    if(kCMVideoCodecType_HEVC == _format.codecType){
        /// VPS
        CMVideoFormatDescriptionGetHEVCParameterSetAtIndex(description, 0, &bytes, &size, nil, nil);
        if(size>0){
            *vps = [NSData dataWithBytes:bytes length:size];
        }
        
        /// SPS
        size = 0;
        CMVideoFormatDescriptionGetHEVCParameterSetAtIndex(description, 1, &bytes, &size, nil, nil);
        if(size>0){
            *sps = [NSData dataWithBytes:bytes length:size];
        }
        
        /// PPS
        size = 0;
        CMVideoFormatDescriptionGetHEVCParameterSetAtIndex(description, 2, &bytes, &size, nil, nil);
        if(size>0){
            *pps = [NSData dataWithBytes:bytes length:size];
        }
    }
    else{
        /// VPS，h264没有VPS
        *vps = nil;
        
        /// SPS
        CMVideoFormatDescriptionGetH264ParameterSetAtIndex(description, 0, &bytes, &size, nil, nil);
        if(size>0){
            *sps = [NSData dataWithBytes:bytes length:size];
        }
        
        /// PPS
        size = 0;
        CMVideoFormatDescriptionGetH264ParameterSetAtIndex(description, 1, &bytes, &size, nil, nil);
        if(size>0){
            *pps = [NSData dataWithBytes:bytes length:size];
        }
    }
}
- (BOOL)isKeyFrame:(CMSampleBufferRef)sampleBufferRef{
    BOOL isKeyFrame = NO;
    CFArrayRef attachments = CMSampleBufferGetSampleAttachmentsArray(sampleBufferRef, 0);
    if (attachments != nil && CFArrayGetCount(attachments)) {
        CFDictionaryRef attachment = (CFDictionaryRef)CFArrayGetValueAtIndex(attachments, 0);
        isKeyFrame = !CFDictionaryContainsKey(attachment, kCMSampleAttachmentKey_NotSync);
    }
    return isKeyFrame;
}
- (BOOL)getTimestampWithBufferRef:(CMSampleBufferRef)sampleBufferRef dts:(NSTimeInterval*)dts pts:(NSTimeInterval*)pts{
    /// 取出时间信息
    CMSampleTimingInfo timingInfo[10];
    CMItemCount timingCount = 0;
    OSStatus status = CMSampleBufferGetSampleTimingInfoArray(sampleBufferRef, 10, timingInfo, &timingCount);
    if (status != noErr) {
        return NO;
    }
    
    /// 根据timescale转换到pts和dts
    CMTime ts   = timingInfo[0].decodeTimeStamp;
    *dts        = (NSTimeInterval)ts.value / (NSTimeInterval)ts.timescale;
    
    ts          = timingInfo[0].presentationTimeStamp;
    *pts        = (NSTimeInterval)ts.value / (NSTimeInterval)ts.timescale;
    return YES;
}
@end

#pragma mark - <session回调函数>
static void compressionOutputCallback(void *outputCallbackRefCon, void *sourceFrameRefCon, OSStatus status, VTEncodeInfoFlags infoFlags, CMSampleBufferRef sampleBuffer){
    XXh26xEncoder *encoder = (__bridge XXh26xEncoder*)outputCallbackRefCon;
    if (status != noErr) {
        [encoder onError:[NSString stringWithFormat:@"[XXh26xEncoder] encode error(%d)", status]];
        return;
    }
    else{
        /// 时间戳
        NSTimeInterval dts,pts;
        if(![encoder getTimestampWithBufferRef:sampleBuffer dts:&dts pts:&pts]){
            NSLog(@"[XXh26xEncoder] can not get timestamp from sample buffer");
            return;
        }
        
        /// 实体数据
        NSData *content = [encoder getContentFromSampleBufferRef:sampleBuffer];
        if (nil == content) {
            if(encoder.onCompressionError){
            }
            NSLog(@"[XXh26xEncoder] can not get content from sample buffer");
            return;
        }
        NSArray<NSData*> *nalus = [encoder toAnnexBWithXVCC:content];
        if (nil == nalus || nalus.count <= 0) {
            if(encoder.onCompressionError){
            }
            NSLog(@"[XXh26xEncoder] can not to annex-b");
            return ;
        }
        
        /// 关键帧参数
        BOOL isKeyFrame = [encoder isKeyFrame:sampleBuffer];
        NSData *data    = nil;
        if (isKeyFrame) {
            NSData *vps, *sps, *pps;
            [encoder getParamaterSetWithBufferRef:sampleBuffer vps:&vps sps:&sps pps:&pps];
            if (nil == sps || nil == pps) {
                if(encoder.onCompressionError){
                }
                NSLog(@"[XXh26xEncoder] can not get sps/pps from sample buffer");
                return;
            }
            
            data = [encoder toStream:nalus vps:vps sps:sps pps:pps];
        }
        else{
            data = [encoder toStream:nalus vps:nil sps:nil pps:nil];
        }
        
        /// 回调
        if (nil == data) {
            if(encoder.onCompressionError){
            }
            NSLog(@"[XXh26xEncoder] can not to stream");
            return;
        }
        
        if (encoder.onCompressionOutput) {
            encoder.onCompressionOutput(encoder, data, dts, pts, isKeyFrame);
        }
        NSLog(@"[XXh26xEncoder] output. size:%d dts:%.3f pts:%.3f key:%d",data.length, dts, pts, isKeyFrame);
    }
}
