#import "XXh26xEncoder.h"
#import <AVFoundation/AVFoundation.h>
#import <VideoToolbox/VideoToolbox.h>
#import <CoreFoundation/CoreFoundation.h>
#include <pthread.h>

const int TIMESCALE = 90000;

@implementation XXh26xEncoderParam
+ (XXh26xEncoderParam*) paramWithWidth:(int)width Height:(int)height FrameRate:(int)frameRate BitRate:(int)bitRate IsH265:(BOOL)isH265{
    return [[XXh26xEncoderParam alloc] initWithWidth:width Height:height FrameRate:frameRate BitRate:bitRate IsH265:isH265];
}

- (instancetype)initWithWidth:(int)width Height:(int)height FrameRate:(int)frameRate BitRate:(int)bitRate IsH265:(BOOL)isH265{
    self = [super init];
    if (self) {
        _width      = width;
        _height     = height;
        _frameRate  = frameRate;
        _bitRate    = bitRate;
        _isH265     = isH265;
    }
    return self;
}
@end

static void compressionOutputCallback(void *outputCallbackRefCon, void *sourceFrameRefCon, OSStatus status, VTEncodeInfoFlags infoFlags, CMSampleBufferRef sampleBuffer);

//
//
//
@interface XXh26xEncoder()
@property (nonatomic,weak) id<XXvideoEncoderEvents> events;
@property (nonatomic,strong) XXh26xEncoderParam *param;

@property (nonatomic,assign) BOOL isRunning;
@property (nonatomic,assign) VTCompressionSessionRef sessionRef;
@end

@implementation XXh26xEncoder
#pragma mark - 构建析构
- (instancetype) initWithName:(NSString*)name Events:(id<XXvideoEncoderEvents> _Nullable)events Param:(XXh26xEncoderParam*)param{
    self = [super initWithName:name];
    if (self) {
        _events     = events;
        _param      = param;
        _sessionRef = nil;
    }
    return self;
}

#pragma mark - 重载函数: <start> <encode> <stop>
- (BOOL) start{
    if (_isRunning || nil == _param) {
        return NO;
    }
    
    OSStatus status;
    if (nil == _sessionRef) {
        // 创建session
        VTCompressionSessionRef session = nil;
        status = VTCompressionSessionCreate(NULL,
                                            _param.width,
                                            _param.height,
                                            _param.isH265 ? kCMVideoCodecType_HEVC : kCMVideoCodecType_H264,
                                            NULL,
                                            NULL,
                                            NULL,
                                            compressionOutputCallback,
                                            (__bridge void *)(self), &session);
        if (status != noErr) {
            return NO;
        }
        
        // 设置码率
        status &= VTSessionSetProperty(session, kVTCompressionPropertyKey_AverageBitRate, (__bridge CFTypeRef)@(_param.bitRate));
        // 设置profile
        if (_param.isH265) {
            status &= VTSessionSetProperty(session, kVTCompressionPropertyKey_ProfileLevel, kVTProfileLevel_HEVC_Main_AutoLevel);
        }
        else{
            status &= VTSessionSetProperty(session, kVTCompressionPropertyKey_ProfileLevel, kVTProfileLevel_H264_Baseline_AutoLevel);
        }
        // 设置实时编码
        status &= VTSessionSetProperty(session, kVTCompressionPropertyKey_RealTime, kCFBooleanTrue);
        // 设置是否允许生成B帧
        status &= VTSessionSetProperty(session, kVTCompressionPropertyKey_AllowFrameReordering, kCFBooleanFalse);
        // 设置gop
        status &= VTSessionSetProperty(session, kVTCompressionPropertyKey_MaxKeyFrameInterval, (__bridge CFTypeRef)@(_param.frameRate));
        // 设置session的帧率
        status &= VTSessionSetProperty(session, kVTCompressionPropertyKey_ExpectedFrameRate, (__bridge CFTypeRef)@(_param.frameRate));
        if (status != noErr) {
            VTCompressionSessionInvalidate(session);
            return NO;
        }
        
        _sessionRef = session;
    }
    
    // 编码器准备编码
    status = VTCompressionSessionPrepareToEncodeFrames(_sessionRef);
    if (status != noErr) {
        return NO;
    }
    
    _isRunning = YES;
    return YES;
}
- (BOOL) encode:(CVImageBufferRef)buffer Timestamp:(NSTimeInterval)timstamp{
    if (!_isRunning || nil == _sessionRef) {
        return NO;
    }
    // pts
    CMTime pts = CMTimeMake(timstamp * TIMESCALE, TIMESCALE);
    // 设置是否为I帧
    //NSDictionary *frameProperties = @{(__bridge NSString *)kVTEncodeFrameOptionKey_ForceKeyFrame: @(forceKeyFrame)};
    // 自定义指针
    void *voidSelf = (__bridge void*)self;
    printf("[%.3f] [Encode] PTS:%.3f(开始编码) (线程ID:%p)\n", (double)clock()/(CLOCKS_PER_SEC/10), timstamp, pthread_self());
    //NSLog(@"[Encode] PTS:%.3f(开始编码) (线程ID:%p)", timstamp, pthread_self());
    VTCompressionSessionEncodeFrame(_sessionRef, buffer, pts, kCMTimeInvalid, nil, voidSelf, nil);
    return YES;
}
- (void) stop{
    if (!_isRunning || nil == _sessionRef) {
        return;
    }
    VTCompressionSessionCompleteFrames(_sessionRef, kCMTimeInvalid);
    _isRunning = NO;
}

#pragma mark - 私有函数: <onError> <onData> <toStream>
- (void) onError:(NSString*)message{
    [_events videoEncoderEvent_onError:self.name Message:message];
}
- (void) onData:(NSData*)data Dts:(NSTimeInterval)dts Pts:(NSTimeInterval)pts IsKeyFrame:(BOOL)isKeyFrame{
    [_events videoEncoderEvent_onData:self.name Data:data Dts:dts Pts:pts IsKeyFrame:isKeyFrame];
}
- (NSArray<NSData*>*) toAnnexBWithXVCC:(NSData*)xvcc{
    if (nil == xvcc || xvcc.length < 5) {
        return nil;
    }

    NSMutableArray *array   = [[NSMutableArray alloc] init];
    const char *bytes       = (const char*)xvcc.bytes;
    NSUInteger offsetLength = 0;
    NSUInteger totalLength  = xvcc.length;
    
    while (offsetLength < totalLength - 4) {
        // 读取slice的长度
        uint32_t naluLength;
        memcpy(&naluLength, bytes+offsetLength, 4);
        // 大端模式->本机字节序
        naluLength = CFSwapInt32BigToHost(naluLength);
        // 读取数据
        NSData *nalu = [NSData dataWithBytes:bytes + offsetLength + 4 length:naluLength];
        // 加入列表
        [array addObject:nalu];
        // 设置offsetLength
        offsetLength += naluLength + 4;
    }
    
    return array;
}
- (NSData*) toStream:(NSArray<NSData*>*)content VPS:(NSData*)vps SPS:(NSData*)sps PPS:(NSData*)pps{
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

#pragma mark - 私有函数: <content> <VPS> <SPS> <PPS> <isKeyFrame>
- (NSData*) getContentFromSampleBufferRef:(CMSampleBufferRef)sampleBufferRef{
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
- (NSData*) getVPSFromSampleBufferRef:(CMSampleBufferRef)sampleBufferRef{
    if (!_param.isH265) {
        return nil;
    }
    CMVideoFormatDescriptionRef description = CMSampleBufferGetFormatDescription(sampleBufferRef);
    
    const uint8_t *bytes    = nil;
    size_t size             = 0;
    CMVideoFormatDescriptionGetHEVCParameterSetAtIndex(description, 0, &bytes, &size, nil, nil);
    if (size <= 0) {
        return nil;
    }
    return [NSData dataWithBytes:bytes length:size];
}
- (NSData*) getSPSFromSampleBufferRef:(CMSampleBufferRef)sampleBufferRef{
    CMVideoFormatDescriptionRef description = CMSampleBufferGetFormatDescription(sampleBufferRef);
    
    const uint8_t *bytes    = nil;
    size_t size             = 0;
    if(!_param.isH265){
        CMVideoFormatDescriptionGetH264ParameterSetAtIndex(description, 0, &bytes, &size, nil, nil);
    }
    else{
        CMVideoFormatDescriptionGetHEVCParameterSetAtIndex(description, 1, &bytes, &size, nil, nil);
    }
    if (size <= 0) {
        return nil;
    }
    return [NSData dataWithBytes:bytes length:size];
}
- (NSData*) getPPSFromSampleBufferRef:(CMSampleBufferRef)sampleBufferRef{
    CMVideoFormatDescriptionRef description = CMSampleBufferGetFormatDescription(sampleBufferRef);
    
    const uint8_t *bytes    = nil;
    size_t size             = 0;
    if(!_param.isH265){
        CMVideoFormatDescriptionGetH264ParameterSetAtIndex(description, 1, &bytes, &size, nil, nil);
    }
    else{
        CMVideoFormatDescriptionGetHEVCParameterSetAtIndex(description, 2, &bytes, &size, nil, nil);
    }
    if (size <= 0) {
        return nil;
    }
    return [NSData dataWithBytes:bytes length:size];
}
- (BOOL) isKeyFrame:(CMSampleBufferRef)sampleBufferRef{
    BOOL isKeyFrame = NO;
    CFArrayRef attachments = CMSampleBufferGetSampleAttachmentsArray(sampleBufferRef, 0);
    if (attachments != nil && CFArrayGetCount(attachments)) {
        CFDictionaryRef attachment = (CFDictionaryRef)CFArrayGetValueAtIndex(attachments, 0);
        isKeyFrame = !CFDictionaryContainsKey(attachment, kCMSampleAttachmentKey_NotSync);
    }
    return isKeyFrame;
}
@end

#pragma mark - 私有函数: <session回调函数>
static void compressionOutputCallback(void *outputCallbackRefCon, void *sourceFrameRefCon, OSStatus status, VTEncodeInfoFlags infoFlags, CMSampleBufferRef sampleBuffer){
    XXh26xEncoder *encoder = (__bridge XXh26xEncoder*)outputCallbackRefCon;
    if (status != noErr) {
        [encoder onError:[NSString stringWithFormat:@"[XXh26xEncoder] encode error(%d)", status]];
        return;
    }
    else{
        // 取出时间信息
        CMSampleTimingInfo timingInfo[10];
        CMItemCount timingCount = 0;
        OSStatus status = CMSampleBufferGetSampleTimingInfoArray(sampleBuffer, 10, timingInfo, &timingCount);
        if (status != noErr) {
            [encoder onError:@"[XXh26xEncoder] can not get sample timing info array"];
            return;
        }
        
        // 根据timescale转换到pts和dts
        CMTime dtsTime      = timingInfo[0].decodeTimeStamp;
        NSTimeInterval dts  = (NSTimeInterval)dtsTime.value / (NSTimeInterval)dtsTime.timescale;
        
        CMTime ptsTime      = timingInfo[0].presentationTimeStamp;
        NSTimeInterval pts  = (NSTimeInterval)ptsTime.value / (NSTimeInterval)ptsTime.timescale;
        
        printf("[%.3f] [Encode] PTS:%.3f(整理数据) (线程ID:%p)\n", (double)clock()/(CLOCKS_PER_SEC/10), pts, pthread_self());
        //NSLog(@"[Encode] PTS:%.3f(整理数据) (线程ID:%p)", pts, pthread_self());
        NSData *content = [encoder getContentFromSampleBufferRef:sampleBuffer];
        if (nil == content) {
            [encoder onError:@"[XXh26xEncoder] can not get content from sample buffer"];
            return;
        }
        NSArray<NSData*> *nalus = [encoder toAnnexBWithXVCC:content];
        if (nil == nalus || nalus.count <= 0) {
            [encoder onError:@"[XXh26xEncoder] can not to annex-b"];
            return ;
        }
        
        BOOL isKeyFrame = [encoder isKeyFrame:sampleBuffer];
        NSData *data    = nil;
        if (isKeyFrame) {
            NSData *vps = [encoder getVPSFromSampleBufferRef:sampleBuffer];
            NSData *sps = [encoder getSPSFromSampleBufferRef:sampleBuffer];
            NSData *pps = [encoder getPPSFromSampleBufferRef:sampleBuffer];
            if (nil == sps || nil == pps) {
                [encoder onError:@"[XXh26xEncoder] can not get sps/pps from sample buffer"];
                return;
            }
            
            data = [encoder toStream:nalus VPS:vps SPS:sps PPS:pps];
        }
        else{
            data = [encoder toStream:nalus VPS:nil SPS:nil PPS:nil];
        }
        
        if (nil == data) {
            [encoder onError:@"[XXh26xEncoder] can not to stream"];
            return;
        }
        [encoder onData:data Dts:dts Pts:pts IsKeyFrame:isKeyFrame];
        printf("[%.3f] [Encode] PTS:%.3f(输出数据) (线程ID:%p)\n", (double)clock()/(CLOCKS_PER_SEC/10), pts, pthread_self());
        //NSLog(@"[Encode] PTS:%.3f(输出数据) (线程ID:%p)", pts, pthread_self());
    }
}
