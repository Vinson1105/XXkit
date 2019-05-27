#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#include "../../C/XXavUtility.h"
#include "./XXencoder.h"

typedef enum : NSUInteger {
    XXvideoRecorderResult_Finished = 0,
} XXvideoRecorderResult;

NS_ASSUME_NONNULL_BEGIN

@protocol XXvideoRecorderEvents <NSObject>
- (void) videoRecorerEvent_onVideo:(CVImageBufferRef)bufferRef Timestamp:(NSTimeInterval)timestamp;
- (void) videoRecorerEvent_onAudio:(NSData*)data Timestamp:(NSTimeInterval)timestamp;
@end

@interface XXvideoRecorder : NSObject<AVCaptureVideoDataOutputSampleBufferDelegate>
@property (nonatomic,weak) XXvideoEncoder *videoEncoder;    // 内置的编码器
@property (nonatomic,assign) int frameRate;                 // 帧率
@property (nonatomic,assign) int type;                      // 输出图像类型(kCVPixelFormatType_XXX)
@property (nonatomic,copy) NSString *preset;                // 分辨率(AVCaptureSessionPresetXXX)

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
