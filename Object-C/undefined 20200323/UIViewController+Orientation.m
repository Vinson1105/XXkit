//
//  UIViewController+Orientation.m
//  Bolts
//
//  Created by VINSON on 2019/10/14.
//

#import "UIViewController+Orientation.h"
#import <objc/runtime.h>

@implementation UIViewController (Orientation)
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

- (void)setLandscape:(BOOL)landscape{
    if(landscape == [self landscape]){
        return;
    }
    [self toLandscape:landscape];
    objc_setAssociatedObject(self, @"XXlandscape", @(landscape), OBJC_ASSOCIATION_COPY_NONATOMIC);
}
- (BOOL)landscape{
    return [objc_getAssociatedObject(self, @"XXlandscape") boolValue];
}

- (void)setLandscapeDidChangedBlock:(void (^)(BOOL))landscapeChangedBlock{
    objc_setAssociatedObject(self, @"XXlandscapeDidChangedBlock", landscapeChangedBlock, OBJC_ASSOCIATION_COPY_NONATOMIC);
}
- (void (^)(BOOL))landscapeDidChangedBlock{
    return objc_getAssociatedObject(self, @"XXlandscapeDidChangedBlock");
}

- (void)setLandscapeWillChangeBlock:(void (^)(void))landscapeWillChangeBlock{
    objc_setAssociatedObject(self, @"XXlandscapeWillChangeBlock", landscapeWillChangeBlock, OBJC_ASSOCIATION_COPY_NONATOMIC);
}
- (void (^)(void))landscapeWillChangeBlock{
    return objc_getAssociatedObject(self, @"XXlandscapeWillChangeBlock");
}

- (void)setInstallLayouts:(NSArray<NSLayoutConstraint *> *)installLayouts{
    objc_setAssociatedObject(self, @"XXinstallLayouts", installLayouts, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
- (NSArray<NSLayoutConstraint *> *)installLayouts{
    return objc_getAssociatedObject(self, @"XXinstallLayouts");
}
- (void)setLandscapeLayouts:(NSArray<NSLayoutConstraint *> *)landscapeLayouts{
    objc_setAssociatedObject(self, @"XXlandscapeLayouts", landscapeLayouts, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
- (NSArray<NSLayoutConstraint *> *)landscapeLayouts{
    return objc_getAssociatedObject(self, @"XXlandscapeLayouts");
}

- (void)toLandscape:(BOOL)landscape{
    if ([[UIDevice currentDevice] respondsToSelector:@selector(setOrientation:)]) {
        SEL selector = NSSelectorFromString(@"setOrientation:");
        NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:[UIDevice instanceMethodSignatureForSelector:selector]];
        [invocation setSelector:selector];
        [invocation setTarget:[UIDevice currentDevice]];
        int val = landscape ? UIInterfaceOrientationLandscapeRight : UIInterfaceOrientationPortrait;
        [invocation setArgument:&val atIndex:2];
        [invocation invoke];
    }
    else{
        NSLog(@"[UIViewController+Orientation] Error occurred! (LINE:%d)", __LINE__);
    }
}

- (void)onOrientationWillChange:(NSNotification*)notification {
    if(nil != self.landscapeWillChangeBlock){
        self.landscapeWillChangeBlock();
    }
}
- (void)onOrientationDidChanged:(NSNotification*)notification {
    CGRect frame = self.view.frame;
    BOOL isLandscape = self.landscape;
    if ([[UIDevice currentDevice] orientation] == UIInterfaceOrientationPortrait || [[UIDevice currentDevice] orientation] == UIInterfaceOrientationPortraitUpsideDown) {
        if(frame.size.width > frame.size.height || !isLandscape) return;
    }
    else {
        if(frame.size.width < frame.size.height || isLandscape)  return;
    }
    
    NSArray *active = self.landscape ? self.installLayouts : self.landscapeLayouts;
    NSArray *nonactive = self.landscape ? self.landscapeLayouts : self.installLayouts;
    if(active && nonactive){
        [NSLayoutConstraint deactivateConstraints:nonactive];
        [NSLayoutConstraint activateConstraints:active];
        [self.view updateConstraints];
    }
    
    isLandscape = !isLandscape;
    objc_setAssociatedObject(self, @"XXlandscape", @(isLandscape), OBJC_ASSOCIATION_COPY_NONATOMIC);
    if(nil != self.landscapeDidChangedBlock){
        self.landscapeDidChangedBlock(isLandscape);
    }
}
@end
