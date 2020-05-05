#import "XXsceneDelegate.h"

#define kSceneDelegate                  @"sceneDelegate"
#define kSigSceneDidDisconnect          @"sigSenceDidDisconnect"
#define kSigSceneWillResignActive       @"sigSenceWillResignActive"
#define kSigSceneDidEnterBackground     @"sigSenceDidEnterBackground"
#define kSigSceneDidBecomeActive        @"sigSenceDidBecomeActive"
#define kSigSceneWillEnterForeground    @"sigSenceWillEnterForeground"

@interface XXsceneDelegate ()
@end

@implementation XXsceneDelegate
- (void)scene:(UIScene *)scene willConnectToSession:(UISceneSession *)session options:(UISceneConnectionOptions *)connectionOptions  API_AVAILABLE(ios(13.0)){
}

#pragma mark - Navigation初始化
- (void)createNavigationWithRootViewController:(UIViewController*)root{
    UINavigationController *nav = [[UINavigationController alloc] initWithRootViewController:root];
    self.window.rootViewController = nav;
}
- (void)createNavigationWithIntitalViewControllerAtStoryboard:(NSString*)storyboard bundle:(nullable NSBundle*)bundle{
    UIViewController *rootVC = [[UIStoryboard storyboardWithName:storyboard bundle:bundle] instantiateInitialViewController];
    [self createNavigationWithRootViewController:rootVC];
}

#pragma mark - Scene状态
- (void)sceneDidDisconnect:(UIScene *)scene  API_AVAILABLE(ios(13.0)){
    // 已经断开
    NSDictionary *userInfo = @{kSceneDelegate:self};
    [[NSNotificationCenter defaultCenter] postNotificationName:kSigSceneDidDisconnect object:nil userInfo:userInfo];
}
- (void)sceneDidBecomeActive:(UIScene *)scene  API_AVAILABLE(ios(13.0)){
    // 已经激活
    NSDictionary *userInfo = @{kSceneDelegate:self};
    [[NSNotificationCenter defaultCenter] postNotificationName:kSigSceneDidBecomeActive object:nil userInfo:userInfo];
}
- (void)sceneWillResignActive:(UIScene *)scene  API_AVAILABLE(ios(13.0)){
    // 即将挂起
    NSDictionary *userInfo = @{kSceneDelegate:self};
    [[NSNotificationCenter defaultCenter] postNotificationName:kSigSceneWillResignActive object:nil userInfo:userInfo];
}
- (void)sceneWillEnterForeground:(UIScene *)scene  API_AVAILABLE(ios(13.0)){
    // 即将前台
    NSDictionary *userInfo = @{kSceneDelegate:self};
    [[NSNotificationCenter defaultCenter] postNotificationName:kSigSceneWillEnterForeground object:nil userInfo:userInfo];
}
- (void)sceneDidEnterBackground:(UIScene *)scene  API_AVAILABLE(ios(13.0)){
    // 已经后台
    NSDictionary *userInfo = @{kSceneDelegate:self};
    [[NSNotificationCenter defaultCenter] postNotificationName:kSigSceneDidEnterBackground object:nil userInfo:userInfo];
}
@end
