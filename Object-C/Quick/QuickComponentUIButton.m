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
static NSString * const kImage = @"Image";

static NSString * const kNormal = @"Normal";
static NSString * const kSelected = @"Selected";
static NSString * const kDisabled = @"Disabled";
static NSString * const kNormalAndDisabled = @"NormalDisabled";
static NSString * const kSelectedAndDisabled = @"SelectedDisabled";


@implementation QuickComponentUIButton
+ (NSString *)targetClass{
    return NSStringFromClass(UIButton.class);
}
+ (void)obj:(id)obj key:(NSString*)key value:(id)value{
    UIButton *button = obj;
    
    // MARK: Title
    if([key isEqualToString:kTitle]){
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
            if(value[kDisabled]){
                [button setTitle:value[kDisabled] forState:UIControlStateDisabled];
            }
            if(value[kNormalAndDisabled]){
                [button setTitle:value[kNormalAndDisabled] forState:UIControlStateNormal|UIControlStateDisabled];
            }
            if(value[kSelectedAndDisabled]){
                [button setTitle:value[kSelectedAndDisabled] forState:UIControlStateSelected|UIControlStateDisabled];
            }
        }
        else{
            
        }
    }
    
    // MARK: Image
    else if([key isEqualToString:kImage]){
        if([value isKindOfClass:NSString.class]){
            [button setImage:[UIImage imageNamed:value] forState:UIControlStateNormal];
        }
        else if([value isKindOfClass:UIImage.class]){
            [button setImage:value forState:UIControlStateNormal];
        }
        else if([value isKindOfClass:NSDictionary.class]){
            if(value[kNormal]){
                if([value[kNormal] isKindOfClass:NSString.class]){
                    [button setImage:[UIImage imageNamed:value] forState:UIControlStateNormal];
                }
                else if([value[kNormal] isKindOfClass:UIImage.class]){
                    [button setImage:value forState:UIControlStateNormal];
                }
                else{
                    [self unexecutedKey:key value:value];
                }
            }
            if(value[kSelected]){
                if([value[kSelected] isKindOfClass:NSString.class]){
                    [button setImage:[UIImage imageNamed:value] forState:UIControlStateSelected];
                }
                else if([value[kSelected] isKindOfClass:UIImage.class]){
                    [button setImage:value forState:UIControlStateSelected];
                }
                else{
                    [self unexecutedKey:key value:value];
                }
            }
            if(value[kDisabled]){
                if([value[kDisabled] isKindOfClass:NSString.class]){
                    [button setImage:[UIImage imageNamed:value] forState:UIControlStateDisabled];
                }
                else if([value[kDisabled] isKindOfClass:UIImage.class]){
                    [button setImage:value forState:UIControlStateDisabled];
                }
                else{
                    [self unexecutedKey:key value:value];
                }
            }
            if(value[kNormalAndDisabled]){
                if([value[kNormalAndDisabled] isKindOfClass:NSString.class]){
                    [button setImage:[UIImage imageNamed:value] forState:UIControlStateNormal|UIControlStateDisabled];
                }
                else if([value[kNormalAndDisabled] isKindOfClass:UIImage.class]){
                    [button setImage:value forState:UIControlStateNormal|UIControlStateDisabled];
                }
                else{
                    [self unexecutedKey:key value:value];
                }
            }
            if(value[kSelectedAndDisabled]){
                if([value[kSelectedAndDisabled] isKindOfClass:NSString.class]){
                    [button setImage:[UIImage imageNamed:value] forState:UIControlStateSelected|UIControlStateDisabled];
                }
                else if([value[kSelectedAndDisabled] isKindOfClass:UIImage.class]){
                    [button setImage:value forState:UIControlStateSelected|UIControlStateDisabled];
                }
                else{
                    
                }
            }
        }
        else {
            [self unexecutedKey:key value:value];
        }
    }
    else{
        [super obj:obj key:key value:value];
    }
    
}
@end
