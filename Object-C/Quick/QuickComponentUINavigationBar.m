//
//  QuickComponentUINavigationBar.m
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/22.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "QuickComponentUINavigationBar.h"
#import <UIKit/UIKit.h>

static NSString * const kBackImage = @"BackImage";

@implementation QuickComponentUINavigationBar
+ (NSString *)targetClass{
    return NSStringFromClass(UINavigationBar.class);
}
+ (void)obj:(id)obj key:(NSString *)key value:(id)value{
    UINavigationBar *nav = obj;
    
    if([key isEqualToString:kBackImage]){
        
    }
    else{
        
    }
}
@end
