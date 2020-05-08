/**
 2020.05.08 - 音频采集，并保存到文件
 */

#import <Foundation/Foundation.h>
#import "XXaudioFormat.h"

NS_ASSUME_NONNULL_BEGIN

@class XXaudioFileRecorder;
@protocol XXaudioRecorderEvents <NSObject>
-(void)audioFileRecorder:(XXaudioFileRecorder*)recorder didOccurError:(NSError*)error;
@end

@interface XXaudioFileRecorder : NSObject
@property (nonatomic,assign,readonly) BOOL isRunning;
@property (nonatomic,assign,readonly) BOOL isRecording;
+ (XXaudioFileRecorder*)sharedInstance;
- (void)config:(XXaudioFormat*)format;
- (BOOL)start:(NSURL*)url;
- (void)pause;
- (void)resume;
- (NSURL*)stop;
@end

NS_ASSUME_NONNULL_END
