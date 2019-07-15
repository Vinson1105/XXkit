#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import "XXcodecBase.h"

@interface XXh26xEncoderParam : NSObject
@property (nonatomic,assign) int width;         // 宽度
@property (nonatomic,assign) int height;        // 高度
@property (nonatomic,assign) int frameRate;     // 帧率
@property (nonatomic,assign) int bitRate;       // 比特率
@property (nonatomic,assign) BOOL isH265;       // 是否为265
+ ( XXh26xEncoderParam* _Nonnull ) paramWithWidth:(int)width Height:(int)height FrameRate:(int)frameRate BitRate:(int)bitRate IsH265:(BOOL)isH265;
@end

NS_ASSUME_NONNULL_BEGIN
@interface XXh26xEncoder : XXvideoEncoderBase
+ (id) paramWithWidth:(int)width Height:(int)height FrameRate:(int)frameRate BitRate:(int)bitRate IsH265:(BOOL)isH265;
@end
NS_ASSUME_NONNULL_END
