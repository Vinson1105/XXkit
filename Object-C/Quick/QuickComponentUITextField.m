//
//  QuickComponentUITextField.m
//  XXkitProject
//
//  Created by VINSON on 2020/11/3.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "QuickComponentUITextField.h"
#import <UIKit/UIKit.h>
#import "../XXocUtils.h"

static NSString * const kFont = @"Font";
static NSString * const kTextColor = @"TextColor";
static NSString * const kText = @"Text";
static NSString * const kPlaceholder = @"Placeholder";
static NSString * const kLeftView = @"LeftView";

@implementation QuickComponentUITextField
+ (NSString *)targetClass{
    return NSStringFromClass(UITextField.class);
}
+ (void)obj:(id)obj key:(NSString *)key value:(id)value{
    UITextField *textField = obj;
    
    // MARK: Font
    if([key isEqualToString:kFont]){
        if([value isKindOfClass:NSNumber.class]){
            textField.font = [UIFont systemFontOfSize:[value floatValue]];
        }
        else{
            [self unexecutedKey:key value:value];
        }
    }
    
    // MARK: TextColor
    else if([key isEqualToString:kTextColor]){
        textField.textColor = [XXocUtils autoColor:value];
    }
    
    // MARK: Text
    else if([key isEqualToString:kText]){
        textField.text = value;
    }
    
    // MARK: Placeholder
    else if([key isEqualToString:kPlaceholder]){
        if([value isKindOfClass:NSDictionary.class]){
            NSDictionary *dict = value;
            NSMutableAttributedString *string = nil;
            if(dict[kText]){
                string = [[NSMutableAttributedString alloc] initWithString:dict[kText]];
            }
            if (string&&dict[kTextColor]) {
                [XXocUtils attributeString:string color:dict[kTextColor] matchString:nil];
            }
            if(string&&dict[kFont]){
                [XXocUtils attributeString:string font:[UIFont systemFontOfSize:[dict[kFont] floatValue]] matchString:nil];
            }
            if(string){
                textField.attributedPlaceholder = string;
            }
        }
        else if([value isKindOfClass:NSString.class]){
            textField.placeholder = value;
        }
        else{
            [self unexecutedKey:key value:value];
        }
    }
    
    // MARK: LeftView
    else if([key isEqualToString:kLeftView]){
        if([value isKindOfClass:NSDictionary.class]){
            [self unexecutedKey:key value:value];
        }
        else if([value isKindOfClass:NSString.class] || [value isKindOfClass:NSArray.class]){
            UIImageView *imageView = [UIImageView new];
            imageView.translatesAutoresizingMaskIntoConstraints = NO;
            imageView.contentMode = UIViewContentModeCenter;
            imageView.image = [XXocUtils autoImage:value];
            [XXocUtils view:imageView size:CGSizeMake(30, 30)];
            textField.leftView = imageView;
            textField.leftViewMode = UITextFieldViewModeAlways;
        }
        else if([value isKindOfClass:UIView.class]){
            textField.leftView = value;
        }
        else{
            [self unexecutedKey:key value:value];
        }
    }
    else{
        
    }
}
@end
