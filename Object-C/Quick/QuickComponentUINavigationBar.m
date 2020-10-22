//
//  QuickComponentUINavigationBar.m
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/22.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "QuickComponentUINavigationBar.h"
#import <UIKit/UIKit.h>

static NSString * const kTranslucent = @"Translucent";

@implementation QuickComponentUINavigationBar
+ (NSString *)targetClass{
    return NSStringFromClass(UINavigationBar.class);
}
+ (void)obj:(id)obj key:(NSString *)key value:(id)value{
    UINavigationBar *navigationBar = obj;
    
    // MARK: TransLucent
    if([key isEqualToString:kTranslucent]){
        navigationBar.translucent = [value boolValue];
    }
    else{
        [super obj:obj key:key value:value];
    }
}
@end
