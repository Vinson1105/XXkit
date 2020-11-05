//
//  QuickComponentBase.m
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/14.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "QuickComponentBase.h"
#import "NSObject+Quick.h"

static NSString * const kSubObject = @"SubObject";
static NSString * const kClass = @"Class";
static NSString * const kQuickName = @"quick_name";
static NSString * const kQuickClass = @"quick_class";

@interface QuickComponentBase() <QuickComponentDelegate>
@end

@implementation QuickComponentBase
+ (BOOL)quick:(id)obj kvdata:(NSDictionary *)kvdata{
    NSEnumerator *keyEnumer = kvdata.keyEnumerator;
    NSString *key = nil;
    while (nil != (key = keyEnumer.nextObject)) {
        [self obj:obj key:key value:kvdata[key]];
    }
    return YES;
}

+ (NSString *)targetClass{
    if([self isMemberOfClass:QuickComponentBase.class]){
        @throw [NSExpression expressionWithFormat:@"[%@] [targetClass] 需要重载。", NSStringFromClass(self.class)];
    }
    return nil;
}
+ (void)obj:(id)obj key:(NSString*)key value:(id)value{
    if([self isMemberOfClass:QuickComponentBase.class]){
        @throw [NSExpression expressionWithFormat:@"[%@] [obj: key: value:] 需要重载。", NSStringFromClass(self.class)];
    }
    else if([key isEqualToString:kSubObject] && [value isKindOfClass:NSArray.class]){
        NSArray *array = value;
        for (id info in array) {
            id obj = [self instanceObject:info];
            if(nil == obj){
                NSLog(@"[%@] 对象实例化失败。info:%@", NSStringFromClass(self.class), info);
                continue;
            }
            
            [obj quick_addSubObject:obj];
        }
    }
    else{
        [self unexecutedKey:key value:value];
    }
}
+(void)unexecutedKey:(NSString*)key value:(id)value{
    NSLog(@"[%@] 未处理键值。key:%@ value:%@", NSStringFromClass(self.class), key, value);
}
+(nullable id)instanceObject:(id)info{
    if(![info isKindOfClass:NSDictionary.class]){
        NSDictionary *dict = info;
        NSString *cls = dict[kClass];
        NSString *quick_name = dict[kQuickName];
        NSString *quick_class = dict[kQuickClass];
        if(nil==cls||nil==quick_name){
            return nil;
        }
        
        Class c = NSClassFromString(cls);
        if(nil == c){
            return nil;
        }
        id obj = [c new];
        [obj setQuick_name:quick_name];
        if(quick_class){
            [obj setQuick_class:quick_class];
        }
        return obj;
    }
    return nil;
}
@end
