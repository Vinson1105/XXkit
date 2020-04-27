//
//  XXocUtility.m
//  XXkitProject
//
//  Created by VINSON on 2020/4/15.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "XXocUtils.h"

static NSDateFormatter *_dateFormatter;

@implementation XXocUtils
+(void)load{
    _dateFormatter = [NSDateFormatter new];
}

+ (UIViewController*)viewController:(NSString*)vc withUIStoryboard:(NSString*)storyboard bundle:(nullable NSBundle*)bundle{
    return [[UIStoryboard storyboardWithName:storyboard bundle:bundle] instantiateViewControllerWithIdentifier:vc];
}

#pragma mark - <NSLayoutConstraint>
+ (void)view:(UIView*)view size:(CGSize)size{
    [view.widthAnchor constraintEqualToConstant:size.width].active = YES;
    [view.heightAnchor constraintEqualToConstant:size.height].active = YES;
}
+ (void)view:(UIView*)view margin:(CGFloat)margin fillAt:(UIView*)fillAt{
    [view.topAnchor constraintEqualToAnchor:fillAt.topAnchor constant:margin].active = YES;
    [view.leadingAnchor constraintEqualToAnchor:fillAt.leadingAnchor constant:margin].active = YES;
    [view.bottomAnchor constraintEqualToAnchor:fillAt.bottomAnchor constant:-margin].active = YES;
    [view.trailingAnchor constraintEqualToAnchor:fillAt.trailingAnchor constant:-margin].active = YES;
}
+ (void)view:(UIView*)view centerAt:(UIView*)centerAt{
    [view.centerXAnchor constraintEqualToAnchor:centerAt.centerXAnchor].active = YES;
    [view.centerYAnchor constraintEqualToAnchor:centerAt.centerYAnchor].active = YES;
}
+ (void)view:(UIView*)view left:(CGFloat)left centerYAt:(UIView*)centerYAt{
    [view.leadingAnchor constraintEqualToAnchor:centerYAt.leadingAnchor constant:left].active = YES;
    [view.centerYAnchor constraintEqualToAnchor:centerYAt.centerYAnchor].active = YES;
}
+ (void)view:(UIView*)view right:(CGFloat)right centerYAt:(UIView*)centerYAt{
    [view.trailingAnchor constraintEqualToAnchor:centerYAt.trailingAnchor constant:right].active = YES;
    [view.centerYAnchor constraintEqualToAnchor:centerYAt.centerYAnchor].active = YES;
}

#pragma mark - <UIButton>
+ (void)button:(UIButton*)button norImg:(UIImage*)norImg selImg:(UIImage*)selImg{
    [button setImage:norImg forState:UIControlStateNormal];
    [button setImage:selImg forState:UIControlStateSelected];
}
+ (void)button:(UIButton*)button norImg:(UIImage*)norImg selImg:(UIImage*)selImg disNorImg:(UIImage*)disNorImg disSelImg:(UIImage*)disSelImg{
    [button setImage:norImg forState:UIControlStateNormal];
    [button setImage:selImg forState:UIControlStateSelected];
    [button setImage:disNorImg forState:UIControlStateNormal|UIControlStateDisabled];
    [button setImage:disSelImg forState:UIControlStateSelected|UIControlStateDisabled];
}
+ (void)button:(UIButton*)button norTxt:(NSString*)norTxt selTxt:(NSString*)selTxt{
    [button setTitle:norTxt forState:UIControlStateNormal];
    [button setTitle:selTxt forState:UIControlStateSelected];
}
+ (void)button:(UIButton*)button norTxt:(NSString*)norTxt selTxt:(NSString*)selTxt disNorTxt:(NSString*)disNorTxt disSelTxt:(NSString*)disSelTxt{
    [button setTitle:norTxt forState:UIControlStateNormal];
    [button setTitle:selTxt forState:UIControlStateSelected];
    [button setTitle:disNorTxt forState:UIControlStateNormal|UIControlStateDisabled];
    [button setTitle:disSelTxt forState:UIControlStateSelected|UIControlStateDisabled];
}

#pragma mark - <UIColor>
+ (unsigned int)intFromHexString:(NSString *)hexStr{
    unsigned int hexInt = 0;
    // Create scanner
    NSScanner *scanner = [NSScanner scannerWithString:hexStr];
    // Tell scanner to skip the # character
    [scanner setCharactersToBeSkipped:[NSCharacterSet characterSetWithCharactersInString:@"#"]];
    // Scan hex value
    [scanner scanHexInt:&hexInt];
    return hexInt;
}
+ (UIColor*)colorFromHexString:(NSString*)hexString{
    unsigned int hexint = [XXocUtils intFromHexString:hexString];
    UIColor *color =[UIColor colorWithRed:((CGFloat) ((hexint & 0xFF0000) >> 16))/255
                                    green:((CGFloat) ((hexint & 0xFF00) >> 8))/255
                                     blue:((CGFloat) (hexint & 0xFF))/255
                                    alpha:1];
    return color;
}
+ (UIColor*)colorFromHexString:(NSString*)hexString alpha:(CGFloat)alpha{
    unsigned int hexint = [XXocUtils intFromHexString:hexString];
    UIColor *color =[UIColor colorWithRed:((CGFloat) ((hexint & 0xFF0000) >> 16))/255
                                    green:((CGFloat) ((hexint & 0xFF00) >> 8))/255
                                     blue:((CGFloat) (hexint & 0xFF))/255
                                    alpha:alpha];
    return color;
}

#pragma mark - <UIAlertController>
+ (UIAlertController*)alertWithTitle:(NSString*)title msg:(NSString*)msg okTitle:(NSString*)okTitle onOK:(void (^)(UIAlertAction *action))onOK{
    UIAlertController* alert = [UIAlertController alertControllerWithTitle:title
                                   message:msg
                                   preferredStyle:UIAlertControllerStyleAlert];
     
    UIAlertAction* okAction = [UIAlertAction actionWithTitle:okTitle style:UIAlertActionStyleDefault handler:onOK];
    [alert addAction:okAction];
    return alert;
}
+ (UIAlertController*)alertWithTitle:(NSString*)title
                                 msg:(NSString*)msg
                             okTitle:(NSString*)okTitle
                                onOK:(void (^)(UIAlertAction *action))onOK
                         cancelTitle:(NSString*)cancelTitle
                            onCancel:(void (^)(UIAlertAction *action))onCancel{
    UIAlertController* alert = [UIAlertController alertControllerWithTitle:title
                                                                   message:msg
                                                            preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction* cancelAction = [UIAlertAction actionWithTitle:cancelTitle style:UIAlertActionStyleCancel handler:onCancel];
    UIAlertAction* okAction     = [UIAlertAction actionWithTitle:okTitle style:UIAlertActionStyleDefault handler:onOK];
    [alert addAction:cancelAction];
    [alert addAction:okAction];
    return alert;
}

#pragma mark - <JSON>
+ (NSString*)jsonStringWithJson:(id)json pretty:(BOOL)pretty{
    if(nil == json) return nil;
    NSError *error;
    NSJSONWritingOptions option;
    if (@available(iOS 11.0, *)) {
        option = pretty?NSJSONWritingPrettyPrinted:NSJSONWritingSortedKeys;
    } else {
        option = pretty?NSJSONWritingPrettyPrinted:kNilOptions;
    }
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:json options:option error:&error];
    if (!jsonData) {
        return nil;
    }
    else{
        return [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
    }
}
+ (nullable id)jsonWithJsonString:(NSString*)jsonString{
    if (jsonString.length == 0) return nil;

    NSData *jsonData = [jsonString dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;
    id obj = [NSJSONSerialization JSONObjectWithData:jsonData
                                                        options:NSJSONReadingMutableContainers
                                                          error:&error];
    return nil != error ? nil : obj;
}

#pragma mark - <Date>
+ (void)setDefaultDateFormatter:(NSDateFormatter*)formatter{
    _dateFormatter = formatter;
}
+ (NSString*)currentDateString{
    return [_dateFormatter stringFromDate:[NSDate date]];
}
+ (NSString*)currentDateStringWithDateFormat:(NSString*)format timeZone:(NSTimeZone*)timeZone{
    NSDateFormatter *formatter = [NSDateFormatter new];
    formatter.dateFormat = format;
    formatter.timeZone = timeZone;
    return [formatter stringFromDate:[NSDate date]];
}
+ (NSString*)dateStringWithTimestamp:(NSTimeInterval)timestamp{
    return [_dateFormatter stringFromDate:[NSDate dateWithTimeIntervalSince1970:timestamp]];
}
+ (NSString*)dateStringWithTimestamp:(NSTimeInterval)timestamp dateFormat:(NSString*)format timeZone:(NSTimeZone*)timeZone{
    NSDateFormatter *formatter = [NSDateFormatter new];
    formatter.dateFormat = format;
    formatter.timeZone = timeZone;
    return [formatter stringFromDate:[NSDate dateWithTimeIntervalSince1970:timestamp]];
}

#pragma mark - <File System>
+ (NSString*)documentAbsolutePath{
    return [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
}
+ (NSString*)fileAbsolutePathInDocument:(NSArray*)nodes{
    NSString *filePath  = [self documentAbsolutePath];
    
    NSEnumerator *enumer    = nodes.objectEnumerator;
    NSString *node          = nil;
    while (nil != (node = enumer.nextObject)) {
        filePath = [filePath stringByAppendingPathComponent:node];
    }
    return filePath;
}
@end
