//
//  QuickObjectAlloter.m
//  XXkitProject
//
//  Created by VINSON on 2020/11/4.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "QuickObjectAlloter.h"
#import "NSObject+Quick.h"

static NSString * const kClass = @"Class";
static NSString * const kQuickName = @"quick_name";
static NSString * const kQuickClass = @"quick_class";

static QuickObjectAlloter *_instance = nil;

@interface QuickObjectAlloter()

@end

@implementation QuickObjectAlloter
-(nullable id)instanceObject:(id)info{
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
-(void)addAlloterWithClass:(NSString*)alloter{
    
}

+(QuickObjectAlloter*)sharedIntance{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instance = [QuickObjectAlloter new];
    });
    return _instance;
}
+(nullable id)instanceObject:(id)info{
    return [[QuickObjectAlloter sharedIntance] instanceObject:info];
}
+(void)addAlloterWithClass:(NSString*)alloter{
    [[QuickObjectAlloter sharedIntance] addAlloterWithClass:alloter];
}
@end
