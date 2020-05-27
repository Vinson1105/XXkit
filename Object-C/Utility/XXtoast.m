#import "XXtoast.h"
#import <UIKit/UIKit.h>
#import "./XXtimer.h"

/// 用于显示的组件
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

        _timer = [XXtimer timerWithDelay:0 interval:duration singleShot:YES];
        __weak typeof(self) ws = self;
        _timer.onTimeout = ^(XXtimer *timer, int times) {
            __strong typeof(ws) ss = ws;
            dispatch_async(dispatch_get_main_queue(), ^{
                [ss removeFromSuperview];
            });
        };
        
        self.backgroundColor = [UIColor colorWithWhite:0 alpha:0.7];
        self.layer.cornerRadius = 4;
    }
    return self;
}
@end

/// 管理显示组件
@interface XXtoast()
@property (nonatomic,strong) XXtoastItem *item;
@end
@implementation XXtoast
+ (void)showMessage:(NSString*)msg{
    /// 每次都重新生成一个新的组件，避免XXtoast中的item被复用
    XXtoast *toast = [[XXtoast alloc] initWithMessage:msg duration:1.5];
    [toast show];
}
+ (void)showMessage:(NSString*)msg duration:(CGFloat)duartion{
    /// 每次都重新生成一个新的组件，避免XXtoast中的item被复用
    XXtoast *toast = [[XXtoast alloc] initWithMessage:msg duration:duartion];
    [toast show];
}

- (instancetype)initWithMessage:(NSString*)message duration:(NSTimeInterval)duration{
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
