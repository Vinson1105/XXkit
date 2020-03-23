//
//  BottomBar.m
//  iCamSee
//
//  Created by VINSON on 2019/11/21.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

#import "BottomBar.h"
#import "StackViewShell.h"
#import "ButtonSelectShell.h"

@interface BottomBar()
@property (nonatomic,strong) UIStackView *leftStack;
@property (nonatomic,strong) UIStackView *rightStack;
@property (nonatomic,strong) StackViewShell *leftStackShell;
@property (nonatomic,strong) StackViewShell *rightStackShell;
@property (nonatomic,strong) ButtonSelectShell *selectShell;

@property (nonatomic,assign) CGSize size;
@property (nonatomic,strong) CAGradientLayer *gradinetLayer;
@property (nonatomic,strong) NSLayoutConstraint *selfHeightConstraint;
@end

@implementation BottomBar
- (instancetype)initWithSuperview:(UIView *)view size:(CGSize)size spacing:(CGFloat)spacing padding:(CGFloat)padding{
    self = [super init];
    if(self){
        _size = size;
        
        self.translatesAutoresizingMaskIntoConstraints = NO;
        [view addSubview:self];
        _selfHeightConstraint = [self.heightAnchor constraintEqualToConstant:size.height];
        _selfHeightConstraint.active = YES;
        [self.bottomAnchor constraintEqualToAnchor:view.bottomAnchor].active = YES;
        [self.leadingAnchor constraintEqualToAnchor:view.leadingAnchor].active = YES;
        [self.trailingAnchor constraintEqualToAnchor:view.trailingAnchor].active = YES;
        
        _leftStack = [UIStackView new];
        _leftStack.spacing = spacing;
        _leftStack.alignment = UIStackViewAlignmentCenter;
        _leftStack.distribution = UIStackViewDistributionEqualSpacing;
        _leftStack.translatesAutoresizingMaskIntoConstraints = NO;
        [self addSubview:_leftStack];
        [_leftStack.topAnchor constraintEqualToAnchor:self.topAnchor].active = YES;
        [_leftStack.leadingAnchor constraintEqualToAnchor:self.leadingAnchor constant:padding].active = YES;
        [_leftStack.heightAnchor constraintEqualToConstant:size.height].active = YES;

        _rightStack = [UIStackView new];
        _rightStack.spacing = spacing;
        _rightStack.alignment = UIStackViewAlignmentCenter;
        _rightStack.distribution = UIStackViewDistributionEqualSpacing;
        _rightStack.translatesAutoresizingMaskIntoConstraints = NO;
        [self addSubview:_rightStack];
        [_rightStack.topAnchor constraintEqualToAnchor:self.topAnchor].active = YES;
        [_rightStack.trailingAnchor constraintEqualToAnchor:self.trailingAnchor constant:-padding].active = YES;
        [_rightStack.heightAnchor constraintEqualToConstant:size.height].active = YES;

        _leftStackShell = [StackViewShell new];
        [_leftStackShell shell:_leftStack];
        _rightStackShell = [StackViewShell new];
        [_rightStackShell shell:_rightStack];

        _selectShell = [ButtonSelectShell new];
        
        _gradinetLayer = [CAGradientLayer layer];
        _gradinetLayer.frame = CGRectMake(0,0,size.width,size.height);
        _gradinetLayer.startPoint = CGPointMake(0.5, 0);
        _gradinetLayer.endPoint = CGPointMake(0.5, 1);
        _gradinetLayer.colors = @[(__bridge id)[UIColor colorWithRed:1/255.0 green:1/255.0 blue:1/255.0 alpha:0.0].CGColor,(__bridge id)[UIColor colorWithRed:0/255.0 green:0/255.0 blue:0/255.0 alpha:0.8].CGColor];
        _gradinetLayer.locations = @[@(0.0),@(1.0f)];
        [self.layer insertSublayer:_gradinetLayer atIndex:0];
    }
    return self;
}
- (void)layoutSubviews{
    [super layoutSubviews];
    if (@available(iOS 11.0, *)) {
        _selfHeightConstraint.constant = _size.height + self.superview.safeAreaInsets.bottom; // 导致再次调用一次layoutSubviews
        _gradinetLayer.frame = self.bounds;
    } else {
    }
}

- (void)setOnClicked:(void (^)(NSString * _Nonnull, BOOL))onClicked{
    _selectShell.onClicked = onClicked;
}
- (void (^)(NSString * _Nonnull, BOOL))onClicked{
    return _selectShell.onClicked;
}
-(void)addBarView:(UIView*)view name:(NSString*)name atPosition:(BottomBarPosition)position{
    StackViewShell *target = BottomBarPositionLeft == position ? _leftStackShell : _rightStackShell;
    [target add:name view:view];
}
-(void)addBarView:(UIView*)view name:(NSString*)name size:(CGSize)size atPosition:(BottomBarPosition)position{
    StackViewShell *target = BottomBarPositionLeft == position ? _leftStackShell : _rightStackShell;
    [target add:name size:size view:view];
}
-(UIButton*)addBarButton:(NSString*)name normalImage:(nullable NSString*)normal selectedImage:(nullable NSString*)selected disabledImage:(nullable NSString*)disabled autoSelect:(BOOL)autoSelect atPosition:(BottomBarPosition)position{
    UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
    [button setImage:[UIImage imageNamed:normal] forState:UIControlStateNormal];
    if(selected) [button setImage:[UIImage imageNamed:selected] forState:UIControlStateSelected];
    if(disabled) [button setImage:[UIImage imageNamed:disabled] forState:UIControlStateDisabled];
    
    StackViewShell *target = BottomBarPositionLeft == position ? _leftStackShell : _rightStackShell;
    [target add:name view:button];
    autoSelect ? [_selectShell addSelectableButton:button name:name] : [_selectShell addButton:button name:name];
    return button;
}
-(UIButton*)addBarButton:(NSString*)name size:(CGSize)size normalImage:(nullable NSString*)normal selectedImage:(nullable NSString*)selected disabledImage:(nullable NSString*)disabled atPosition:(BottomBarPosition)position{
    UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
    [button setImage:[UIImage imageNamed:normal] forState:UIControlStateNormal];
    if(selected) [button setImage:[UIImage imageNamed:selected] forState:UIControlStateSelected];
    if(disabled) [button setImage:[UIImage imageNamed:disabled] forState:UIControlStateDisabled];
    
    StackViewShell *target = BottomBarPositionLeft == position ? _leftStackShell : _rightStackShell;
    [target add:name size:size view:button];
    [_selectShell addSelectableButton:button name:name];
    return button;
}
//-(void)addBarSelectableButton:(UIButton*)button name:(NSString*)name size:(CGSize)size atPosition:(BottomBarPosition)position{
//    StackViewShell *target = BottomBarPositionLeft == position ? _leftStackShell : _rightStackShell;
//    [target add:name size:size view:button];
//    [_selectShell addSelectableButton:button name:name];
//}
-(void)remove:(NSString*)name atPosition:(BottomBarPosition)position{
    StackViewShell *target = BottomBarPositionLeft == position ? _leftStackShell : _rightStackShell;
    [target remove:name];
}
-(void)enabled:(BOOL)enabled name:(NSString*)name atPosition:(BottomBarPosition)position{
    StackViewShell *target = BottomBarPositionLeft == position ? _leftStackShell : _rightStackShell;
    UIView *view = [target view:name];
    if(nil == view) return;
    if([view respondsToSelector:@selector(setEnabled:)]){
        UIControl *control = (UIControl *)view;
        control.enabled = enabled;
    }
    else{
        view.userInteractionEnabled = enabled;
    }
}
-(void)enabled:(BOOL)enabled names:(NSArray*)names atPosition:(BottomBarPosition)position{
    StackViewShell *target = BottomBarPositionLeft == position ? _leftStackShell : _rightStackShell;
    NSEnumerator *enumer = names.objectEnumerator;
    NSString *name = nil;
    while (nil != (name = enumer.nextObject)) {
        UIView *view = [target view:name];
        if(nil == view) continue;
        if([view respondsToSelector:@selector(setEnabled:)]){
            UIControl *control = (UIControl *)view;
            control.enabled = enabled;
        }
        else{
            view.userInteractionEnabled = enabled;
        }
    }
}

-(UIView*)barView:(NSString*)name atPosition:(BottomBarPosition)position{
    StackViewShell *target = BottomBarPositionLeft == position ? _leftStackShell : _rightStackShell;
    return [target view:name];
}
-(UIButton*)barButton:(NSString*)name atPosition:(BottomBarPosition)position{
    StackViewShell *target = BottomBarPositionLeft == position ? _leftStackShell : _rightStackShell;
    return (UIButton*)[target view:name];
}
@end
