/**
 2020-03-25
 1、内置App的状态通知，配合XXappDelegate使用
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

typedef enum : NSUInteger {
    ApplicationState_willResignActive,      // 即将挂起
    ApplicationState_didEnterBackground,    // 已经进入后台
    ApplicationState_willEnterForeground,   // 即将进入前台
    ApplicationState_didBecomeActive,       // 已经激活
} ApplicationState;

@interface UIViewController(ApplicationState)
@property (nonatomic,copy) void(^onApplicationStateChanged)(ApplicationState state);
/**
 装载通知处理
 @param install YES：装载，NO：移除，装载之后需要在dealloc处需要调用[self installApplicationState:NO]
 */
-(void)installApplicationState:(BOOL)install;   // 对象释放之前需要[self installApplicationState:NO]
@end

NS_ASSUME_NONNULL_END
