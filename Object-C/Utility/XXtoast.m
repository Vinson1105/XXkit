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



@interface XXtoastItem : UIView
@property (nonatomic,strong) XXtimer *timer;
@property (nonatomic,strong) UILabel *titleLabel;
@end
@implementation XXtoastItem
+(XXtoastItem*) toastItemWithMessage:(NSString*)message duration:(CGFloat)duration{
    return [[XXtoastItem alloc] initWithMessage:message duration:duration];
}
- (instancetype)initWithMessage:(NSString*)message duration:(CGFloat)duration{
    self = [super init];
    if (self) {
        self.translatesAutoresizingMaskIntoConstraints = NO;
        
        _titleLabel                 = [UILabel new];
        _titleLabel.textAlignment   = NSTextAlignmentCenter;
        _titleLabel.numberOfLines   = 0;
        _titleLabel.font            = [UIFont systemFontOfSize:15];
        _titleLabel.text            = message;
        _titleLabel.textColor       = UIColor.whiteColor;
        _titleLabel.translatesAutoresizingMaskIntoConstraints = NO;
        [self addSubview:_titleLabel];
        
        [_titleLabel.topAnchor constraintEqualToAnchor:self.topAnchor constant:4].active = YES;
        [_titleLabel.leadingAnchor constraintEqualToAnchor:self.leadingAnchor constant:4].active = YES;
        [_titleLabel.bottomAnchor constraintEqualToAnchor:self.bottomAnchor constant:-4].active = YES;
        [_titleLabel.trailingAnchor constraintEqualToAnchor:self.trailingAnchor constant:-4].active = YES;
        [_titleLabel setContentHuggingPriority:UILayoutPriorityDefaultHigh forAxis:UILayoutConstraintAxisHorizontal];

        _timer = [XXtimer timerWithDelay:0 interval:duration singleShot:YES];
        __weak typeof(self) ws = self;
        _timer.onTimeout = ^(int times) {
            __strong typeof(ws) ss = ws;
            dispatch_async(dispatch_get_main_queue(), ^{
                [ss removeFromSuperview];
            });
        };
        
        self.backgroundColor = [UIColor colorWithWhite:0 alpha:0.7];
        self.layer.cornerRadius = 4;
        [self setContentHuggingPriority:UILayoutPriorityDefaultHigh forAxis:UILayoutConstraintAxisHorizontal];
    }
    return self;
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
        
        NSLayoutConstraint *constraint = [self.item.leadingAnchor constraintGreaterThanOrEqualToAnchor:keyWindow.leadingAnchor constant:20];
        constraint.priority = 500;
        constraint.active = YES;
        
        constraint = [self.item.trailingAnchor constraintGreaterThanOrEqualToAnchor:keyWindow.trailingAnchor constant:-20];
        constraint.priority = 500;
        constraint.active = YES;
        
        [_item.timer start];
    }
    else{
        dispatch_async(dispatch_get_main_queue(), ^{
            UIWindow *keyWindow = [UIApplication sharedApplication].keyWindow;
            [keyWindow addSubview:self.item];
            [self.item.centerXAnchor constraintEqualToAnchor:keyWindow.centerXAnchor].active = YES;
            [self.item.centerYAnchor constraintEqualToAnchor:keyWindow.centerYAnchor].active = YES;
            
            NSLayoutConstraint *constraint = [self.item.leadingAnchor constraintGreaterThanOrEqualToAnchor:keyWindow.leadingAnchor constant:20];
            constraint.priority = 500;
            constraint.active = YES;
            
            constraint = [self.item.trailingAnchor constraintGreaterThanOrEqualToAnchor:keyWindow.trailingAnchor constant:-20];
            constraint.priority = 500;
            constraint.active = YES;
            
            [self.item.timer start];
        });
    }
}
@end
