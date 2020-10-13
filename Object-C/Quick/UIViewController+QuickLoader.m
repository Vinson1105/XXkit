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
@property (nonatomic,strong,readonly) NSMutableDictionary *nameToView;
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
        
        UIView *view = self.nameToView[key];
        if(nil == view){
            continue;
        }
        
        [view quick_reset:value];
    }
}

- (NSMutableDictionary *)nameToView{
    id value = objc_getAssociatedObject(self, "QuickLoader_nameToView");
    if(nil == value){
        NSMutableDictionary *dict = [NSMutableDictionary new];
        
        NSArray *subviews = self.view.subviews;
        for (UIView *view in subviews) {
            if(nil == view.quick_name){
                continue;
            }
            
            dict[view.quick_name] = view;
        }
        
        objc_setAssociatedObject(self, "QuickLoader_nameToView", dict, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return value;
}
@end
