//
//  NSObject+Quick.m
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "NSObject+Quick.h"
#import <objc/runtime.h>
#import "XXquickFactory.h"

@implementation NSObject(Quick)
@dynamic quick_name;
-(void)quick_reset:(id)data{
    if([self isKindOfClass:XXquickFactory.class]){
        NSLog(@"[NSObject+Quick] [quick_reset] 此实例不能使用。self:%@", self);
        return;
    }
    
    [XXquickFactory quick:self data:data];
}
@end
