//
//  UIViewController+ApplicationState.h
//  iCamSee
//
//  Created by VINSON on 2020/1/13.
//  Copyright © 2020 Macrovideo. All rights reserved.
//

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
-(void)installApplicationState:(BOOL)install;
@end

NS_ASSUME_NONNULL_END
