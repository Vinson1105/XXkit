/**
 2020-04-29 基于'GCD Timer'的封装封装
 */

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXtimer : NSObject
@property (nonatomic,copy) void(^onTimeout)(XXtimer *timer, int times);
@property (nonatomic,assign,readonly) BOOL isRunning;

/**
 创建timer实例
 @param delay 延迟多少秒开启
 @param interval 两次触发之间的时间间隔
 @param singleShot 是否为单次触发
 */
+(XXtimer*)timerWithDelay:(NSTimeInterval)delay interval:(NSTimeInterval)interval singleShot:(BOOL)singleShot;

/**
 timer初始化
 @param delay 延迟多少秒开启
 @param interval 两次触发之间的时间间隔
 @param singleShot 是否为单次触发
 */
-(instancetype)initWithDelay:(NSTimeInterval)delay interval:(NSTimeInterval)interval singleShot:(BOOL)singleShot;

/**
 重置定时器，会将timer停止，需要重新调用start
 @param delay 延迟多少秒开启
 @param interval 两次触发之间的时间间隔
 @param singleShot 是否为单次触发
 */
-(void)resetDelay:(NSTimeInterval)delay interval:(NSTimeInterval)interval singleShot:(BOOL)singleShot;

/**
 开启定时器
 */
-(void)start;

/**
 停止定时器
 */
-(void)stop;
@end

NS_ASSUME_NONNULL_END
