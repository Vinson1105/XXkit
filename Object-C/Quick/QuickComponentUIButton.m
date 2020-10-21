//
//  QuickComponentUIButton.m
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/18.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "QuickComponentUIButton.h"
#import <UIKit/UIKit.h>
#import "../XXocUtils.h"

static NSString * const kTitle = @"Title";
static NSString * const kTitleColor = @"TitleColor";
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
            [self dictionary:value containsBlock:^(id valueForKey, UIControlState state) {
                [button setTitle:valueForKey forState:state];
            }];
        }
        else{
            
        }
    }
    
    // MARK: TitleColor
    else if([key isEqualToString:kTitleColor]){
        if([value isKindOfClass:NSString.class] || [value isKindOfClass:UIColor.class]){
            UIColor * color = [XXocUtils autoColor:value];
            if(color){
                [button setTitleColor:color forState:UIControlStateNormal];
            }
        }
        else if([value isKindOfClass:NSDictionary.class]){
            [self dictionary:value containsBlock:^(id valueForKey, UIControlState state) {
                [button setTitleColor:[XXocUtils autoColor:valueForKey] forState:state];
            }];
        }
        else {
            [self unexecutedKey:key value:value];
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
            [self dictionary:value containsBlock:^(id valueForKey, UIControlState state) {
                if([valueForKey isKindOfClass:NSString.class]){
                    [button setImage:[UIImage imageNamed:valueForKey] forState:UIControlStateNormal];
                }
                else if([valueForKey isKindOfClass:UIImage.class]){
                    [button setImage:valueForKey forState:UIControlStateNormal];
                }
                else{
                    [self unexecutedKey:key value:value];
                }
            }];            
        }
        else {
            [self unexecutedKey:key value:value];
        }
    }
    else{
        [super obj:obj key:key value:value];
    }
}
+(void)dictionary:(NSDictionary*)dict containsBlock:(void(^)(id valueForKey, UIControlState state))block{
    
    NSEnumerator *keyEnumer = dict.keyEnumerator;
    NSString *key = nil;
    while (nil != (key = keyEnumer.nextObject)) {
        if([key isEqualToString:kNormal]){
            block(dict[key], UIControlStateNormal);
        }
        else if([key isEqualToString:kSelected]){
            block(dict[key], UIControlStateSelected);
        }
        else if([key isEqualToString:kNormalAndDisabled]){
            block(dict[key], UIControlStateNormal|UIControlStateDisabled);
        }
        else if([key isEqualToString:kSelectedAndDisabled]){
            block(dict[key], UIControlStateSelected|UIControlStateDisabled);
        }
        else{
            
        }
    }
}
@end
