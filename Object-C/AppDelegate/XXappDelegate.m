#import "XXappDelegate.h"

#define kSigApplicationWillResignActive     @"sigApplicationWillResignActive"
#define kSigApplicationDidEnterBackground   @"sigApplicationDidEnterBackground"
#define kSigApplicationDidBecomeActive      @"sigApplicationDidBecomeActive"
#define kSigApplicationWillEnterForeground  @"sigApplicationWillEnterForeground"

#define isUsingScene 0

@interface XXappDelegate()
@property (nonatomic,assign) BOOL rotationLock;
@end

@implementation XXappDelegate
#pragma mark - 横竖屏锁定
+ (void)rotationLock:(BOOL)lock{
    if ([NSThread currentThread].isMainThread) {
        XXappDelegate *appDelegate = (XXappDelegate*)[UIApplication sharedApplication].delegate;
        appDelegate.rotationLock = lock;
    }
    else{
        dispatch_async(dispatch_get_main_queue(), ^{
            XXappDelegate *appDelegate = (XXappDelegate*)[UIApplication sharedApplication].delegate;
            appDelegate.rotationLock = lock;
        });
    }
}
+ (BOOL)isRotationLocked{
    if ([NSThread currentThread].isMainThread) {
        XXappDelegate *appDelegate = (XXappDelegate*)[UIApplication sharedApplication].delegate;
        return appDelegate.rotationLock;
    }
    else{
        __block BOOL isLocked = NO;
        dispatch_sync(dispatch_get_main_queue(), ^{
            XXappDelegate *appDelegate = (XXappDelegate*)[UIApplication sharedApplication].delegate;
            isLocked = appDelegate.rotationLock;
        });
        return isLocked;
    }
}

#pragma mark - Navigation初始化
- (void)createNavigationWithRootViewController:(UIViewController*)root{
    if(nil == self.window){
        self.window = [UIApplication sharedApplication].windows.firstObject;
    }
    if(nil == self.window){
        self.window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
    }
    
    UINavigationController *nav = [[UINavigationController alloc] initWithRootViewController:root];
    self.window.rootViewController = nav;
    [self.window makeKeyAndVisible];
}
- (void)createNavigationWithIntitalViewControllerAtStoryboard:(NSString*)storyboard bundle:(nullable NSBundle*)bundle{
    UIViewController *rootVC = [[UIStoryboard storyboardWithName:storyboard bundle:bundle] instantiateInitialViewController];
    [self createNavigationWithRootViewController:rootVC];
}

#pragma mark - APP加载完成
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary<UIApplicationLaunchOptionsKey,id> *)launchOptions{
    return YES;
}

#pragma mark - APP状态
- (void)applicationWillResignActive:(UIApplication *)application{
    // 即将挂起
    [[NSNotificationCenter defaultCenter] postNotificationName:kSigApplicationWillResignActive object:nil];
}
- (void)applicationDidEnterBackground:(UIApplication *)application{
    // 已经后台
    [[NSNotificationCenter defaultCenter] postNotificationName:kSigApplicationWillEnterForeground object:nil];
}
- (void)applicationWillEnterForeground:(UIApplication *)application{
    // 即将前台
    [[NSNotificationCenter defaultCenter] postNotificationName:kSigApplicationWillEnterForeground object:nil];
}
- (void)applicationDidBecomeActive:(UIApplication *)application{
    // 已经激活
    [[NSNotificationCenter defaultCenter] postNotificationName:kSigApplicationDidBecomeActive object:nil];
}

#pragma mark - 旋转
- (UIInterfaceOrientationMask)application:(UIApplication *)application supportedInterfaceOrientationsForWindow:(nullable UIWindow *)window {
    if (!_rotationLock) {
        return UIInterfaceOrientationMaskLandscape|UIInterfaceOrientationMaskPortrait;
    }
    else{
        UIInterfaceOrientation orientation = application.statusBarOrientation;
        if(orientation == UIInterfaceOrientationPortrait){
            return UIInterfaceOrientationMaskPortrait;
        }
        else{
            return UIInterfaceOrientationMaskLandscape;
        }
    }
}

#pragma mark - UISceneSession lifecycle
#if isUsingScene
- (UISceneConfiguration *)application:(UIApplication *)application configurationForConnectingSceneSession:(UISceneSession *)connectingSceneSession options:(UISceneConnectionOptions *)options  API_AVAILABLE(ios(13.0)){
    // Called when a new scene session is being created.
    // Use this method to select a configuration to create the new scene with.
    return [[UISceneConfiguration alloc] initWithName:@"Default Configuration" sessionRole:connectingSceneSession.role];
}

- (void)application:(UIApplication *)application didDiscardSceneSessions:(NSSet<UISceneSession *> *)sceneSessions  API_AVAILABLE(ios(13.0)){
    // Called when the user discards a scene session.
    // If any sessions were discarded while the application was not running, this will be called shortly after application:didFinishLaunchingWithOptions.
    // Use this method to release any resources that were specific to the discarded scenes, as they will not return.
}
#endif
@end
