//
//  UIViewController+ApplicationState.m
//  iCamSee
//
//  Created by VINSON on 2020/1/13.
//  Copyright © 2020 Macrovideo. All rights reserved.
//

#import "UIViewController+ApplicationState.h"
#import <objc/runtime.h>

@implementation UIViewController(ApplicationState)
- (void)setOnApplicationStateChanged:(void (^)(ApplicationState))onApplicationStateChanged{
    objc_setAssociatedObject(self, @"setOnApplicationStateChanged", onApplicationStateChanged, OBJC_ASSOCIATION_COPY);
}
- (void (^)(ApplicationState))onApplicationStateChanged{
    return objc_getAssociatedObject(self, @"setOnApplicationStateChanged");
}

-(void)installApplicationState:(BOOL)install{
    if(install){
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onApplicationWillResignActive:) name:@"sigApplicationWillResignActive" object:nil];       // 即将进入后台
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onApplicationDidEnterBackground:) name:@"sigApplicationDidEnterBackground" object:nil];   // 已经进入后台
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onApplicationWillEnterForeground:) name:@"sigApplicationWillEnterForeground" object:nil]; // 即将进入前台
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onApplicationDidBecomeActive:) name:@"sigApplicationDidBecomeActive" object:nil];         // 已经进入后台
    }
    else{
        [[NSNotificationCenter defaultCenter] removeObserver:self name:@"sigApplicationWillResignActive" object:nil];
        [[NSNotificationCenter defaultCenter] removeObserver:self name:@"sigApplicationDidEnterBackground" object:nil];
        [[NSNotificationCenter defaultCenter] removeObserver:self name:@"sigApplicationWillEnterForeground" object:nil];
        [[NSNotificationCenter defaultCenter] removeObserver:self name:@"sigApplicationDidBecomeActive" object:nil];
    }
}
-(void)onApplicationDidBecomeActive:(NSNotification*)notifition{
    if(self.onApplicationStateChanged){
        self.onApplicationStateChanged(ApplicationState_didBecomeActive);
    }
}
-(void)onApplicationWillEnterForeground:(NSNotification*)notifition{
    if(self.onApplicationStateChanged){
        self.onApplicationStateChanged(ApplicationState_willEnterForeground);
    }
}
-(void)onApplicationWillResignActive:(NSNotification*)notifition{
    if(self.onApplicationStateChanged){
        self.onApplicationStateChanged(ApplicationState_willResignActive);
    }
}
-(void)onApplicationDidEnterBackground:(NSNotification*)notifition{
    if(self.onApplicationStateChanged){
        self.onApplicationStateChanged(ApplicationState_didEnterBackground);
    }
}
@end
