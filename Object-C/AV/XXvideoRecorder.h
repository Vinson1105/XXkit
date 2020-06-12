#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>
#import "XXh26xEncoder.h"

NS_ASSUME_NONNULL_BEGIN

@interface XXvideoRecorder : NSObject<AVCaptureVideoDataOutputSampleBufferDelegate>
/** 摄像头采集预览layer显示 */
@property (nonatomic,weak,nullable) UIView *preview;

/** 内置的编码器,当设置了该属性,会按编码输出 */
@property (nonatomic,strong,nullable) XXh26xEncoder *encoder;

/** 帧率，摄像头数据采集回调频率，默认15帧/S，不同的机型又不同可用帧率范围（需要重新start生效） */
@property (nonatomic,assign) int frameRate;

/** 图像类型，注意：系统只支持个别类型，如kCVPixelFormatType_32BGRA（默认）、kCVPixelFormatType_420YpCbCr8BiPlanarFullRange等*/
@property (nonatomic,assign) int type;

/** 分辨率（AVCaptureSessionPresetXXX）,注意:不同机型可能有不同的可用分辨率，默认：AVCaptureSessionPresetHigh */
@property (nonatomic,copy) NSString *preset;

/** 是否为连接中，如果没有设置preview，是否为connect没有实质的可视化影响 */
@property (nonatomic,assign,readonly) BOOL isConnecting;

/** 是否在录制中 */
@property (nonatomic,assign,readonly) BOOL isRecording;

/** 采集位置，默认：AVCaptureDevicePositionBack */
@property (nonatomic,assign) AVCaptureDevicePosition position;

/** 镜头方向，默认：AVCaptureVideoOrientationPortrait */
@property (nonatomic,assign) AVCaptureVideoOrientation orientation;

+ (XXvideoRecorder*)sharedInstance;
- (BOOL)startConnect;
- (BOOL)startrecord;
- (void)stopRecord;
- (void)stopConnect;
@end

NS_ASSUME_NONNULL_END
