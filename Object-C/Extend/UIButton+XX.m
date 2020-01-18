//
//  UIButton+XX.m
//  Bolts
//
//  Created by VINSON on 2019/10/16.
//

#import "UIButton+XX.h"

@implementation UIButton(XX)
- (UIButton * _Nonnull (^)(NSString * _Nonnull))normalTitle{
    return ^UIButton*(NSString *t){
        [self setTitle:t forState:UIControlStateNormal];
        return self;
    };
}
- (UIButton * _Nonnull (^)(UIColor * _Nonnull))normalTitleColor{
    return ^UIButton*(UIColor *c){
        [self setTitleColor:c forState:UIControlStateNormal];
        return self;
    };
}
- (UIButton * _Nonnull (^)(NSString * _Nonnull))selectedTitle{
    return ^UIButton*(NSString *t){
        [self setTitle:t forState:UIControlStateSelected];
        return self;
    };
}
- (UIButton * _Nonnull (^)(CGFloat))radius{
    return ^UIButton*(CGFloat r){
        self.layer.cornerRadius = r;
        return self;
    };
}
- (UIButton * _Nonnull (^)(CGFloat))borderWidth{
    return ^UIButton*(CGFloat w){
        self.layer.borderWidth = w;
        return self;
    };
}
- (UIButton * _Nonnull (^)(UIColor *_Nonnull))borderColor{
    return ^UIButton*(UIColor *c){
        self.layer.borderColor = c.CGColor;
        return self;
    };
}
@end
