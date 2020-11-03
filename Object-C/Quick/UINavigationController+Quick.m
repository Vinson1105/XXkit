//
//  UINavigationController+Quick.m
//  XXkitProject
//
//  Created by VINSON on 2020/10/24.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "UINavigationController+Quick.h"
#import "../XXocUtils.h"
#import "QuickLoader.h"

@implementation UINavigationController(Quick)
+ (void)load{
    [XXocUtils replaceMethod:[self class] src:@selector(pushViewController:animated:) dest:@selector(quick_pushViewController:animated:)];
}
- (void)quick_pushViewController:(UIViewController *)viewController animated:(BOOL)animated{
    [self quick_pushViewController:viewController animated:animated];
    [QuickLoader page:viewController];
}
@end
