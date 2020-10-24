//
//  QuickLoader.m
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "QuickLoader.h"
#import "QuickComponentBase.h"
#import "NSObject+Quick.h"
#import <objc/runtime.h>
#import "../Category/UIViewController+SafeLoad.h"

static QuickLoader *_instance = nil;

struct simulation_objc_class {
    Class _Nonnull isa;
    Class _Nullable superclass;
};

static NSString * const kDefaulteScript = @"QuickInitScript";
static NSString * const kQuickComponent = @"QuickComponent";
static NSString * const kMainQuickFile = @"MainQuickFile";
    static NSString * const kName = @"Name";
    static NSString * const kBundle = @"bundle";

@interface QuickLoader()
@property (nonatomic,strong) NSMutableDictionary *classToComponent;
@property (nonatomic,strong) NSMutableDictionary *mainQuickData;
@end

@implementation QuickLoader
#pragma mark - 初始化
- (instancetype)init{
    self = [super init];
    if (self) {
        self.classToComponent = [NSMutableDictionary new];
        NSDictionary *script = [self loadQuickInitScriptFromPlist:kDefaulteScript bundle:nil];
        if(script){
            NSMutableDictionary *dict = [self loadQuickComponentFromInitScript:script];
            if(dict){
                self.classToComponent = dict;
            }
            
            dict = [self loadMainQuickDataFromInitScript:script];
            self.mainQuickData = dict;
        }
    }
    return self;
}

#pragma mark - ‘MainQuickData’的设置
- (void)setMainQuickDataFromFileName:(NSString*)fileName bundle:(nullable NSBundle*)bundle{
    self.mainQuickData = [self loadMainQuickDataFromFileName:fileName bundle:bundle];
}
- (nullable NSMutableDictionary*)loadMainQuickDataFromFileName:(NSString*)fileName bundle:(nullable NSBundle*)bundle{
    if(nil == bundle){
        bundle = [NSBundle mainBundle];
    }
    NSString *filePath = [bundle pathForResource:fileName ofType:nil];
    if(nil == filePath){
        NSLog(@"[QuickLoader] 没有找到对应的MainQuickFile路径。（FileName:%@）",fileName);
        return nil;
    }
    NSData *data = [NSData dataWithContentsOfFile:filePath];
    NSError *error = nil;
    id jobj = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:&error];
    if(error){
        NSLog(@"[QuickLoader] 加载JSON数据失败。（fileName:%@ error:%@）",fileName, error);
        return nil;
    }
    if(![jobj isKindOfClass:NSDictionary.class]){
        NSLog(@"[QuickLoader] JSON数据并不是有效的NSDictionary格式。（fileName:%@ jobj:%@）",fileName, jobj);
        return nil;
    }
    return [jobj mutableCopy];
}

#pragma mark - ‘QuickComponent’的设置
- (void)setQuickComponentWithClass:(NSString*)component{
    Class cls = NSClassFromString(component);
    if(![cls instancesRespondToSelector:@selector(targetClass)]){
        NSLog(@"[%@] [installComponentClass] 无效Component。（Component:%@）", NSStringFromClass(self.class), component);
        return;
    }
    
    self.classToComponent[[cls targetClass]] = cls;
}

#pragma mark - 在初始化脚本中读取‘QuickComponent’和‘MainQuickFile’的数据
-(nullable NSDictionary*)loadQuickInitScriptFromPlist:(NSString*)fileName bundle:(nullable NSBundle*)bundle{
    if(nil == bundle){
        bundle = [NSBundle mainBundle];
    }
    NSString *path = [bundle pathForResource:fileName ofType:@"plist"];
    if(nil == path){
        return nil;
    }
    return [[NSDictionary alloc] initWithContentsOfFile:path];
}
-(nullable NSMutableDictionary*)loadQuickComponentFromInitScript:(NSDictionary*)script{
    id value = script[kQuickComponent];
    if(nil == value || ![value isKindOfClass:NSArray.class]){
        return nil;
    }
    
    NSMutableDictionary *componentDict = [NSMutableDictionary new];
    NSArray *componentClassArray = value;
    for (NSString *componentClass in componentClassArray) {
        Class cls = NSClassFromString(componentClass);
        if(![cls respondsToSelector:@selector(targetClass)]){
            NSLog(@"[%@] [init] 无效Component。（Component:%@）", NSStringFromClass(self.class), componentClass);
            continue;;
        }
        
        componentDict[[cls targetClass]] = cls;
    }
    return componentDict;
}
-(nullable NSMutableDictionary*)loadMainQuickDataFromInitScript:(NSDictionary*)script{
    id value = script[kMainQuickFile];
    if(nil==value){
        return nil;
    }
    if([value isKindOfClass:NSString.class]){
        return [self loadMainQuickDataFromFileName:value bundle:nil];
    }
    else if([value isKindOfClass:NSDictionary.class]){
        id nameValue = value[kName];
        id bundleValue = value[kBundle];
        if(nil==nameValue || ![nameValue isKindOfClass:NSString.class] || (nil!=bundleValue&&![bundleValue isKindOfClass:NSString.class])){
            return nil;
        }
        return [self loadMainQuickDataFromFileName:nameValue bundle:[NSBundle bundleWithIdentifier:bundleValue]];
    }
    else {
        return nil;
    }
}

#pragma mark - UIViewController的创建并进行Quick配置
-(UIViewController*)createViewController:(NSString*)cls{
    Class vcClass = NSClassFromString(cls);
    UIViewController *vc = [vcClass safeLoad_init];
    [self page:vc];
    return vc;
}
-(BOOL)page:(UIViewController*)obj{
    if(nil == obj.quick_name){
        return NO;
    }

    id data = self.mainQuickData[obj.quick_name];
    if(nil == data){
        return NO;
    }
    
    [self item:obj data:data];
    return YES;
}

#pragma mark - 通过对象使用QuickData进行配置
-(BOOL)item:(id)obj data:(id)data{
    NSString *cls = [obj quick_class];
    if(nil == cls){
        cls = NSStringFromClass([obj class]);
    }
    NSString *quickCls = [self findRelationshipClass:cls AtClasss:self.classToComponent.allKeys];
    if(nil == quickCls){
        NSLog(@"[QuickLoader] [quick] 没有对应的QuickClass。obj:%@ obj.class:%@ obj.quick_class:%@ data:%@",
              obj,
              NSStringFromClass([obj class]),
              [obj quick_class],
              data);
        return NO;
    }
    
    
    id<XXquickComponentDelegate> component = [QuickLoader defaultLoader].classToComponent[quickCls];
    if(nil == component){
        NSLog(@"[QuickLoader] [quick] 没有对应的Component。obj:%@ obj.class:%@ obj.quick_class:%@ data:%@",
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
            NSLog(@"[QuickLoader] [quick] 转换到JSON失败。obj:%@ data:%@ error:%@", obj, data, error);
            return NO;
        }
        
        if(![jobj isKindOfClass:NSDictionary.class]){
            NSLog(@"[QuickLoader] [quick] JOBJ并不是NSDictionary类型。");
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
- (nullable NSString*)findRelationshipClass:(NSString*)class AtClasss:(NSArray<NSString*>*)classStrings{
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

#pragma mark - 单例
+(QuickLoader*)defaultLoader{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instance = [QuickLoader new];
    });
    return _instance;
}

#pragma mark - 对应的静态函数
+ (UIViewController *)createViewController:(NSString *)cls{
    return [[QuickLoader defaultLoader] createViewController:cls];
}
+(void)setMainQuickDataWithFileName:(NSString*)fileName bundle:(nullable NSBundle*)bundle{
    [[QuickLoader defaultLoader] setMainQuickDataFromFileName:fileName bundle:bundle];
}
+(void)setQuickComponentWithClass:(NSString*)component{
    [[QuickLoader defaultLoader] setQuickComponentWithClass:component];
}
+(BOOL)page:(UIViewController*)obj{
    return [[QuickLoader defaultLoader] page:obj];
}
#pragma mark - 根据obj类型，寻找适合的‘QuickComponent’对其数据加载
+ (BOOL)item:(id)obj data:(nonnull id)data{
    return [[QuickLoader defaultLoader] item:obj data:data];
}
@end
