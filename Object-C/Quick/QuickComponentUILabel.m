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
    if([key isEqualToString:kText]){
        // MARK: Text
        label.text = value;
    }
    else if([key isEqualToString:kTextColor]){
        // MARK: TextColor
        UIColor *color = [XXocUtils autoColor:value];
        label.textColor = color;
    }
    else if([key isEqualToString:kFont]){
        // MARK: Font
        if([value isKindOfClass:NSNumber.class]){
            label.font = [UIFont systemFontOfSize:[value floatValue]];
        }
        else{
            NSLog(@"[%@] [quick] 未处理键值。key:%@ value:%@", NSStringFromClass(self.class), key, value);
        }
    }
    else if([key isEqualToString:kTextAlignment]){
        // MARK: TextAlignment
        if([value isKindOfClass:NSString.class]){
            NSLog(@"[%@] [quick] 未处理键值。key:%@ value:%@", NSStringFromClass(self.class), key, value);
        }
        else{
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
                NSLog(@"[%@] [quick] 未处理键值。key:%@ value:%@", NSStringFromClass(self.class), key, value);
            }
        }
    }
    else if([key isEqualToString:kNumberOfLines]){
        // MARK: NumberOfLines
        if([value isKindOfClass:NSNumber.class]){
            label.numberOfLines = [value intValue];
        }
        else{
            NSLog(@"[%@] [quick] 未处理键值。key:%@ value:%@", NSStringFromClass(self.class), key, value);
        }
    }
    else{
        NSLog(@"[%@] [quick] 未处理键值。key:%@ value:%@", NSStringFromClass(self.class), key, value);
    }
}
@end
