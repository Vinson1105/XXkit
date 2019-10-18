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
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onOrientationDidChanged:) name:UIDeviceOrientationDidChangeNotification object:nil];
    }
    else{
        [[NSNotificationCenter defaultCenter] removeObserver:self name:UIDeviceOrientationDidChangeNotification object:nil];
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

- (void)setLandscapeChangedBlock:(void (^)(BOOL))landscapeChangedBlock{
    objc_setAssociatedObject(self, @"XXlandscapeChangedBlock", landscapeChangedBlock, OBJC_ASSOCIATION_COPY_NONATOMIC);
}
- (void (^)(BOOL))landscapeChangedBlock{
    return objc_getAssociatedObject(self, @"XXlandscapeChangedBlock");
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
- (void)onOrientationDidChanged:(NSNotification*)notification {
    CGRect frame = self.view.frame;
    BOOL isLandscape = self.landscape;
    if ([[UIDevice currentDevice] orientation] == UIInterfaceOrientationPortrait || [[UIDevice currentDevice] orientation] == UIInterfaceOrientationPortraitUpsideDown) {
        if(frame.size.width > frame.size.height || !isLandscape) return;
    }
    else {
        if(frame.size.width < frame.size.height || isLandscape)  return;
    }
    isLandscape = !isLandscape;
    objc_setAssociatedObject(self, @"XXlandscape", @(isLandscape), OBJC_ASSOCIATION_COPY_NONATOMIC);
    if(nil != self.landscapeChangedBlock){
        self.landscapeChangedBlock(isLandscape);
    }
}
@end
