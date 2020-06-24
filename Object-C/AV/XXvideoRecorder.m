#import "XXvideoRecorder.h"
#import "XXocUtils.h"

#define kFrameRateDefault   15
#define kTypeDefault        kCVPixelFormatType_32BGRA
#define kPresetDefault      AVCaptureSessionPresetHigh

static XXvideoRecorder *_instance = nil;

@interface XXvideoRecorder()
@property (nonatomic,strong) AVCaptureDeviceInput *captureDeviceInput;              /// 采集输入设备
@property (nonatomic,strong) AVCaptureVideoDataOutput *captureVideoDataOutput;      /// 采集输出数据
@property (nonatomic,strong) AVCaptureSession *captureSession;                      /// 采集会话
@property (nonatomic,strong) AVCaptureVideoPreviewLayer *captureVideoPreviewLayer;  /// 采集预览layer
@property (nonatomic,assign) CMSampleBufferRef sampleBufferRef;
@end

@implementation XXvideoRecorder
#pragma mark - <Init>
- (instancetype)init{
    self = [super init];
    if(self){
        _frameRate      = kFrameRateDefault;
        _type           = kTypeDefault;
        self.preset     = kPresetDefault;
        _position       = AVCaptureDevicePositionBack;
        _orientation    = AVCaptureVideoOrientationPortrait;
    }
    return self;
}
- (void)dealloc{
    if(nil != _sampleBufferRef){
        CFRelease(_sampleBufferRef);
        _sampleBufferRef = nil;
    }
}

#pragma mark - <Property>
- (void)setPreview:(UIView *)preview{
    if(preview == _preview) return;
    if(_preview){
        
    }
}

#pragma mark - <Shared Instance>
+ (XXvideoRecorder*)sharedInstance{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instance = [XXvideoRecorder new];
    });
    return _instance;
}

#pragma mark - <开启、停止>
- (BOOL)startConnect{
    /// 是否为连接中
    if(_isConnecting){
        return NO;
    }
    _isConnecting = YES;
    
    /// 摄像头权限判断
    if (![XXocUtils authorizedCamera]){
        _isConnecting = NO;
        return NO;
    }
    
    /// 连接
    if(![self connect]){
        _isConnecting = NO;
        return NO;
    }

    [_captureSession startRunning];
    return YES;
}
- (BOOL)startrecord{
    if(_isRecording || !_isConnecting){
        return NO;
    }
    _isRecording = YES;
    return YES;
}
- (void)stopRecord{
    _isRecording = NO;
}
- (void)stopConnect{
    if(!_isConnecting){
        return;
    }
    _isConnecting = NO;
    [_captureSession stopRunning];
}

#pragma mark - <协议：AVCaptureVideoDataOutputSampleBufferDelegate>
- (void)captureOutput:(AVCaptureOutput *)output didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection{
    if(nil != _sampleBufferRef){
        CFRelease(_sampleBufferRef);
    }
    _sampleBufferRef = (CMSampleBufferRef)CFRetain(sampleBuffer);
    
    CMSampleTimingInfo timingInfo[10];
    CMItemCount timingCount = 0;
    OSStatus status = CMSampleBufferGetSampleTimingInfoArray(_sampleBufferRef, 10, timingInfo, &timingCount);
    
    if (status < 0) {
        return;
    }
    
    CMTime ptsTime      = timingInfo[0].presentationTimeStamp;
    NSTimeInterval pts  = (NSTimeInterval)ptsTime.value / (NSTimeInterval)ptsTime.timescale;
    
    if (nil != _encoder) {
        CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(_sampleBufferRef);
        if (nil != imageBuffer) {
            [_encoder encode:imageBuffer timestamp:pts];
        }
    }
    else{
        
    }
}

#pragma mark - <输入、输出上下文创建和连接>
- (AVCaptureDeviceInput*) createInput:(AVCaptureDevicePosition)position{
    /// 获取所有摄像头
    AVCaptureDeviceDiscoverySession *discoverySession = [AVCaptureDeviceDiscoverySession discoverySessionWithDeviceTypes:@[AVCaptureDeviceTypeBuiltInWideAngleCamera] mediaType:AVMediaTypeVideo position:position];
    if (0 == discoverySession.devices.count) {
        return nil;
    }
    
    /// 转化为输入设备
    AVCaptureDevice *camera     = discoverySession.devices.firstObject;
    NSError *errorMessage       = nil;
    AVCaptureDeviceInput *input = [AVCaptureDeviceInput deviceInputWithDevice:camera error:&errorMessage];
    if (errorMessage){
        NSLog(@"[XXvideoRecorder] AVCaptureDevice转AVCaptureDeviceInput失败");
        return nil;
    }
    
    /// 设置帧率
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
    
    /// 设置视频数据格式
    NSDictionary *videoSetting = [NSDictionary dictionaryWithObjectsAndKeys:@(_type), kCVPixelBufferPixelFormatTypeKey, nil];
    [output setVideoSettings:videoSetting];
    
    /// 设置输出代理、串行队列和数据回调
    dispatch_queue_t queue = dispatch_queue_create("VideoOutputQueue", DISPATCH_QUEUE_SERIAL);
    [output setSampleBufferDelegate:self queue:queue];
    
    /// 丢弃延迟的帧
    output.alwaysDiscardsLateVideoFrames = YES;
    
    return output;
}
- (AVCaptureSession*) createSession:(AVCaptureDeviceInput*)videoInput VideoOuput:(AVCaptureVideoDataOutput*)videoOutput Preset:(NSString*)preset{
    if (nil == videoInput || nil == videoOutput) {
        return nil;
    }
    
    /// 采集会话
    AVCaptureSession *session = [[AVCaptureSession alloc] init];
    
    /// 不使用应用的实例，避免被异常挂断
    session.usesApplicationAudioSession = NO;
    
    /// 添加输入设备到会话
    if (![session canAddInput:videoInput]) { return nil; }
    [session addInput:videoInput];
    
    /// 添加输出设备到会话
    if (![session canAddOutput:videoOutput]) { return nil; }
    [session addOutput:videoOutput];
    
    /// 设置分辨率
    if (![session canSetSessionPreset:preset]) { return nil; }
    session.sessionPreset = preset;
    
    return session;
}
- (BOOL) connect{
    /// 输入设备
    _captureDeviceInput = [self createInput:_position];
    if (nil == _captureDeviceInput) {
        return NO;
    }
    
    /// 视频输出
    _captureVideoDataOutput = [self createVideoOutput];
    if (nil == _captureVideoDataOutput) {
        return NO;
    }
    
    /// 会话
    _captureSession = [self createSession:_captureDeviceInput VideoOuput:_captureVideoDataOutput Preset:_preset];
    if (nil == _captureSession) {
        return NO;
    }
    
    /// 获取连接并设置视频方向为竖屏方向
    AVCaptureConnection *captureVideoConnection = [_captureVideoDataOutput connectionWithMediaType:AVMediaTypeVideo];
    captureVideoConnection.videoOrientation = _orientation;
    
    /// 设置是否为镜像，前置摄像头采集到的数据本来就是翻转的，这里设置为镜像把画面转回来
    if (_position == AVCaptureDevicePositionFront && captureVideoConnection.supportsVideoMirroring){
        captureVideoConnection.videoMirrored = YES;
    }
    
    /// 获取预览Layer并设置视频方向，注意_videoPreviewLayer.connection跟_captureConnection不是同一个对象，要分开设置
    _captureVideoPreviewLayer = [AVCaptureVideoPreviewLayer layerWithSession:_captureSession];
    _captureVideoPreviewLayer.connection.videoOrientation = _orientation;
    _captureVideoPreviewLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
    
    if (nil != _preview) {
        _captureVideoPreviewLayer.frame = _preview.frame;
        [_preview.layer insertSublayer:_captureVideoPreviewLayer atIndex:0];
    }
    return YES;
}

#pragma mark - <前后置摄像头、画面选择设置>
- (BOOL) changePosition:(AVCaptureDevicePosition)position{
    if(position == _captureDeviceInput.device.position){
        return NO;
    }
    
    AVCaptureDeviceInput *input = [self createInput:position];
    if (nil == input) {
        return NO;
    }
    
    BOOL succeed = NO;
    /// 开始配置
    if (_captureSession.isRunning) {
        [_captureSession beginConfiguration];
    }
    
    /// 移除旧的输入
    [_captureSession removeInput:_captureDeviceInput];
    
    /// 检查是否可以加入新的输入
    if ([_captureSession canAddInput:input]) {
        [_captureSession addInput:input];
        _captureDeviceInput = input;
        succeed = YES;
    }
    else{
        [_captureSession addInput:_captureDeviceInput];
    }
    
    /// 获取连接并设置视频方向为竖屏方向
    AVCaptureConnection *captureVideoConnection = [_captureVideoDataOutput connectionWithMediaType:AVMediaTypeVideo];
    captureVideoConnection.videoOrientation = _orientation;
    
    /// 设置是否为镜像，前置摄像头采集到的数据本来就是翻转的，这里设置为镜像把画面转回来
    if (position == AVCaptureDevicePositionFront && captureVideoConnection.supportsVideoMirroring){
        captureVideoConnection.videoMirrored = YES;
    }
    
    /// 提交配置
    if (_captureSession.isRunning) {
        [_captureSession commitConfiguration];
    }
    return succeed;
}
- (BOOL) changeOrientation:(AVCaptureVideoOrientation)orientation{
    return NO;
}
@end
