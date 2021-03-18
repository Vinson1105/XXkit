/**
 2020.05.08 - 音频采集，并保存到文件
 */

#import <Foundation/Foundation.h>
#import "XXavFormat.h"

NS_ASSUME_NONNULL_BEGIN

@class XXaudioFileRecorder;
@protocol XXaudioRecorderEvents <NSObject>
-(void)audioFileRecorder:(XXaudioFileRecorder*)recorder didOccurError:(NSError*)error;
@end

@interface XXaudioFileRecorder : NSObject
@property (nonatomic,assign,readonly) BOOL isRunning;
@property (nonatomic,assign,readonly) BOOL isRecording;
@property (nonatomic,weak) id<XXaudioRecorderEvents> events;
+ (XXaudioFileRecorder*)sharedInstance;
- (void)config:(XXaudioFormat*)format;
- (BOOL)start:(NSURL*)url;
- (void)pause;
- (BOOL)resume;
- (nullable NSURL*)stop;
@end

NS_ASSUME_NONNULL_END
