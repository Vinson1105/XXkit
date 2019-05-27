#import "XXvideoRecorder.h"

@interface XXvideoRecorder()
@property (nonatomic,assign) BOOL isReady;
@property (nonatomic,assign) BOOL isRunning;
@property (nonatomic,weak) id<XXvideoRecorderEvents> events;
@property (nonatomic,weak) UIView *view;
@property (nonatomic,assign) AVCaptureVideoOrientation currentOrientation;
@property (nonatomic,assign) AVCaptureDevicePosition currentPosition;

@property (nonatomic,strong) AVCaptureDeviceInput *captureDeviceInput;              // 采集输入设备
@property (nonatomic,strong) AVCaptureVideoDataOutput *captureVideoDataOutput;      // 采集输出数据
@property (nonatomic,strong) AVCaptureSession *captureSession;                      // 采集会话
@property (nonatomic,strong) AVCaptureConnection *captureConnection;                // 采集连接
@property (nonatomic,strong) AVCaptureVideoPreviewLayer *captureVideoPreviewLayer;  // 采集预览layer

@property (nonatomic,strong) dispatch_queue_t videoCaptureOutputQueue;
@property (nonatomic,assign) CMSampleBufferRef sampleBufferRef;
@end

@implementation XXvideoRecorder
#pragma mark - 构建析构
- (instancetype) initWithEvents:(id<XXvideoRecorderEvents>)events View:(UIView* _Nullable)view IsDefaultPositionFront:(BOOL)isFront{// VideoType:(XXavFrameType)videoType;
    self = [super init];
    if (self) {
        _frameRate  = 25;
        _type       = kCVPixelFormatType_420YpCbCr8BiPlanarFullRange;
        _resolution = AVCaptureSessionPreset1280x720;
        _currentOrientation = AVCaptureVideoOrientationPortrait;
        _currentPosition    = isFront ? AVCaptureDevicePositionFront : AVCaptureDevicePositionBack;
        
        _events     = events;
        _view       = view;
        _isReady    = [self connect];
    }
    return self;
}
- (void)dealloc{
    if(nil != _sampleBufferRef){
        CFRelease(_sampleBufferRef);
        _sampleBufferRef = nil;
    }
}

#pragma mark - 公共接口: <开始> <停止>
- (BOOL) start{
    if (!_isReady || _isRunning) {
        return NO;
    }
    
    if (nil != _videoEncoder && ![_videoEncoder start]) {
        return NO;
    }
    
    // 摄像头权限判断
    AVAuthorizationStatus videoAuthStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
    if (videoAuthStatus != AVAuthorizationStatusAuthorized){
        return NO;
    }
    
    [_captureSession startRunning];
    _isRunning = YES;
    return YES;
}
- (void) stop{
    [_captureSession stopRunning];
    if (nil != _videoEncoder) {
        [_videoEncoder stop];
    }
    _isRunning = NO;
}

#pragma mark - 公共接口: <前置摄像头> <后置摄像头>
- (BOOL) toFront{
    if (!_isRunning) {
        return NO;
    }
    return [self changePosition:AVCaptureDevicePositionFront];
}
- (BOOL) toBack{
    if (!_isRunning) {
        return NO;
    }
    return [self changePosition:AVCaptureDevicePositionBack];
}

#pragma mark - 公共接口: <竖屏> <右横屏> <左横屏>
- (BOOL) toPortrait{
    if (!_isRunning) {
        return NO;
    }
    return [self changeOrientation:AVCaptureVideoOrientationPortrait];
}
- (BOOL) toLandscapeRight{
    if (!_isRunning) {
        return NO;
    }
    return [self changeOrientation:AVCaptureVideoOrientationLandscapeRight];
}
- (BOOL) toLandscapeLeft{
    if (!_isRunning) {
        return NO;
    }
    return [self changeOrientation:AVCaptureVideoOrientationLandscapeLeft];
}

#pragma mark - 协议函数: <AVCaptureVideoDataOutputSampleBufferDelegate>
- (void)captureOutput:(AVCaptureOutput *)output didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection{
    if(nil != _sampleBufferRef){
        CFRelease(_sampleBufferRef);
    }
    _sampleBufferRef = (CMSampleBufferRef)CFRetain(sampleBuffer);
    
    CMSampleTimingInfo timingInfo[10];
    CMItemCount timingCount = 0;
    OSStatus status = CMSampleBufferGetSampleTimingInfoArray(_sampleBufferRef, 10, timingInfo, &timingCount);
    
    if (status < 0) {
        [_events videoRecorderEvent_onError:@"[XXvideoRecorder] can not get sample timing info array"];
        return;
    }
    
    CMTime ptsTime      = timingInfo[0].presentationTimeStamp;
    NSTimeInterval pts  = (NSTimeInterval)ptsTime.value / (NSTimeInterval)ptsTime.timescale;
    
    if (nil != _videoEncoder) {
        CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(_sampleBufferRef);
        if (nil == imageBuffer) {
            [_events videoRecorderEvent_onError:@"[XXvideoRecorder] can not get image buffer"];
        }
        else{
            [_videoEncoder encode:imageBuffer Timestamp:pts];
        }
    }
    else if(nil != _events){
        CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(_sampleBufferRef);
        NSLog(@"%zu*%zu", CVPixelBufferGetWidth(imageBuffer), CVPixelBufferGetHeight(imageBuffer));
        
        if (nil == imageBuffer) {
            [_events videoRecorderEvent_onError:@"[XXvideoRecorder] can not get data buffer"];
        }
        else{
            [_events videoRecorerEvent_onVideo:imageBuffer Timestamp:pts];
        }
        
//        CMBlockBufferRef blockBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
//        if (nil == blockBuffer) {
//            [_events videoRecorderEvent_onError:@"[XXvideoRecorder] can not get data buffer"];
//            return;
//        }
//
//        size_t length = CMBlockBufferGetDataLength(blockBuffer);
//        if (length <= 0) {
//            [_events videoRecorderEvent_onError:@"[XXvideoRecorder] data buffer is empty"];
//            return;
//        }
//
//        Byte buffer[length];
//        CMBlockBufferCopyDataBytes(blockBuffer, 0, length, buffer);
//        NSData *data = [NSData dataWithBytes:buffer length:length];
//        [_events videoRecorerEvent_onVideo:data Timestamp:pts];
    }
    else{
        
    }
}

#pragma mark - 私有函数: <获取输入设备> <设置视频数据输出> <设置会话> <CaptureConnect>
- (AVCaptureDeviceInput*) createInput:(AVCaptureDevicePosition)position{
    // 获取所有摄像头
    AVCaptureDeviceDiscoverySession *discoverySession = [AVCaptureDeviceDiscoverySession discoverySessionWithDeviceTypes:@[AVCaptureDeviceTypeBuiltInWideAngleCamera] mediaType:AVMediaTypeVideo position:position];
    if (0 == discoverySession.devices.count) {
        return nil;
    }
    
    // 转化为输入设备
    AVCaptureDevice *camera     = discoverySession.devices.firstObject;
    NSError *errorMessage       = nil;
    AVCaptureDeviceInput *input = [AVCaptureDeviceInput deviceInputWithDevice:camera error:&errorMessage];
    if (errorMessage){
        NSLog(@"AVCaptureDevice转AVCaptureDeviceInput失败");
        return nil;
    }
    
    // 设置帧率
    AVFrameRateRange *frameRateRange = [input.device.activeFormat.videoSupportedFrameRateRanges objectAtIndex:0];
    if (_frameRate > frameRateRange.maxFrameRate || _frameRate < frameRateRange.minFrameRate){
        NSLog(@"[XXvideoRecorder] 目标帧率(%d)超出设置帧率范围(%.3f-%.3f)", _frameRate, frameRateRange.minFrameRate, frameRateRange.maxFrameRate);
        return nil;
    }
    [input.device lockForConfiguration:nil];
    input.device.activeVideoMinFrameDuration = CMTimeMake(1, _frameRate);
    input.device.activeVideoMaxFrameDuration = CMTimeMake(1, _frameRate);
    [input.device unlockForConfiguration];
    
    return input;
}
- (AVCaptureVideoDataOutput*) createVideoOutput{
    AVCaptureVideoDataOutput *output = [[AVCaptureVideoDataOutput alloc] init];
    
    // 设置视频数据格式
    NSDictionary *videoSetting = [NSDictionary dictionaryWithObjectsAndKeys:@(_type), kCVPixelBufferPixelFormatTypeKey, nil];
    [output setVideoSettings:videoSetting];
    
    // 设置输出代理、串行队列和数据回调
    dispatch_queue_t queue = dispatch_queue_create("VideoOutputQueue", DISPATCH_QUEUE_SERIAL);
    [output setSampleBufferDelegate:self queue:queue];
    
    // 丢弃延迟的帧
    output.alwaysDiscardsLateVideoFrames = YES;
    
    return output;
}
- (AVCaptureSession*) createSession:(AVCaptureDeviceInput*)videoInput VideoOuput:(AVCaptureVideoDataOutput*)videoOutput{
    if (nil == videoInput || nil == videoOutput) {
        return nil;
    }
    
    // 采集会话
    AVCaptureSession *session = [[AVCaptureSession alloc] init];
    
    // 不使用应用的实例，避免被异常挂断
    session.usesApplicationAudioSession = NO;
    
    // 添加输入设备到会话
    if (![session canAddInput:videoInput]){
        return nil;
    }
    [session addInput:videoInput];
    
    // 添加输出设备到会话
    if (![session canAddOutput:videoOutput]){
        return nil;
    }
    [session addOutput:videoOutput];
    
    // 设置分辨率
    if (![session canSetSessionPreset:_resolution]){
        return nil;
    }
    session.sessionPreset = _resolution;
    
    return session;
}
- (BOOL) connect{
    // 输入设备
    _captureDeviceInput = [self createInput:_currentPosition];
    if (nil == _captureDeviceInput) {
        return NO;
    }
    
    // 视频输出
    _captureVideoDataOutput = [self createVideoOutput];
    if (nil == _captureVideoDataOutput) {
        return NO;
    }
    
    // 会话
    _captureSession = [self createSession:_captureDeviceInput VideoOuput:_captureVideoDataOutput];
    if (nil == _captureSession) {
        return NO;
    }
    
    // 获取连接并设置视频方向为竖屏方向
    _captureConnection = [_captureVideoDataOutput connectionWithMediaType:AVMediaTypeVideo];
    _captureConnection.videoOrientation = _currentOrientation;
    
    // 设置是否为镜像，前置摄像头采集到的数据本来就是翻转的，这里设置为镜像把画面转回来
    if (_currentPosition == AVCaptureDevicePositionFront && _captureConnection.supportsVideoMirroring){
        _captureConnection.videoMirrored = YES;
    }
    
    // 获取预览Layer并设置视频方向，注意_videoPreviewLayer.connection跟_captureConnection不是同一个对象，要分开设置
    _captureVideoPreviewLayer = [AVCaptureVideoPreviewLayer layerWithSession:_captureSession];
    _captureVideoPreviewLayer.connection.videoOrientation = _currentOrientation;
    _captureVideoPreviewLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
    
    if (nil != _view) {
        _captureVideoPreviewLayer.frame = _view.frame;
        [_view.layer insertSublayer:_captureVideoPreviewLayer atIndex:0];
    }
    return YES;
}

#pragma mark - 私有函数: <切换前后摄像头> <竖/左/右屏切换>
- (BOOL) changePosition:(AVCaptureDevicePosition)position{
    if(position == _captureDeviceInput.device.position){
        return NO;
    }
    
    AVCaptureDeviceInput *input = [self createInput:position];
    if (nil == input) {
        return NO;
    }
    
    BOOL succeed = NO;
    // 开始配置
    if (_captureSession.isRunning) {
        [_captureSession beginConfiguration];
    }
    
    // 移除旧的输入
    [_captureSession removeInput:_captureDeviceInput];
    
    // 检查是否可以加入新的输入
    if ([_captureSession canAddInput:input]) {
        [_captureSession addInput:input];
        _captureDeviceInput = input;
        succeed = YES;
    }
    else{
        [_captureSession addInput:_captureDeviceInput];
    }
    
    // 获取连接并设置视频方向为竖屏方向
    _captureConnection = [_captureVideoDataOutput connectionWithMediaType:AVMediaTypeVideo];
    _captureConnection.videoOrientation = _currentOrientation;
    
    // 设置是否为镜像，前置摄像头采集到的数据本来就是翻转的，这里设置为镜像把画面转回来
    if (position == AVCaptureDevicePositionFront && _captureConnection.supportsVideoMirroring){
        _captureConnection.videoMirrored = YES;
    }
    
    // 提交配置
    if (_captureSession.isRunning) {
        [_captureSession commitConfiguration];
    }
    return succeed;
}
- (BOOL) changeOrientation:(AVCaptureVideoOrientation)orientation{
    return NO;
}
@end
