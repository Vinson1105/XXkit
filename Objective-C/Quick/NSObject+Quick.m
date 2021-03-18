//
//  NSObject+Quick.m
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "NSObject+Quick.h"
#import <objc/runtime.h>
#import "QuickLoader.h"
#import "../Category/NSString+JSON.h"

@implementation NSObject(Quick)
-(void)quick_reset:(id)data{
    if([self isKindOfClass:QuickLoader.class]){
        NSLog(@"[NSObject+Quick] [quick_reset] 此实例不能使用。self:%@", self);
        return;
    }
    
    [QuickLoader item:self data:data];
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
-(void)quick_addSubObject:(id)obj{
    NSString *name = [obj quick_name];
    if(nil == name){
        return;
    }
    NSMutableDictionary *dict = (NSMutableDictionary*)self.quick_nameToSubObject;
    dict[name] = obj;
}
-(void)quick_removeSubObject:(id)obj{
    NSString *name = [obj quick_name];
    if(nil == name){
        return;
    }
    NSMutableDictionary *dict = (NSMutableDictionary*)self.quick_nameToSubObject;
    [dict removeObjectForKey:name];
}
-(void)quick_removeSubObjectWithName:(NSString*)name{
    if(nil == name){
        return;
    }
    NSMutableDictionary *dict = (NSMutableDictionary*)self.quick_nameToSubObject;
    [dict removeObjectForKey:name];
}

- (void)setQuick_name:(NSString *)quick_name{
    objc_setAssociatedObject(self, "quick_name", quick_name, OBJC_ASSOCIATION_COPY_NONATOMIC);
}
- (NSString *)quick_name{
    return objc_getAssociatedObject(self, "quick_name");
}
- (void)setQuick_class:(NSString *)quick_class{
    objc_setAssociatedObject(self, "quick_class", quick_class, OBJC_ASSOCIATION_COPY_NONATOMIC);
}
- (NSString *)quick_class{
    return objc_getAssociatedObject(self, "quick_class");
}
- (NSDictionary *)quick_nameToSubObject{
    id obj = objc_getAssociatedObject(self, "quick_nameToSubObject");
    if(nil == obj){
        NSMutableDictionary *dict = [NSMutableDictionary new];
        obj = dict;
        objc_setAssociatedObject(self, "quick_nameToSubObject", obj, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return obj;
}
@end
