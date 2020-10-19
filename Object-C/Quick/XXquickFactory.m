//
//  XXquickFactory.m
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "XXquickFactory.h"
#import "QuickComponentBase.h"

static XXquickFactory *_instance = nil;

@interface XXquickFactory()
@property (nonatomic,strong) NSMutableDictionary *classToComponent;
@end

@implementation XXquickFactory
- (instancetype)init{
    self = [super init];
    if (self) {
        self.classToComponent = [NSMutableDictionary new];
        
        NSString *path = [[NSBundle mainBundle] pathForResource:@"QuickInitScript" ofType:@"plist"];
        if(nil != path){
            NSDictionary *dict = [[NSDictionary alloc] initWithContentsOfFile:path];
            
            id value = dict[@"Component"];
            if([value isKindOfClass:NSArray.class]){
                NSArray *components = value;
                for (NSString *component in components) {
                    Class cls = NSClassFromString(component);
                    if(![cls respondsToSelector:@selector(targetClass)]){
                        NSLog(@"[%@] [init] 无效Component。（Component:%@）", NSStringFromClass(self.class), component);
                        continue;;
                    }
                    
                    self.classToComponent[[cls targetClass]] = cls;
                }
            }
        }
    }
    return self;
}
+(XXquickFactory*)factory{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instance = [XXquickFactory new];
    });
    return _instance;
}
+ (BOOL)quick:(id)obj data:(nonnull id)data{
    NSString *cls = NSStringFromClass([obj class]);
    id<XXquickComponentDelegate> component = [XXquickFactory factory].classToComponent[cls];
    if(nil == component){
        NSLog(@"[XXquickFactory] [quick] 没有对应的Component。obj:%@ data:%@", obj, data);
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
@end
