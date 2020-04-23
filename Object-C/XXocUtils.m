//
//  XXocUtility.m
//  XXkitProject
//
//  Created by VINSON on 2020/4/15.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "XXocUtils.h"

@implementation XXocUtils
+ (UIViewController*)viewController:(NSString*)vc withUIStoryboard:(NSString*)storyboard bundle:(nullable NSBundle*)bundle{
    return [[UIStoryboard storyboardWithName:storyboard bundle:bundle] instantiateViewControllerWithIdentifier:vc];
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
+ (void)view:(UIView*)view left:(CGFloat)left centerYAt:(UIView*)centerYAt{
    [view.leadingAnchor constraintEqualToAnchor:centerYAt.leadingAnchor constant:left].active = YES;
    [view.centerYAnchor constraintEqualToAnchor:centerYAt.centerYAnchor].active = YES;
}
+ (void)view:(UIView*)view right:(CGFloat)right centerYAt:(UIView*)centerYAt{
    [view.trailingAnchor constraintEqualToAnchor:centerYAt.trailingAnchor constant:right].active = YES;
    [view.centerYAnchor constraintEqualToAnchor:centerYAt.centerYAnchor].active = YES;
}

#pragma mark - <UIButton>
+ (void)button:(UIButton*)button norImg:(UIImage*)norImg selImg:(UIImage*)selImg{
    [button setImage:norImg forState:UIControlStateNormal];
    [button setImage:selImg forState:UIControlStateSelected];
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
+ (void)button:(UIButton*)button norTxt:(NSString*)norTxt selTxt:(NSString*)selTxt disNorTxt:(NSString*)disNorTxt disSelTxt:(NSString*)disSelTxt{
    [button setTitle:norTxt forState:UIControlStateNormal];
    [button setTitle:selTxt forState:UIControlStateSelected];
    [button setTitle:disNorTxt forState:UIControlStateNormal|UIControlStateDisabled];
    [button setTitle:disSelTxt forState:UIControlStateSelected|UIControlStateDisabled];
}
@end
