#import <Foundation/Foundation.h>
#import <CoreVideo/CoreVideo.h>

NS_ASSUME_NONNULL_BEGIN
@protocol XXencoderBase <NSObject>
@property (nonatomic,copy) NSString *name;
- (instancetype) initWithName:(NSString*)name;
- (BOOL) isVideoEncoder;
- (BOOL) start;
- (BOOL) encode:(CVImageBufferRef)buffer Timestamp:(NSTimeInterval)timestamp;
- (void) stop;
@end

@protocol XXvideoEncoderEvents <NSObject>
- (void) videoEncoderEvent_onData:(NSString*)name Data:(NSData*)data Dts:(NSTimeInterval)dts Pts:(NSTimeInterval)pts IsKeyFrame:(BOOL)isKeyFrame;
- (void) videoEncoderEvent_onError:(NSString*)name Message:(NSString*)message;
@end

@protocol XXaudioEncoderEvents <NSObject>
- (void) audioEncoderEvent_onData:(NSString*)name Data:(NSData*)data Timestamp:(int64_t)timestamp;
- (void) audioEncoderEvent_onError:(NSString*)name Message:(NSString*)message;
@end

@interface XXvideoEncoder : NSObject <XXencoderBase>
@end
NS_ASSUME_NONNULL_END
