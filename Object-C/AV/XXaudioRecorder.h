/**
 2020.03.14 - 音频实时采集器
 */

#import <Foundation/Foundation.h>
#import "XXaudioFormat.h"

NS_ASSUME_NONNULL_BEGIN

@class XXaudioRecorder;
@protocol XXaudioRecorderEvents <NSObject>
-(void)audioRecorder:(XXaudioRecorder*)recorder receiveData:(NSData*)data;
@end

@interface XXaudioRecorder : NSObject
@property (nonatomic,assign,readonly) BOOL isRunning;
@property (nonatomic,weak) id<XXaudioRecorderEvents> events;

/** 单例 */
+ (XXaudioRecorder*)sharedInstance;

/** 配置音频格式，需要在start之前调用 */
- (void)config:(XXaudioFormat*)format;

/** 开启 */
- (void)start;

/** 停止 */
- (void)stop;
@end

NS_ASSUME_NONNULL_END

