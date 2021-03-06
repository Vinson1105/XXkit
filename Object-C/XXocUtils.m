//
//  XXocUtility.m
//  XXkitProject
//
//  Created by VINSON on 2020/4/15.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "XXocUtils.h"
#import <LocalAuthentication/LocalAuthentication.h>
#import <objc/runtime.h>
#import <AVFoundation/AVFoundation.h>

static NSDateFormatter *_dateFormatter;

@implementation XXocUtils
+(void)load{
    _dateFormatter = [NSDateFormatter new];
    _dateFormatter.dateFormat = @"yyyy-MM-dd HH:mm:ss.SSS";
}


#pragma mark - <UIViewController>
+ (UIViewController*)viewController:(NSString*)storyboardID{
    return [[UIStoryboard storyboardWithName:@"Main" bundle:nil] instantiateViewControllerWithIdentifier:storyboardID];
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

+ (void)view:(UIView*)view containsLeft:(CGFloat)left centerYAt:(UIView*)centerYAt{
    [view.leadingAnchor constraintEqualToAnchor:centerYAt.leadingAnchor constant:left].active = YES;
    [view.centerYAnchor constraintEqualToAnchor:centerYAt.centerYAnchor].active = YES;
}
+ (void)view:(UIView*)view containsRight:(CGFloat)right centerYAt:(UIView*)centerYAt{
    [view.trailingAnchor constraintEqualToAnchor:centerYAt.trailingAnchor constant:right].active = YES;
    [view.centerYAnchor constraintEqualToAnchor:centerYAt.centerYAnchor].active = YES;
}
+ (void)view:(UIView*)view containsTop:(CGFloat)top centerXAt:(UIView*)centerXAt{
    [view.topAnchor constraintEqualToAnchor:centerXAt.topAnchor constant:top].active = YES;
    [view.centerXAnchor constraintEqualToAnchor:centerXAt.centerXAnchor].active = YES;
}
+ (void)view:(UIView*)view containsBottom:(CGFloat)bottom centerXAt:(UIView*)centerXAt{
    [view.bottomAnchor constraintEqualToAnchor:centerXAt.bottomAnchor constant:bottom].active = YES;
    [view.centerXAnchor constraintEqualToAnchor:centerXAt.centerXAnchor].active = YES;
}

+ (void)view:(UIView*)view appendLeft:(CGFloat)left centerYAt:(UIView*)centerYAt{
    [view.leadingAnchor constraintEqualToAnchor:centerYAt.trailingAnchor constant:left].active = YES;
    [view.centerYAnchor constraintEqualToAnchor:centerYAt.centerYAnchor].active = YES;
}
+ (void)view:(UIView*)view appendRight:(CGFloat)right centerYAt:(UIView*)centerYAt{
    [view.trailingAnchor constraintEqualToAnchor:centerYAt.leadingAnchor constant:right].active = YES;
    [view.centerYAnchor constraintEqualToAnchor:centerYAt.centerYAnchor].active = YES;
}
+ (void)view:(UIView*)view appendTop:(CGFloat)top centerXAt:(UIView*)centerXAt{
    [view.topAnchor constraintEqualToAnchor:centerXAt.bottomAnchor constant:top].active = YES;
    [view.centerXAnchor constraintEqualToAnchor:centerXAt.centerXAnchor].active = YES;
}
+ (void)view:(UIView*)view appendbottom:(CGFloat)bottom centerXAt:(UIView*)centerXAt{
    [view.bottomAnchor constraintEqualToAnchor:centerXAt.topAnchor constant:bottom].active = YES;
    [view.centerXAnchor constraintEqualToAnchor:centerXAt.centerXAnchor].active = YES;
}

+ (void)view:(UIView*)view adjustFillAtScrollView:(UIScrollView*)scrollView{
    if(@available(iOS 11.0, *)){
        [view.leadingAnchor constraintEqualToAnchor:scrollView.frameLayoutGuide.leadingAnchor].active = YES;
        [view.trailingAnchor constraintEqualToAnchor:scrollView.frameLayoutGuide.trailingAnchor].active = YES;
        [view.centerXAnchor constraintEqualToAnchor:scrollView.frameLayoutGuide.centerXAnchor].active = YES;
        
        [view.topAnchor constraintEqualToAnchor:scrollView.contentLayoutGuide.topAnchor].active = YES;
        [view.trailingAnchor constraintEqualToAnchor:scrollView.contentLayoutGuide.trailingAnchor].active = YES;
        [view.leadingAnchor constraintEqualToAnchor:scrollView.contentLayoutGuide.leadingAnchor].active = YES;
        [view.bottomAnchor constraintEqualToAnchor:scrollView.contentLayoutGuide.bottomAnchor].active = YES;
        
        NSLayoutConstraint *layout = [view.heightAnchor constraintEqualToAnchor:scrollView.frameLayoutGuide.heightAnchor];
        layout.priority = 250;
        layout.active = YES;
    }
}

#pragma mark - <UIButton>
+ (void)button:(UIButton*)button norImg:(UIImage*)norImg selImg:(UIImage*)selImg{
    [button setImage:norImg forState:UIControlStateNormal];
    [button setImage:selImg forState:UIControlStateSelected];
}
+ (void)button:(UIButton*)button norImg:(UIImage*)norImg disImg:(UIImage*)disImg{
    [button setImage:norImg forState:UIControlStateNormal];
    [button setImage:disImg forState:UIControlStateDisabled];
}
+ (void)button:(UIButton*)button norImg:(UIImage*)norImg selImg:(UIImage*)selImg disImg:(UIImage*)disImg{
    [button setImage:norImg forState:UIControlStateNormal];
    [button setImage:selImg forState:UIControlStateSelected];
    [button setImage:disImg forState:UIControlStateDisabled];
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
+ (void)button:(UIButton*)button norTxt:(NSString*)norTxt disTxt:(NSString*)disTxt{
    [button setTitle:norTxt forState:UIControlStateNormal];
    [button setTitle:disTxt forState:UIControlStateDisabled];
}
+ (void)button:(UIButton*)button norTxt:(NSString*)norTxt selTxt:(NSString*)selTxt disTxt:(NSString*)disTxt{
    [button setTitle:norTxt forState:UIControlStateNormal];
    [button setTitle:selTxt forState:UIControlStateSelected];
    [button setTitle:disTxt forState:UIControlStateDisabled];
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
+ (UIColor *)autoColor:(id)obj{
    if([obj isKindOfClass:NSString.class]){
        if(NSOrderedSame == [obj compare:@"transparent" options:NSCaseInsensitiveSearch]){
            return UIColor.clearColor;
        }
        else{
            return [self colorFromHexString:obj];
        }
    }
    else if([obj isKindOfClass:UIColor.class]){
        return obj;
    }
    else if([obj isKindOfClass:NSArray.class]&&[obj count]>0){
        // 0:非暗黑模式下 1:暗黑模式下
        UIColor *color = nil;
        if (@available(iOS 13.0, *)) {
            color = [UIColor colorWithDynamicProvider:^UIColor * _Nonnull(UITraitCollection * _Nonnull trait) {
                if (trait.userInterfaceStyle == UIUserInterfaceStyleDark && [obj count]>1) {
                    return [XXocUtils autoColor:obj[1]];
                } else {
                    return [XXocUtils autoColor:obj[0]];
                }
            }];
        } else {
            color = [XXocUtils autoColor:obj[0]];
        }
        return color;
    }
    else{
        return nil;
    }
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
+ (UIColor *)colorFromLightHex:(NSString *)lightHex darkHex:(NSString *)darkHex{
    UIColor *color = nil;
    if(@available(iOS 13.0,*)){
        color = [UIColor colorWithDynamicProvider:^UIColor *_Nonnull(UITraitCollection *_Nonnull trait){
            if(trait.userInterfaceStyle == UIUserInterfaceStyleDark){
                return [XXocUtils colorFromHexString:darkHex];
            }
            else{
                return [XXocUtils colorFromHexString:lightHex];
            }
        }];
    }
    else{
        color = [XXocUtils colorFromHexString:lightHex];
    }
    return color;
}

#pragma mark - <UIAlertController>
+ (UIAlertController*)alertWithTitle:(NSString*)title
                                 msg:(NSString*)msg
                             okTitle:(nullable NSString*)okTitle
                                onOK:(nullable void (^)(UIAlertAction *action))onOK
                         cancelTitle:(nullable NSString*)cancelTitle
                            onCancel:(nullable void (^)(UIAlertAction *action))onCancel{
    UIAlertController* alert = [UIAlertController alertControllerWithTitle:title
                                                                   message:msg
                                                            preferredStyle:UIAlertControllerStyleAlert];
    if(okTitle){
        UIAlertAction* okAction     = [UIAlertAction actionWithTitle:okTitle style:UIAlertActionStyleDefault handler:onOK];
        [alert addAction:okAction];
    }
    if(cancelTitle){
        UIAlertAction* cancelAction = [UIAlertAction actionWithTitle:cancelTitle style:UIAlertActionStyleCancel handler:onCancel];
        [alert addAction:cancelAction];
    }
    return alert;
}

#pragma mark - <NSDictionary>
+ (BOOL)dictionary:(NSDictionary*)dictionary contains:(NSDictionary*)contains{
    __block NSInteger containsCount = 0;
    [contains enumerateKeysAndObjectsUsingBlock:^(id  _Nonnull key, id  _Nonnull obj, BOOL * _Nonnull stop) {
        if(dictionary[key] && [dictionary[key] isEqual:obj]){
            ++containsCount;
        }
    }];
    return contains.count==containsCount;
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

#pragma mark - <Time>
+ (NSString*)timeStringWithSecond:(NSTimeInterval)second timeFormat:(NSString*)format{
    NSString *string = format;
    
    NSRange hh = [format rangeOfString:@"hh"];
    if(hh.location != NSNotFound){
        NSString *hour = [NSString stringWithFormat:@"%02d",(int)second/3600];
        string = [string stringByReplacingCharactersInRange:hh withString:hour];
    }
    
    NSRange mm = [format rangeOfString:@"mm"];
    if(mm.location != NSNotFound){
        NSString *min = [NSString stringWithFormat:@"%02d",((int)second%3600)/60];
        string = [string stringByReplacingCharactersInRange:mm withString:min];
    }
    
    NSRange ss = [format rangeOfString:@"ss"];
    if(mm.location != NSNotFound){
        NSString *sec = [NSString stringWithFormat:@"%02d",(int)second%60];
        string = [string stringByReplacingCharactersInRange:ss withString:sec];
    }
    
    return string;
}

#pragma mark - <File System>
+ (NSString*)documentAbsolutePathString{
    return [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
}
+ (NSURL*)documentAbsolutePathUrl{
    return [[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].lastObject;
}
+ (NSString*)absolutePathStringInDocument:(NSArray*)nodes{
    NSString *filePath  = [self documentAbsolutePathString];
    
    NSEnumerator *enumer    = nodes.objectEnumerator;
    NSString *node          = nil;
    while (nil != (node = enumer.nextObject)) {
        filePath = [filePath stringByAppendingPathComponent:node];
    }
    return filePath;
}
+ (NSURL*)absolutePathUrlInDocument:(NSArray*)nodes{
    NSURL *url  = [self documentAbsolutePathUrl];
    
    NSEnumerator *enumer    = nodes.objectEnumerator;
    NSString *node          = nil;
    while (nil != (node = enumer.nextObject)) {
        url = [url URLByAppendingPathComponent:node];
    }
    return url;
}
+ (nullable NSString*)mkdirInDocument:(NSArray*)nodes error:(NSError**)error{
    NSString *dirPath = [self absolutePathStringInDocument:nodes];
    if([[NSFileManager defaultManager] createDirectoryAtPath:dirPath withIntermediateDirectories:YES attributes:nil error:error]){
        return dirPath;
    }
    return nil;
}

#pragma mark - <TouchID/FaceID>
+ (BOOL)evaluatePolicyWithReason:(NSString*)reason reply:(void(^)(BOOL success, NSError * _Nullable error))reply{
    LAContext *context = [LAContext new];
    if(![context canEvaluatePolicy:LAPolicyDeviceOwnerAuthentication error:nil])
        return NO;
    [[LAContext new] evaluatePolicy:LAPolicyDeviceOwnerAuthentication localizedReason:reason reply:reply];
    return YES;
}

#pragma mark - <Thread>
+ (void)mainThreadProcess:(void(^)(void))handler{
    if([NSThread currentThread].isMainThread){
        handler();
    }
    else {
        dispatch_async(dispatch_get_main_queue(), handler);
    }
}

#pragma mark - <Xib/Image in bundle>
+ (NSBundle*)bundleNamed:(NSString*)name{
    if (name.length > 0) {
        NSString *bundlePath = [[NSBundle mainBundle] pathForResource:name ofType:@"bundle"];
        return [NSBundle bundleWithPath:bundlePath];
    }
    return nil;
}

#pragma mark - <Run Time>
+ (void)replaceMethod:(Class)cls src:(SEL)src dest:(SEL)dest{
    Method srcMethod = class_getInstanceMethod(cls, src);
    Method destMethod = class_getInstanceMethod(cls, dest);
    
    BOOL didAddMethod =
    class_addMethod(cls, src, method_getImplementation(destMethod), method_getTypeEncoding(destMethod));
    
    if (didAddMethod) {
        class_replaceMethod(cls, dest, method_getImplementation(srcMethod), method_getTypeEncoding(srcMethod));
    }
    else {
        method_exchangeImplementations(srcMethod, destMethod);
    }
}

#pragma mark - <Audio/Video>
+ (NSTimeInterval)audioDuration:(NSURL*)url{
    NSDictionary *options   = @{AVURLAssetPreferPreciseDurationAndTimingKey:@(YES)};
    AVURLAsset *asset       = [AVURLAsset URLAssetWithURL:url options:options];;
    CMTime duration         = asset.duration;
    return CMTimeGetSeconds(duration);
}

#pragma mark - <Image>
+ (UIImage*)imageFromColor:(UIColor*)color size:(CGSize)size{
    CGRect rect = CGRectMake(0, 0, size.width, size.height);
    
    UIGraphicsBeginImageContextWithOptions(rect.size, NO, [UIScreen mainScreen].scale);
    
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSetFillColorWithColor(context,color.CGColor);
    CGContextFillRect(context, rect);
    
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    return image;
}
+ (UIImage*)autoImage:(id)obj{
    if([obj isKindOfClass:NSString.class]){
        return [UIImage imageNamed:obj];
    }
    else if([obj isKindOfClass:NSArray.class]){
        // 0:非暗黑模式下 1:暗黑模式下
        if (@available(iOS 13.0, *)) {
            if(UITraitCollection.currentTraitCollection.userInterfaceStyle==UIUserInterfaceStyleDark && [obj count]>1){
                return [XXocUtils autoImage:obj[1]];
            }
            else{
                return [XXocUtils autoImage:obj[0]];
            }
        } else {
            return [XXocUtils autoImage:obj[0]];
        }
    }
    else if([obj isKindOfClass:UIImage.class]){
        return obj;
    }
    else{
        return nil;
    }
}

#pragma mark - <权限>
+ (AVAuthorizationStatus)authorizedCamera{
    return [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
}
+ (AVAuthorizationStatus)authorizedMicrophone{
    return [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeAudio];
}
+ (void)anthorizedCameraCheckAtViewController:(UIViewController*)viewController title:(NSString*)title message:(NSString*)message succeed:(void(^)(void))succeed{
    AVAuthorizationStatus status = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
    if(AVAuthorizationStatusDenied != status) {
        succeed();
    }
    else{
        UIAlertController *alert = [XXocUtils alertWithTitle:title
                                                         msg:message
                                                     okTitle:NSLocalizedString(@"gotoSystemSettings", @"去设置")
                                                        onOK:^(UIAlertAction * _Nonnull action) {
            NSURL *url = [NSURL URLWithString:UIApplicationOpenSettingsURLString];
            [[UIApplication sharedApplication] openURL:url options:@{} completionHandler:nil];
        }
                                                 cancelTitle:NSLocalizedString(@"cancel", @"取消")
                                                    onCancel:^(UIAlertAction * _Nonnull action) {

        }];
        [viewController presentViewController:alert animated:YES completion:^{

        }];
        
    }
}

#pragma mark - <AttributeString>
+ (void)attributeString:(NSMutableAttributedString*)aString color:(id)color matchString:(nullable NSString*)mString{
    UIColor *c = [XXocUtils autoColor:color];
    NSRange range = NSMakeRange(0, aString.string.length);
    if(mString){
        range = [aString.string rangeOfString:mString];
    }
    if(c){
        [aString addAttribute:NSForegroundColorAttributeName value:c range:range];
    }
}
+ (void)attributeString:(NSMutableAttributedString*)aString font:(UIFont*)font matchString:(nullable NSString*)mString{
    NSRange range = NSMakeRange(0, aString.string.length);
    if(mString){
        range = [aString.string rangeOfString:mString];
    }
    [aString addAttribute:NSFontAttributeName value:font range:range];
}
@end
