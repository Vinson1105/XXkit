#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import "XXencoder.h"

@interface XXh26xEncoderParam : NSObject<NSCopying>
@property (nonatomic,assign) int width;
@property (nonatomic,assign) int height;
@property (nonatomic,assign) int framerate;
@property (nonatomic,assign) int timescale;
@end

NS_ASSUME_NONNULL_BEGIN
@interface XXh26xEncoder : XXvideoEncoder
- (instancetype) initWithName:(NSString*)name Events:(id<XXvideoEncoderEvents> _Nullable)events Param:(XXh26xEncoderParam*)param;
@end
NS_ASSUME_NONNULL_END
