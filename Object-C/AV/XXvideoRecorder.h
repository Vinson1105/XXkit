#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>
#import "XXh26xEncoder.h"

typedef enum : NSUInteger {
    XXvideoRecorderResult_Finished = 0,
} XXvideoRecorderResult;

NS_ASSUME_NONNULL_BEGIN

@protocol XXvideoRecorderEvents <NSObject>
- (void) videoRecorerEvent_onVideo:(CVImageBufferRef)bufferRef Timestamp:(NSTimeInterval)timestamp;
- (void) videoRecorerEvent_onAudio:(NSData*)data Timestamp:(NSTimeInterval)timestamp;
@end

@interface XXvideoRecorder : NSObject<AVCaptureVideoDataOutputSampleBufferDelegate>
/** 摄像头采集预览layer显示 */
@property (nonatomic,weak,nullable) UIView *preview;
/** 内置的编码器,当设置了该属性,会按编码输出 */
@property (nonatomic,weak,nullable) XXh26xEncoder *encoder;
/** 帧率，摄像头数据采集回调频率，默认15帧/S，不同的机型又不同可用帧率范围 */
@property (nonatomic,assign) int frameRate;
/** 图像类型，注意：系统只支持个别类型，如kCVPixelFormatType_32BGRA、kCVPixelFormatType_420YpCbCr8BiPlanarFullRange等 */
@property (nonatomic,assign) int type;
/** 分辨率（AVCaptureSessionPresetXXX）,注意:不同机型可能有不同的可用分辨率 */
@property (nonatomic,copy) NSString *preset;    // 分辨率(AVCaptureSessionPresetXXX),注意:不同机型可能有不同的可用分辨率

- (instancetype) initWithEvents:(id<XXvideoRecorderEvents>)events View:(UIView* _Nullable)view IsDefaultPositionFront:(BOOL)isFront;

- (BOOL)startPreview;
- (void)stopPreivew;

- (BOOL)startRecord;
- (void)stopRecord;

- (BOOL) start;
- (void) stop;

- (BOOL) toFront;
- (BOOL) toBack;

- (BOOL) toPortrait;
- (BOOL) toLandscapeRight;
- (BOOL) toLandscapeLeft;
@end

NS_ASSUME_NONNULL_END
