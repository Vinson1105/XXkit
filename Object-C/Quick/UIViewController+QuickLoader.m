//
//  UIViewController+QuickLoader.m
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/13.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "UIViewController+QuickLoader.h"
#import "NSObject+Quick.h"
#import <objc/runtime.h>

@interface UIViewController()
@property (nonatomic,strong,readonly) NSMutableDictionary *nameToObj;
@end

@implementation UIViewController(QuickLoader)
- (void)quick_loadFromFileName:(NSString *)fileName bundle:(nullable NSBundle *)bundle{
    if(nil == bundle){
        bundle = [NSBundle mainBundle];
    }
    
    NSString *path = [bundle pathForResource:fileName ofType:nil];
    if(nil == path){
        NSLog(@"[UIViewController+QuickLoader] [quick_loadFromFileName] 没有找到目标文件。self:%@ fileName:%@", self, fileName);
        return;
    }
    
    NSData *data = [NSData dataWithContentsOfFile:path];
    [self quick_loadFromData:data];
}
- (void)quick_loadFromData:(NSData *)data{
    NSError *error = nil;
    id jobj = [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:&error];
    if(error){
        NSLog(@"[UIViewController+QuickLoader] [quick_loadFromData] 转换到JSON失败。self:%@ error:%@",self, error);
        return;
    }
    
    if(![jobj isKindOfClass:NSDictionary.class]){
        NSLog(@"[UIViewController+QuickLoader] [quick_loadFromData] JSON类型并不是字典模式。self:%@ jobj:%@", self, jobj);
        return;;
    }
    
    NSDictionary *jdict = jobj;
    NSEnumerator *keyEnumer = jdict.keyEnumerator;
    NSString *key = nil;
    while (nil != (key = keyEnumer.nextObject)) {
        id value = jdict[key];
        
        UIView *view = self.nameToObj[key];
        if(nil == view){
            continue;
        }
        
        [view quick_reset:value];
    }
}

- (NSMutableDictionary *)nameToObj{
    id value = objc_getAssociatedObject(self, "QuickLoader_nameToObj");
    if(nil == value){
        NSMutableDictionary *dict = [NSMutableDictionary new];
        [self addViews:self.view.subviews toNameToObj:dict];
        value = dict;
        objc_setAssociatedObject(self, "QuickLoader_nameToObj", value, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return value;
}
-(void)addViews:(NSArray<UIView*>*)views toNameToObj:(NSMutableDictionary*)nameToObj{
    if(nil == views || 0 == views.count){
        return;
    }
    for (UIView *view in views) {
        [self addViews:view.subviews toNameToObj:nameToObj];
        
        if(nil == view.quick_name){
            continue;
        }
        nameToObj[view.quick_name] = view;
    }
}
@end
