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

static NSString * const kTitle = @"Title";

static NSString * const kNavigationBar = @"NavigationBar";
static NSString * const kNavigationbarHidden = @"NavigationBarHidden";
static NSString * const kNavigationItem = @"NavigationItem";
static NSString * const kView = @"View";
static NSString * const kSubView = @"SubView";
static NSString * const kBottomBarHidden = @"BottomBarHidden";

@implementation QuickComponentUIViewController
+ (NSString *)targetClass{
    return NSStringFromClass(UIViewController.class);
}
+ (void)obj:(id)obj key:(NSString *)key value:(id)value{
    UIViewController *viewController = obj;
    
    // MARK: Title
    if([key isEqualToString:kTitle]){
        if([value isKindOfClass:NSString.class]){
            viewController.title = value;
        }
        else{
            [self unexecutedKey:key value:value];
        }
    }
    
    // MARK: NavigationBar
    else if([key isEqualToString:kNavigationBar]){
        if([value isKindOfClass:NSDictionary.class]){
            [viewController.navigationController.navigationBar quick_reset:value];
        }
        else{
            [self unexecutedKey:key value:value];
        }
    }
    
    // MARK: NavigationBarHidden
    else if([key isEqualToString:kNavigationbarHidden]){
        if([value isKindOfClass:NSNumber.class]){
            viewController.navigationController.navigationBarHidden = [value boolValue];
        }
        else{
            [self unexecutedKey:key value:value];
        }
    }
        
    // MARK: NavigationItem
    else if([key isEqualToString:kNavigationItem]){
        if([value isKindOfClass:NSDictionary.class]){
            [viewController.navigationItem quick_reset:value];
        }
        else{
            [self unexecutedKey:key value:value];
        }
    }
    
    // MARK: View
    else if([key isEqualToString:kView]){
        [viewController.view quick_reset:value];
    }
    
    // MARK: SubView
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
    
    // MARK: BottomBarHidden
    else if([key isEqualToString:kBottomBarHidden]){
        if([value isKindOfClass:NSNumber.class]){
            viewController.hidesBottomBarWhenPushed = [value boolValue];
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
