//
//  QuickComponentUINavigationBar.m
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/22.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "QuickComponentUINavigationBar.h"
#import <UIKit/UIKit.h>
#import "../XXocUtils.h"

static NSString * const kTranslucent = @"Translucent";
static NSString * const kBackgroundColor = @"BackgroundColor";
static NSString * const kBackItemColor = @"BackItemColor";

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
    
    // MARK: BackgroundColor
    else if([key isEqualToString:kBackgroundColor]){
        navigationBar.barTintColor = [XXocUtils autoColor:value];
    }
    
    // MARK: BackItemColor
    else if([key isEqualToString:kBackItemColor]){
        navigationBar.tintColor = [XXocUtils autoColor:value];
    }
    
    else{
        [super obj:obj key:key value:value];
    }
}
@end
