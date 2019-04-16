#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#include "../../C/XXavUtility.h"

typedef enum : NSUInteger {
    XXvideoRecorderResult_Finished = 0,
} XXvideoRecorderResult;

@protocol XXvideoRecorderEvents <NSObject>
- (void) videoRecorderEvent_onData:(NSData*)data Timestamp:(int64_t)timestamp IsKeyFrame:(BOOL)isKeyFrame;
- (void) videoRecorderEvent_onError:(NSString*)message;
- (void) videoRecorderEvent_onFinished:(XXvideoRecorderResult)result;
@end

NS_ASSUME_NONNULL_BEGIN

@interface XXvideoRecorder : NSObject<AVCaptureVideoDataOutputSampleBufferDelegate>
- (instancetype) initWithEvents:(id<XXvideoRecorderEvents>)events;// VideoType:(XXavFrameType)videoType;
- (BOOL) start;
- (void) stop;
@end

NS_ASSUME_NONNULL_END
