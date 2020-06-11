#import <Foundation/Foundation.h>
#import "XXavFormat.h"

NS_ASSUME_NONNULL_BEGIN
@interface XXh26xEncoder : NSObject
@property (nonatomic,copy,readonly) XXvideoFormat *format;
@property (nonatomic,copy,nullable) void(^onCompressionOutput)(XXh26xEncoder *encoder, NSData *data, NSTimeInterval dts, NSTimeInterval pts, BOOL isKeyFrame);
@property (nonatomic,copy,nullable) void(^onCompressionError)(XXh26xEncoder *encoder, NSError *error);

+ (nullable XXh26xEncoder*)encoderWithFormat:(XXvideoFormat*)format;
- (BOOL)encode:(CVImageBuffer)buffer timestamp:(NSTimeInterval)timestamp;
@end
NS_ASSUME_NONNULL_END
