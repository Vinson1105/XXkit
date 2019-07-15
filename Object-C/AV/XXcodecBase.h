#import <Foundation/Foundation.h>
#import <CoreVideo/CoreVideo.h>

NS_ASSUME_NONNULL_BEGIN
//
// encoder
//
@protocol XXvideoEncoderEvents <NSObject>
- (void) videoEncoderEvent_onData:(NSString*)name Data:(NSData*)data Dts:(NSTimeInterval)dts Pts:(NSTimeInterval)pts IsKeyFrame:(BOOL)isKeyFrame;
- (void) videoEncoderEvent_onError:(NSString*)name Message:(NSString*)message;
@end

@protocol XXaudioEncoderEvents <NSObject>
- (void) audioEncoderEvent_onData:(NSString*)name Data:(NSData*)data Timestamp:(int64_t)timestamp;
- (void) audioEncoderEvent_onError:(NSString*)name Message:(NSString*)message;
@end

@interface XXvideoEncoderBase : NSObject
@property (nonatomic,copy) NSString *name;
@property (nonatomic,weak) id<XXvideoEncoderEvents> events;
- (instancetype) initWithName:(NSString*)name Param:(id)param Events:(id<XXvideoEncoderEvents> _Nullable)events;
- (BOOL) start;
- (BOOL) encode:(CVImageBufferRef)buffer Timestamp:(NSTimeInterval)timestamp;
- (void) stop;
@end

//
// decoder
//
@protocol XXvideoDecoderEvents <NSObject>
- (void) videoDecoderEvent_onData:(NSString*)name Data:(NSData*)data Dts:(NSTimeInterval)dts Pts:(NSTimeInterval)pts IsKeyFrame:(BOOL)isKeyFrame;
@end
@interface XXvideoDecoderBase : NSObject
@property (nonatomic,copy) NSString *name;
@property (nonatomic,weak) id<XXvideoDecoderEvents> events;
- (instancetype) initWithName:(NSString*)name Param:(id)param Events:(id<XXvideoDecoderEvents> _Nullable)events;
- (void) decode:(NSData*)data;
@end
NS_ASSUME_NONNULL_END
