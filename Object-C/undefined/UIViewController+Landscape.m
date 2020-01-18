//
//  UIViewController+Orientation.m
//  Bolts
//
//  Created by VINSON on 2019/10/14.
//

#import "UIViewController+Landscape.h"
#import <objc/runtime.h>

#define kXXlandscape "XXlandscape"
#define kXXonLandscapeDidChanged "XXonLandscapeDidChanged"
#define kXXonLandscapeWillChange "XXonLandscapeWillChange"
#define kSetOrientation @"setOrientation"

@implementation UIViewController (Landscape)
- (void)installLandscape:(BOOL)install{
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
    objc_setAssociatedObject(self, kXXlandscape, @(landscape), OBJC_ASSOCIATION_ASSIGN);
}
- (BOOL)landscape{
    return [objc_getAssociatedObject(self, kXXlandscape) boolValue];
}

- (void)setOnLandscapeDidChanged:(void (^)(BOOL))onLandscapeDidChangedBlock{
    objc_setAssociatedObject(self, kXXonLandscapeDidChanged, onLandscapeDidChangedBlock, OBJC_ASSOCIATION_COPY_NONATOMIC);
}
- (void (^)(BOOL))onLandscapeDidChanged{
    return objc_getAssociatedObject(self, kXXonLandscapeDidChanged);
}

- (void)setOnLandscapeWillChange:(void (^)(BOOL))onLandscapeWillChange{
    objc_setAssociatedObject(self, kXXonLandscapeWillChange, onLandscapeWillChange, OBJC_ASSOCIATION_COPY_NONATOMIC);
}
- (void (^)(BOOL))onLandscapeWillChange{
    return objc_getAssociatedObject(self, kXXonLandscapeWillChange);
}

- (void)toLandscape:(BOOL)landscape{
    if ([[UIDevice currentDevice] respondsToSelector:@selector(setOrientation:)]) {
        SEL selector = NSSelectorFromString(kSetOrientation);
        NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:[UIDevice instanceMethodSignatureForSelector:selector]];
        [invocation setSelector:selector];
        [invocation setTarget:[UIDevice currentDevice]];
        
        int orientation = landscape ? UIInterfaceOrientationLandscapeRight : UIInterfaceOrientationPortrait;
        [invocation setArgument:&orientation atIndex:2];
        [invocation invoke];
    }
    else{
        NSLog(@"[UIViewController+Landscape] Error (LINE:%d)", __LINE__);
    }
}

- (void)onOrientationWillChange:(NSNotification*)notification {
    if(nil != self.onLandscapeWillChange){
        self.onLandscapeWillChange(self.landscape);
    }
}
- (void)onOrientationDidChanged:(NSNotification*)notification {
    CGRect frame        = self.view.frame;
    BOOL isLandscape    = self.landscape;
    if ([[UIDevice currentDevice] orientation] == UIInterfaceOrientationPortrait || [[UIDevice currentDevice] orientation] == UIInterfaceOrientationPortraitUpsideDown) {
        if(frame.size.width > frame.size.height || !isLandscape) return;
    }
    else {
        if(frame.size.width < frame.size.height || isLandscape)  return;
    }
    isLandscape = !isLandscape;
    objc_setAssociatedObject(self, kXXlandscape, @(isLandscape), OBJC_ASSOCIATION_ASSIGN);
    if(nil != self.onLandscapeDidChanged){
        self.onLandscapeDidChanged(isLandscape);
    }
}
@end
