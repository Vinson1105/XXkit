//
//  XXtoast.m
//  iCamSee
//
//  Created by VINSON on 2020/4/27.
//  Copyright © 2020 Macrovideo. All rights reserved.
//

#import "XXtoast.h"
#import <UIKit/UIKit.h>
#import "XXtimer.h"



@interface XXtoastItem : UIButton
@property (nonatomic,strong) XXtimer *timer;
@end
@implementation XXtoastItem
+(XXtoastItem*) toastItemWithMessage:(NSString*)message duration:(CGFloat)duration{
    XXtoastItem *item = [XXtoastItem buttonWithType:UIButtonTypeCustom];
    item.translatesAutoresizingMaskIntoConstraints = NO;
    item.titleLabel.font = [UIFont systemFontOfSize:15];
    [item setTitle:message forState:UIControlStateNormal];
    [item setTitleColor:UIColor.whiteColor forState:UIControlStateNormal];
    item.backgroundColor = [UIColor colorWithWhite:0 alpha:0.7];
    item.layer.cornerRadius = 4;
    item.contentEdgeInsets = UIEdgeInsetsMake(4, 4, 4, 4);
    
    item.timer = [XXtimer timerWithDelay:0 interval:duration singleShot:YES];
    __weak XXtoastItem *wi = item;
    item.timer.onTimeout = ^(int times) {
        dispatch_async(dispatch_get_main_queue(), ^{
            __strong XXtoastItem *si = wi;
            [si removeFromSuperview];
        });
    };
    
    return item;
}
@end

@interface XXtoast()
@property (nonatomic,strong) XXtoastItem *item;
@end

@implementation XXtoast
+ (void)showMessage:(NSString*)msg{
    XXtoast *toast = [[XXtoast alloc] initWithMessage:msg duration:1.5];
    [toast show];
}
+ (void)showMessage:(NSString*)msg duration:(CGFloat)duartion{
    XXtoast *toast = [[XXtoast alloc] initWithMessage:msg duration:duartion];
    [toast show];
}

- (instancetype)initWithMessage:(NSString*)message duration:(CGFloat)duration{
    self = [super init];
    if(self){
        _item = [XXtoastItem toastItemWithMessage:message duration:duration];
    }
    return self;
}
- (void)show{
    if([NSThread currentThread].isMainThread){
        UIWindow *keyWindow = [UIApplication sharedApplication].keyWindow;
        [keyWindow addSubview:_item];
        [self.item.centerXAnchor constraintEqualToAnchor:keyWindow.centerXAnchor].active = YES;
        [self.item.centerYAnchor constraintEqualToAnchor:keyWindow.centerYAnchor].active = YES;
        [_item.timer start];
    }
    else{
        dispatch_async(dispatch_get_main_queue(), ^{
            UIWindow *keyWindow = [UIApplication sharedApplication].keyWindow;
            [keyWindow addSubview:self.item];
            [self.item.centerXAnchor constraintEqualToAnchor:keyWindow.centerXAnchor].active = YES;
            [self.item.centerYAnchor constraintEqualToAnchor:keyWindow.centerYAnchor].active = YES;
            [self.item.timer start];
        });
    }
}
@end
