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
- (NSString *)targetClass{
    return NSStringFromClass(UILabel.class);
}
- (BOOL)quick:(id)obj kvdata:(NSDictionary *)kvdata{
    UILabel *label = obj;
    
    NSEnumerator *keyEnumer = kvdata.keyEnumerator;
    NSString *key = nil;
    while (nil != (key = keyEnumer.nextObject)) {
        id value = kvdata[key];
        
        // MARK: Text
        if([key isEqualToString:kText]){
            label.text = value;
        }
        // MARK: TextColor
        else if([key isEqualToString:kTextColor]){
            if([value isKindOfClass:UIColor.class]){
                label.textColor = value;
            }
            else if([value isKindOfClass:NSString.class]){
                UIColor *color = [XXocUtils colorFromHexString:value];
                label.textColor = color;
            }
            else{
                NSLog(@"[QuickComponentUILabel] [quick] 未处理键值。key:%@ value:%@",key, value);
            }
        }
        // MARK: Font
        else if([key isEqualToString:kFont]){
            if([value isKindOfClass:NSNumber.class]){
                label.font = [UIFont systemFontOfSize:[value floatValue]];
            }
            else{
                NSLog(@"[QuickComponentUILabel] [quick] 未处理键值。key:%@ value:%@",key, value);
            }
        }
        // MARK: TextAlignment
        else if([key isEqualToString:kTextAlignment]){
            if([value isKindOfClass:NSString.class]){
                NSLog(@"[QuickComponentUILabel] [quick] 未处理键值。key:%@ value:%@",key, value);
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
                    NSLog(@"[QuickComponentUILabel] [quick] 未处理键值。key:%@ value:%@",key, value);
                }
            }
        }
        // MARK: NumberOfLines
        else if([key isEqualToString:kNumberOfLines]){
            if([value isKindOfClass:NSNumber.class]){
                label.numberOfLines = [value intValue];
            }
            else{
                NSLog(@"[QuickComponentUILabel] [quick] 未处理键值。key:%@ value:%@",key, value);
            }
        }
        else{
            NSLog(@"[QuickComponentUILabel] [quick] 未处理键值。key:%@ value:%@",key, value);
        }
    }
    return YES;
}
@end
