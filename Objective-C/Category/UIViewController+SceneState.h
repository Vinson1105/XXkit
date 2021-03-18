/**
 2020.05.05
 1、内置Scene的状态通知
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

typedef enum : NSUInteger {
    SceneState_didDisconnect,           // 即将挂起
    SceneState_willResignActive,        // 即将挂起
    SceneState_didEnterBackground,      // 已经进入后台
    SceneState_willEnterForeground,     // 即将进入前台
    SceneState_didBecomeActive,         // 已经激活
} SceneState;

@class XXsceneDelegate;
@interface UIViewController(SceneState)
@property (nonatomic,copy) void(^onSceneStateChanged)(SceneState state, XXsceneDelegate *sceneDelegate);

/**
 装载通知处理
 @param install YES：装载，NO：移除，装载之后需要在dealloc处需要调用[self installSceneState:NO]
 */
-(void)installSceneState:(BOOL)install;
@end

NS_ASSUME_NONNULL_END
