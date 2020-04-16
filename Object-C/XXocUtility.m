//
//  XXocUtility.m
//  XXkitProject
//
//  Created by VINSON on 2020/4/15.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "XXocUtility.h"

@implementation XXocUtility
+ (UIViewController*)viewController:(NSString*)vc withUIStoryboard:(NSString*)storyboard bundle:(nullable NSBundle*)bundle{
    return [[UIStoryboard storyboardWithName:storyboard bundle:bundle] instantiateViewControllerWithIdentifier:vc];
}

@end
