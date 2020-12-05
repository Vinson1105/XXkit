//
//  UIViewController+ActivityIndicator.m
//  iCamSee
//
//  Created by VINSON on 2020/11/11.
//  Copyright © 2020 Macrovideo. All rights reserved.
//

#import "UIViewController+ActivityIndicator.h"
#import <objc/runtime.h>
#import "XXocUtils.h"
#import "XXmodalView.h"

@interface UIViewController()
@property (nonatomic,strong) UIActivityIndicatorView *activityIndicator;
@end

@implementation UIViewController(ActivityIndicator)
- (void)setActivityIndicator_color:(UIColor *)activityIndicator_color{
    objc_setAssociatedObject(self, "activityIndicator_color", activityIndicator_color, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    self.activityIndicator.color = activityIndicator_color;
}
- (UIColor *)activityIndicator_color{
    UIColor *color = objc_getAssociatedObject(self, "activityIndicator_color");
    if(nil == color){
        color = UIColor.whiteColor;
        objc_setAssociatedObject(self, "activityIndicator_color", color, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return color;
}
- (void)setActivityIndicator_backgroundColor:(UIColor *)activityIndicator_backgroundColor{
    objc_setAssociatedObject(self, "activityIndicator_backgroundColor", activityIndicator_backgroundColor, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    self.activityIndicator.backgroundColor = activityIndicator_backgroundColor;
}
- (UIColor *)activityIndicator_backgroundColor{
    UIColor *color = objc_getAssociatedObject(self, "activityIndicator_backgroundColor");
    if(nil == color){
        color = [UIColor colorWithWhite:0 alpha:0.5];
        objc_setAssociatedObject(self, "activityIndicator_backgroundColor", color, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return color;
}

- (UIActivityIndicatorView *)activityIndicator{
    UIActivityIndicatorView *activityIndicator = objc_getAssociatedObject(self, "activityIndicator");
    if(nil == activityIndicator){
        if (@available(iOS 13.0, *)) {
            activityIndicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleLarge];
            
        } else {
            activityIndicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
        }
        
        activityIndicator.color = self.activityIndicator_color;
        activityIndicator.backgroundColor = self.activityIndicator_backgroundColor;
        activityIndicator.layer.cornerRadius = 10;
        
        activityIndicator.translatesAutoresizingMaskIntoConstraints = NO;
        activityIndicator.hidesWhenStopped = YES;
        activityIndicator.hidden = YES;
        [self.view addSubview:activityIndicator];
        [XXocUtils view:activityIndicator centerAt:self.view];
        [XXocUtils view:activityIndicator size:CGSizeMake(60, 60)];
        objc_setAssociatedObject(self, "activityIndicator", activityIndicator, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    };
    return activityIndicator;
}
- (void)activityIndicator_show{
    XXOC_WS;
    [XXocUtils mainThreadProcess:^{
        XXOC_SS;
        UIActivityIndicatorView *view = ss.activityIndicator;
        [ss.view bringSubviewToFront:view];
        [view startAnimating];
        ss.view.userInteractionEnabled = NO;
    }];
}
- (void)activityIndicator_hide{
    XXOC_WS;
    [XXocUtils mainThreadProcess:^{
        XXOC_SS;
        UIActivityIndicatorView *view = ss.activityIndicator;
        [view stopAnimating];
        ss.view.userInteractionEnabled = YES;
    }];
}
@end
