//
//  QuickComponentUIViewController.m
//  XXkitProject
//
//  Created by VINSON on 2020/10/21.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "QuickComponentUIViewController.h"
#import <UIKit/UIKit.h>
#import "NSObject+Quick.h"

static NSString * const kView = @"View";
static NSString * const kSubView = @"SubView";

@implementation QuickComponentUIViewController
+ (NSString *)targetClass{
    return NSStringFromClass(UIViewController.class);
}
+ (void)obj:(id)obj key:(NSString *)key value:(id)value{
    UIViewController *viewController = obj;
    
    if([key isEqualToString:kView]){
        [viewController.view quick_reset:value];
    }
    else if([key isEqualToString:kSubView]){
        if([value isKindOfClass:NSDictionary.class]){
            NSDictionary *nameToData = value;
            NSDictionary *nameToView = [self getSubviewLoopFromView:viewController.view];
            
            NSEnumerator *nameEnumer = nameToData.keyEnumerator;
            NSString *name = nil;
            while (nil != (name = nameEnumer.nextObject)) {
                id data = nameToData[name];
                
                UIView *view = nameToView[name];
                if(nil == view){
                    continue;
                }
                
                [view quick_reset:data];
            }
        }
        else{
            [self unexecutedKey:key value:value];
        }
    }
    else{
        [super obj:obj key:key value:value];
    }
}

+(NSDictionary *)getSubviewLoopFromView:(UIView*)view{
    NSMutableDictionary *nameToView = [NSMutableDictionary new];
    [self addViews:view.subviews toNameToObj:nameToView];
    return nameToView;
}
+(void)addViews:(NSArray<UIView*>*)views toNameToObj:(NSMutableDictionary*)nameToObj{
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
