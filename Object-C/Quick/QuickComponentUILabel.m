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
static NSString * const kColor = @"Color";
static NSString * const kFont = @"Font";

@implementation QuickComponentUILabel
- (BOOL)quick:(id)obj kvdata:(NSDictionary *)kvdata{
    UILabel *label = obj;
    
    NSEnumerator *keyEnumer = kvdata.keyEnumerator;
    NSString *key = nil;
    while (nil != (key = keyEnumer.nextObject)) {
        id value = kvdata[key];
        
        if([key isEqualToString:kText]){
            label.text = value;
        }
        else if([key isEqualToString:kColor]){
            if([value isKindOfClass:UIColor.class]){
                label.textColor = value;
            }
            else if([value isKindOfClass:NSString.class]){
                UIColor *color = [XXocUtils colorFromHexString:value];
                label.textColor = color;
            }
            else{
                
            }
        }
        else if([key isEqualToString:kFont]){
            if([value isKindOfClass:NSNumber.class]){
                label.font = [UIFont systemFontOfSize:[value floatValue]];
            }
            else{
                
            }
        }
        else{
            NSLog(@"[QuickComponentUILabel] [quick] 未处理键值。key:%@ value:%@",key, value);
        }
    }
    return YES;
}
@end
