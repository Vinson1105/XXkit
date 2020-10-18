//
//  QuickComponentUIButton.m
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/18.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "QuickComponentUIButton.h"
#import <UIKit/UIKit.h>

static NSString * const kTitle = @"Title";

static NSString * const kNormal = @"Normal";
static NSString * const kSelected = @"Selected";
static NSString * const kDisabled = @"Disabled";
static NSString * const kNormalAndDisabled = "NormalDisabled";
static NSString * const kSelectedAndDisabled = "SelectedDisabled";


@implementation QuickComponentUIButton
+ (NSString *)targetClass{
    return NSStringFromClass(UIButton.class);
}
+ (void)obj:(id)obj key:(NSString*)key value:(id)value{
    UIButton *button = obj;
    if([key isEqualToString:kTitle]){
        // MARK: Title
        if([value isKindOfClass:NSString.class]){
            [button setTitle:value forState:UIControlStateNormal];
        }
        else if([value isKindOfClass:NSDictionary.class]){
            if(value[kNormal]){
                [button setTitle:value[kNormal] forState:UIControlStateNormal];
            }
            if(value[kSelected]){
                [button setTitle:value[kSelected] forState:UIControlStateSelected];
            }
        }
        else{
            
        }
    }
    
}
@end
