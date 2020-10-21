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
#import "../Category/NSString+JSON.h"

@implementation NSObject(Quick)
-(void)quick_reset:(id)data{
    if([self isKindOfClass:XXquickFactory.class]){
        NSLog(@"[NSObject+Quick] [quick_reset] 此实例不能使用。self:%@", self);
        return;
    }
    
    [XXquickFactory quick:self data:data];
}
-(void)quick_resetFromJFile:(NSString*)file bundle:(nullable NSBundle*)bundle{
    if(nil == bundle){
        bundle = [NSBundle mainBundle];
    }
    NSString *path = [bundle pathForResource:file ofType:nil];
    if(nil == path){
        return;
    }
    NSData *data = [NSData dataWithContentsOfFile:path];
    NSError *error = nil;
    id jobj = [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:&error];
    if(error){
        return;
    }
    
    [self quick_reset:jobj];
}
- (void)setQuick_name:(NSString *)quick_name{
    objc_setAssociatedObject(self, "quick_name", quick_name, OBJC_ASSOCIATION_COPY_NONATOMIC);
}
- (NSString *)quick_name{
    return objc_getAssociatedObject(self, "quick_name");
}
@end
