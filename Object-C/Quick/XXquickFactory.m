//
//  XXquickFactory.m
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "XXquickFactory.h"
#import "QuickComponentBase.h"
#import "NSObject+Quick.h"
#import <objc/runtime.h>

static XXquickFactory *_instance = nil;

struct simulation_objc_class {
    Class _Nonnull isa;
    Class _Nullable superclass;
};

static NSString * const kDefaulteScript = @"QuickInitScript";

@interface XXquickFactory()
@property (nonatomic,strong) NSMutableDictionary *classToComponent;
@end

@implementation XXquickFactory
- (instancetype)init{
    self = [super init];
    if (self) {
        self.classToComponent = [NSMutableDictionary new];
        [self loadDefaultComponentFromPlist:kDefaulteScript toComponentDict:self.classToComponent];
    }
    return self;
}
-(void)loadDefaultComponentFromPlist:(NSString*)filename toComponentDict:(NSMutableDictionary*)componentDict{
    NSString *path = [[NSBundle mainBundle] pathForResource:filename ofType:@"plist"];
    if(nil == path){
        return;
    }
    
    NSDictionary *script = [[NSDictionary alloc] initWithContentsOfFile:path];
    
    id value = script[@"Component"];
    if([value isKindOfClass:NSArray.class]){
        NSArray *components = value;
        for (NSString *component in components) {
            Class cls = NSClassFromString(component);
            if(![cls respondsToSelector:@selector(targetClass)]){
                NSLog(@"[%@] [init] 无效Component。（Component:%@）", NSStringFromClass(self.class), component);
                continue;;
            }
            
            componentDict[[cls targetClass]] = cls;
        }
    }
}
+(XXquickFactory*)factory{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instance = [XXquickFactory new];
    });
    return _instance;
}
+ (BOOL)quick:(id)obj data:(nonnull id)data{
    NSString *cls = [obj quick_class];
    if(nil == cls){
        cls = NSStringFromClass([obj class]);
    }
    NSString *quickCls = [self findRelationshipClass:cls AtClasss:[XXquickFactory factory].classToComponent.allKeys];
    if(nil == quickCls){
        NSLog(@"[XXquickFactory] [quick] 没有对应的QuickClass。obj:%@ obj.class:%@ obj.quick_class:%@ data:%@",
              obj,
              NSStringFromClass([obj class]),
              [obj quick_class],
              data);
        return NO;
    }
    
    
    id<XXquickComponentDelegate> component = [XXquickFactory factory].classToComponent[quickCls];
    if(nil == component){
        NSLog(@"[XXquickFactory] [quick] 没有对应的Component。obj:%@ obj.class:%@ obj.quick_class:%@ data:%@",
              obj,
              NSStringFromClass([obj class]),
              [obj quick_class],
              data);
        return NO;
    }
    
    if([data isKindOfClass:NSString.class]){
        NSString *jstring = data;
        NSData *jdata = [jstring dataUsingEncoding:NSUTF8StringEncoding];
        NSError *error = nil;
        id jobj = [NSJSONSerialization JSONObjectWithData:jdata options:kNilOptions error:&error];
        if(error){
            NSLog(@"[XXquickFactory] [quick] 转换到JSON失败。obj:%@ data:%@ error:%@", obj, data, error);
            return NO;
        }
        
        if(![jobj isKindOfClass:NSDictionary.class]){
            NSLog(@"[XXquickFactory] [quick] JOBJ并不是NSDictionary类型。");
            return NO;
        }
        
        return [component quick:obj kvdata:jobj];
    }
    else if([data isKindOfClass:NSDictionary.class]){
        return [component quick:obj kvdata:data];
    }
    else{
        return NO;
    }
}
+(void)installComponentClass:(NSString*)component{
    Class cls = NSClassFromString(component);
    if(![cls instancesRespondToSelector:@selector(targetClass)]){
        NSLog(@"[%@] [installComponentClass] 无效Component。（Component:%@）", NSStringFromClass(self.class), component);
        return;
    }
    
    [XXquickFactory factory].classToComponent[[cls targetClass]] = cls;
}

+ (nullable NSString*)findRelationshipClass:(NSString*)class AtClasss:(NSArray<NSString*>*)classStrings{
    NSString *member = nil;
    NSString *kind = nil;
    
    Class srcClass = NSClassFromString(class);
    for (NSString *destString in classStrings) {
        Class destClass = NSClassFromString(destString);
        if(srcClass == destClass){
            member = destString;
            break;
        }
        else {
            for(Class tcls = srcClass; tcls; tcls = ((__bridge struct simulation_objc_class *)(tcls))->superclass) {
                if(tcls == destClass) {
                    kind = destString;
                    break;
                }
            }
        }
    }
    
    if(member) return member;
    if(kind) return kind;
    return nil;
}
@end
