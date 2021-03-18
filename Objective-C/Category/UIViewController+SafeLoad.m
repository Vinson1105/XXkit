//
//  UIViewController+SafeLoad.m
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/23.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "UIViewController+SafeLoad.h"

@implementation UIViewController(SafeLoad)
+(UIViewController*)safeLoad_init{
    @try {
        UIViewController *vc = [[UIStoryboard storyboardWithName:@"Main"
                                                          bundle:nil]
                                instantiateViewControllerWithIdentifier:NSStringFromClass(self.class)];
        return vc;
    } @catch (NSException *exception) {
        return [[self class] new];
    } @finally {
        
    }
}

@end
