//
//  XXappDelegate.m
//  ProjectTest
//
//  Created by VINSON on 2020/1/18.
//  Copyright © 2020 VINSON. All rights reserved.
//

#import "XXappDelegate.h"
#define kXXapplicationWillResignActive      @"XXapplicationWillResignActive"
#define kXXapplicationDidEnterBackground    @"XXapplicationDidEnterBackground"
#define kXXapplicationWillEnterForeground   @"XXapplicationWillEnterForeground"
#define kXXapplicationDidBecomeActive       @"XXapplicationDidBecomeActive"
#define kXXapplicationWillTerminate         @"XXapplicationWillTerminate"

#define kDefaultOrientation                 UIInterfaceOrientationMaskPortrait

@implementation XXappDelegate
- (void)applicationWillResignActive:(UIApplication *)application {
    // 挂起
    [[NSNotificationCenter defaultCenter] postNotificationName:kXXapplicationWillResignActive object:nil];
}
- (void)applicationDidEnterBackground:(UIApplication *)application {
    // 后台
    [[NSNotificationCenter defaultCenter] postNotificationName:kXXapplicationDidEnterBackground object:nil];
}
- (void)applicationWillEnterForeground:(UIApplication *)application {
    // 前台
    [[NSNotificationCenter defaultCenter] postNotificationName:kXXapplicationWillEnterForeground object:nil];
}
- (void)applicationDidBecomeActive:(UIApplication *)application {
    // 活跃
    [[NSNotificationCenter defaultCenter] postNotificationName:kXXapplicationDidBecomeActive object:nil];
}


- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
    [[NSNotificationCenter defaultCenter] postNotificationName:kXXapplicationWillTerminate object:nil];
}
- (UIInterfaceOrientationMask)application:(UIApplication *)application supportedInterfaceOrientationsForWindow:(UIWindow *)window{
    if (self.orientationEnabled) {
        return self.orientation;
    }
    else{
        return 0;
    }
}
@end
