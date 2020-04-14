//
//  XXappDelegate.m
//  iCamSee
//
//  Created by VINSON on 2020/3/26.
//  Copyright © 2020 Macrovideo. All rights reserved.
//

#import "XXappDelegate.h"

#define kSigApplicationWillResignActive     @"sigApplicationWillResignActive"
#define kSigApplicationDidEnterBackground   @"sigApplicationDidEnterBackground"
#define kSigApplicationDidBecomeActive      @"sigApplicationDidBecomeActive"
#define kSigApplicationWillEnterForeground  @"sigApplicationWillEnterForeground"

@interface XXappDelegate()
@property (nonatomic,assign) BOOL rotationLock;
@end

@implementation XXappDelegate
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
- (void)createNavigationWithRootViewController:(UIViewController*)root{
    self.window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
    UINavigationController *nav = [[UINavigationController alloc] initWithRootViewController:root];
    self.window.rootViewController = nav;
    [self.window makeKeyAndVisible];
}
- (void)createNavigationWithRootViewController:(NSString*)root type:(NSString*)type bundle:(nullable NSBundle*)bundle{
    if(type isEqualToString:@"")
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
    [[NSNotificationCenter defaultCenter] postNotificationName:kSigApplicationWillResignActive object:nil];
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
@end
