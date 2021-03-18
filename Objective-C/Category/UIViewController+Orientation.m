#import "UIViewController+Orientation.h"
#import <objc/runtime.h>

#define kLandscape                  @"landscape"
#define kOnOrientationWillChange    @"onOrientationWillChange"
#define kOnOrientationDidChanged    @"onOrientationDidChanged"
#define kPortraitLayouts            @"portraitLayouts"
#define kLandscapeLayouts           @"landscaoeLayouts"

@interface UIViewController()
@property (nonatomic,assign) BOOL landscape;
@end

@implementation UIViewController (Orientation)
#pragma mark - Public
- (void)installOrientation:(BOOL)install{
    if(install){
        [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onOrientationWillChange:) name:UIApplicationWillChangeStatusBarOrientationNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onOrientationDidChanged:) name:UIApplicationDidChangeStatusBarOrientationNotification object:nil];
    }
    else{
        [[NSNotificationCenter defaultCenter] removeObserver:self name:UIApplicationWillChangeStatusBarOrientationNotification object:nil];
        [[NSNotificationCenter defaultCenter] removeObserver:self name:UIApplicationDidChangeStatusBarOrientationNotification object:nil];
    }
}
- (void)landscapeLeft{
    [self toChangeOrientation:UIInterfaceOrientationLandscapeLeft];
}
- (void)landscapeRight{
    [self toChangeOrientation:UIInterfaceOrientationLandscapeRight];
}
- (void)portrait{
    [self toChangeOrientation:UIInterfaceOrientationPortrait];
}

#pragma mark - Property
- (void)setLandscape:(BOOL)landscape{
    objc_setAssociatedObject(self, kLandscape, @(landscape), OBJC_ASSOCIATION_ASSIGN);
}
- (BOOL)landscape{
    return [objc_getAssociatedObject(self, kLandscape) boolValue];
}
- (BOOL)isLandscape{
    UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
    return orientation == UIInterfaceOrientationLandscapeLeft || orientation == UIInterfaceOrientationLandscapeRight;
}
- (void)setOnOrientationDidChanged:(void (^)(void))onOrientationDidChanged{
    objc_setAssociatedObject(self, kOnOrientationDidChanged, onOrientationDidChanged, OBJC_ASSOCIATION_COPY_NONATOMIC);
}
- (void (^)(void))onOrientationDidChanged{
    return objc_getAssociatedObject(self, kOnOrientationDidChanged);
}
- (void)setOnOrientationWillChange:(void (^)(void))onOrientationWillChange{
    objc_setAssociatedObject(self, kOnOrientationWillChange, onOrientationWillChange, OBJC_ASSOCIATION_COPY_NONATOMIC);
}
- (void (^)(void))onOrientationWillChange{
    return objc_getAssociatedObject(self, kOnOrientationWillChange);
}

- (void)setPortraitLayouts:(NSArray<NSLayoutConstraint *> *)portraitLayouts{
    objc_setAssociatedObject(self, kPortraitLayouts, portraitLayouts, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
- (NSArray<NSLayoutConstraint *> *)portraitLayouts{
    return objc_getAssociatedObject(self, kPortraitLayouts);
}
- (void)setLandscapeLayouts:(NSArray<NSLayoutConstraint *> *)landscapeLayouts{
    objc_setAssociatedObject(self, kLandscapeLayouts, landscapeLayouts, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
- (NSArray<NSLayoutConstraint *> *)landscapeLayouts{
    return objc_getAssociatedObject(self, kLandscapeLayouts);
}

- (void)toChangeOrientation:(int)orientation{
    if ([[UIDevice currentDevice] respondsToSelector:@selector(setOrientation:)]) {
        SEL selector = NSSelectorFromString(@"setOrientation:");
        NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:[UIDevice instanceMethodSignatureForSelector:selector]];
        [invocation setSelector:selector];
        [invocation setTarget:[UIDevice currentDevice]];
        int val = orientation;
        [invocation setArgument:&val atIndex:2];
        [invocation invoke];
    }
    else{
        NSLog(@"[UIViewController+Orientation] Error occurred! (LINE:%d)", __LINE__);
    }
}
- (void)onOrientationWillChange:(NSNotification*)notification {
    /// MARK: 会触发两次，但是现在已经加上判断，避免了这个问题，但是后续需要了解为何这样
    NSDictionary *userInfo          = notification.userInfo;
    UIInterfaceOrientation will     = [userInfo[@"UIApplicationStatusBarOrientationUserInfoKey"] intValue];
    
    BOOL isWillLandscape = will == UIInterfaceOrientationLandscapeLeft || will == UIInterfaceOrientationLandscapeRight;
    if(self.isLandscape == isWillLandscape){
        return;
    }

    if(nil != self.onOrientationWillChange){
        self.onOrientationWillChange();
    }
}
- (void)onOrientationDidChanged:(NSNotification*)notification {
    /// MARK: 会触发两次，但是现在已经加上判断，避免了这个问题，但是后续需要了解为何这样
    BOOL isLandscape    = self.isLandscape;
    if(self.landscape == isLandscape){
        return;
    }
    self.landscape = isLandscape;
    
    NSArray *nonactive  = isLandscape ? self.portraitLayouts : self.landscapeLayouts;
    NSArray *active     = isLandscape ? self.landscapeLayouts : self.portraitLayouts;
    if(active && nonactive){
        [NSLayoutConstraint deactivateConstraints:nonactive];
        [NSLayoutConstraint activateConstraints:active];

        [UIView animateWithDuration:0.03 animations:^{
            [self.view layoutIfNeeded];
        }];
    }

    if(nil != self.onOrientationDidChanged){
        self.onOrientationDidChanged();
    }
}
@end
