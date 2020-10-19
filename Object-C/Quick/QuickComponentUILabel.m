//
//  QuickComponentUILabel.m
//  XXkitProject
//
//  Created by 郭文轩 on 2020/10/13.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "QuickComponentUILabel.h"
#import <UIKit/UIKit.h>
#import "../XXocUtils.h"

static NSString * const kText = @"Text";
static NSString * const kTextColor = @"TextColor";
static NSString * const kFont = @"Font";
static NSString * const kTextAlignment = @"TextAlignment";
    static NSString * const kTextAlignmentCenter = @"Center";
    static NSString * const kTextAlignmentLeft = @"Left";
    static NSString * const kTextAlignmentRight = @"Right";

static NSString * const kNumberOfLines = @"NumberOfLines";

@implementation QuickComponentUILabel
+ (NSString *)targetClass{
    return NSStringFromClass(UILabel.class);
}

+ (void)obj:(id)obj key:(NSString *)key value:(id)value{
    UILabel *label = obj;
    // MARK: Text
    if([key isEqualToString:kText]){
        label.text = value;
    }
    
    // MARK: TextColor
    else if([key isEqualToString:kTextColor]){
        UIColor *color = [XXocUtils autoColor:value];
        label.textColor = color;
    }
    
    // MARK: Font
    else if([key isEqualToString:kFont]){
        if([value isKindOfClass:NSNumber.class]){
            label.font = [UIFont systemFontOfSize:[value floatValue]];
        }
        else{
            [self unexecutedKey:key value:value];
        }
    }
    
    // MARK: TextAlignment
    else if([key isEqualToString:kTextAlignment]){
        if([value isKindOfClass:NSString.class]){
            if ([value isEqualToString:kTextAlignmentCenter]) {
                label.textAlignment = NSTextAlignmentCenter;
            }
            else if([value isEqualToString:kTextAlignmentLeft]){
                label.textAlignment = NSTextAlignmentLeft;
            }
            else if([value isEqualToString:kTextAlignmentRight]){
                label.textAlignment = NSTextAlignmentRight;
            }
            else {
                [self unexecutedKey:key value:value];
            }
        }
        else{
            [self unexecutedKey:key value:value];
        }
    }
    
    // MARK: NumberOfLines
    else if([key isEqualToString:kNumberOfLines]){
        if([value isKindOfClass:NSNumber.class]){
            label.numberOfLines = [value intValue];
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
