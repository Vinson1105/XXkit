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

static NSString * const kCornerRadius = @"CornerRadius";
static NSString * const kBorderWidth = @"BorderWidth";
static NSString * const kBorderColor = @"BorderColor";
static NSString * const kBackgroundColor = @"BackgroundColor";

@implementation QuickComponentUIView
+ (NSString *)targetClass{
    return NSStringFromClass(UIView.class);
}
+ (void)obj:(id)obj key:(NSString*)key value:(id)value{
    UIView *view = obj;
    if([key isEqualToString:kCornerRadius]){
        // MARK: CornerRadius
        view.layer.cornerRadius = [value floatValue];
    }
    else if([key isEqualToString:kBorderWidth]){
        // MARK: BorderWidth
        view.layer.borderWidth = [value floatValue];
    }
    else if([key isEqualToString:kBorderColor]){
        // MARK: BorderColor
        UIColor *color = [XXocUtils autoColor:value];
        if(color){
            view.layer.borderColor = [value CGColor];
        }
        else{
            [self unexecutedKey:key value:value];
        }
    }
    else if([key isEqualToString:kBackgroundColor]){
        // MARK: BackgroundColor
        UIColor *color = [XXocUtils autoColor:value];
        if(color){
            view.backgroundColor = color;
        }
        else{
            [self unexecutedKey:key value:value];
        }
    }
    else{
        [super obj:obj key:key value:value];
    }
}
@end
