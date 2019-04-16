#import "XXvideoRecorder.h"

@interface XXvideoRecorder()
@property (nonatomic,assign) BOOL isInterruptionRequested;
@property (nonatomic,weak) id<XXvideoRecorderEvents> events;

@property (nonatomic,strong) AVCaptureDeviceInput *captureDeviceInput;
@property (nonatomic,strong) AVCaptureVideoDataOutput *captureVideoDataOutput;
@property (nonatomic,strong) dispatch_queue_t videoCaptureOutputQueue;
@end

@implementation XXvideoRecorder
- (instancetype) initWithEvents:(id<XXvideoRecorderEvents>)events{//} VideoType:(XXvideoRecorderVideoType)videoType{
    self = [super init];
    if (self) {
        _events = events;
    }
    return self;
}
- (BOOL) start{
    // 获取所有摄像头
    AVCaptureDeviceDiscoverySession *discoverySession = [AVCaptureDeviceDiscoverySession discoverySessionWithDeviceTypes:@[AVCaptureDeviceTypeBuiltInWideAngleCamera] mediaType:AVMediaTypeVideo position: AVCaptureDevicePositionUnspecified];
    if (0 == discoverySession.devices.count) {
        return NO;
    }
    
    // 转化为输入设备
    AVCaptureDevice *camera = discoverySession.devices.firstObject;
    NSError *errorMessage = nil;
    self.captureDeviceInput = [AVCaptureDeviceInput deviceInputWithDevice:camera error:&errorMessage];
    if (errorMessage){
        NSLog(@"AVCaptureDevice转AVCaptureDeviceInput失败");
        return NO;
    }
    
    // 设置视频输出
    self.captureVideoDataOutput = [[AVCaptureVideoDataOutput alloc] init];
    
    // 设置视频数据格式
    NSDictionary *videoSetting = [NSDictionary dictionaryWithObjectsAndKeys:@(kCVPixelFormatType_420YpCbCr8BiPlanarFullRange), kCVPixelBufferPixelFormatTypeKey, nil];
    [self.captureVideoDataOutput setVideoSettings:videoSetting];
    
    // 设置输出代理、串行队列和数据回调
    self.videoCaptureOutputQueue = dispatch_queue_create("VideoCaptureOutputQueue", DISPATCH_QUEUE_SERIAL);
    [self.captureVideoDataOutput setSampleBufferDelegate:self queue:self.videoCaptureOutputQueue];
    
    // 丢弃延迟的帧
    self.captureVideoDataOutput.alwaysDiscardsLateVideoFrames = YES;
    
    return YES;
}
- (void) stop{
    
}



@end
