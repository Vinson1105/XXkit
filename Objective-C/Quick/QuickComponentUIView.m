//
//  QuickComponentUIView.m
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/18.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "QuickComponentUIView.h"
#import <UIKit/UIKit.h>
#import "../XXocUtils.h"

CONST_STR(BackgroundColor)
CONST_STR(BorderColor)
CONST_STR(BorderWidth)
CONST_STR(CornerRadius)

@implementation QuickComponentUIView
QUICK_PROPERTY(BackgroundColor, color)
QUICK_PROPERTY(BorderColor, color)
QUICK_PROPERTY(BorderWith, float)
QUICK_PROPERTY(CornerRadius, float)

+ (NSString *)targetClass{
    return NSStringFromClass(UIView.class);
}
+ (void)obj:(id)obj key:(NSString*)key value:(id)value{
    UIView *view = obj;
    
    // MARK: CornerRadius
    if([key isEqualToString:kCornerRadius]){
        view.layer.cornerRadius = [value floatValue];
    }
    
    // MARK: BorderWidth
    else if([key isEqualToString:kBorderWidth]){
        view.layer.borderWidth = [value floatValue];
    }
    
    // MARK: BorderColor
    else if([key isEqualToString:kBorderColor]){
        UIColor *color = [XXocUtils autoColor:value];
        if(color){
            view.layer.borderColor = [value CGColor];
        }
        else{
            [self unexecutedKey:key value:value];
        }
    }
    
    // MARK: BackgroundColor
    else if([key isEqualToString:kBackgroundColor]){
        UIColor *color = [XXocUtils autoColor:value];
        if(color){
            view.backgroundColor = color;
        }
        else{
            [self unexecutedKey:key value:value];
        }
    }
    else{
        NSString * string = kBackgroundColor;
        [super obj:obj key:key value:value];
    }
}
@end
