#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>
#include "../../C/XXavUtility.h"
#include "./XXcodecBase.h"

typedef enum : NSUInteger {
    XXvideoRecorderResult_Finished = 0,
} XXvideoRecorderResult;

NS_ASSUME_NONNULL_BEGIN

@protocol XXvideoRecorderEvents <NSObject>
- (void) videoRecorerEvent_onVideo:(CVImageBufferRef)bufferRef Timestamp:(NSTimeInterval)timestamp;
- (void) videoRecorerEvent_onAudio:(NSData*)data Timestamp:(NSTimeInterval)timestamp;
@end

@interface XXvideoRecorder : NSObject<AVCaptureVideoDataOutputSampleBufferDelegate>
@property (nonatomic,weak) XXvideoEncoderBase *videoEncoder;    // 内置的编码器,当设置了该属性,会按编码输出
@property (nonatomic,assign) int frameRate;     // 帧率
@property (nonatomic,assign) int type;          // 输出图像类型(kCVPixelFormatType_XXX),注意:只有个别的类型可用,kCVPixelFormatType_32BGRA/kCVPixelFormatType_420YpCbCr8BiPlanarFullRange
@property (nonatomic,copy) NSString *preset;    // 分辨率(AVCaptureSessionPresetXXX),注意:不同机型可能有不同的可用分辨率

- (instancetype) initWithEvents:(id<XXvideoRecorderEvents>)events View:(UIView* _Nullable)view IsDefaultPositionFront:(BOOL)isFront;
- (BOOL) start;
- (void) stop;

- (BOOL) toFront;
- (BOOL) toBack;

- (BOOL) toPortrait;
- (BOOL) toLandscapeRight;
- (BOOL) toLandscapeLeft;
@end

NS_ASSUME_NONNULL_END
