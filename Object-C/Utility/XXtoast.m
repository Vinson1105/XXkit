#import "XXtoast.h"
#import <UIKit/UIKit.h>
#import "./XXtimer.h"

/// 用于显示的组件
@interface XXtoastItem : UIButton
@property (nonatomic,strong) XXtimer *timer;
@end
@implementation XXtoastItem
+(XXtoastItem*) toastItemWithMessage:(NSString*)message duration:(NSTimeInterval)duration{
    XXtoastItem *item = [XXtoastItem buttonWithType:UIButtonTypeCustom];
    item.translatesAutoresizingMaskIntoConstraints = NO;
    item.titleLabel.font        = [UIFont systemFontOfSize:15];
    item.backgroundColor        = [UIColor colorWithWhite:0 alpha:0.7];
    item.layer.cornerRadius     = 4;
    item.contentEdgeInsets      = UIEdgeInsetsMake(4, 4, 4, 4);
    [item setTitle:message forState:UIControlStateNormal];
    [item setTitleColor:UIColor.whiteColor forState:UIControlStateNormal];
    
    item.timer = [XXtimer timerWithDelay:0 interval:duration singleShot:YES];
    __weak XXtoastItem *wi = item;
    item.timer.onTimeout = ^(XXtimer * _Nonnull timer, int times) {
        dispatch_async(dispatch_get_main_queue(), ^{
            __strong XXtoastItem *si = wi;
            [si removeFromSuperview];
        });
    };
    
    return item;
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
