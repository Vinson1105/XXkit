//
//  RightBar.m
//  iCamSee
//
//  Created by VINSON on 2019/11/22.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

#import "RightBar.h"
#import "StackViewShell.h"
#import "ButtonSelectShell.h"
#import "XXmacro.h"

@interface RightBar()
@property (nonatomic,strong) UIStackView *stackView;
@property (nonatomic,strong) StackViewShell *stackShell;
@property (nonatomic,strong) ButtonSelectShell *selectShell;
@end

@implementation RightBar
-(instancetype) initWithSuperview:(UIView*)view size:(CGSize)size spacing:(CGFloat)spacing padding:(CGFloat)padding margin:(CGFloat)margin{
    self = [super init];
    if (self) {
        self.translatesAutoresizingMaskIntoConstraints = NO;
        self.layer.cornerRadius = size.width * 0.5;
        self.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0.5];
        [view addSubview:self];
        AnchorWidth(self, size.width)
        AnchorCenterY(self, view)
        if (@available(iOS 11.0, *)) {
            AnchorTrailing(self, view.safeAreaLayoutGuide, -margin)
        } else {
            AnchorTrailing(self, view, -margin)
        }
        
        _stackView = [UIStackView new];
        _stackView.backgroundColor = UIColor.redColor;
        _stackView.axis = UILayoutConstraintAxisVertical;
        _stackView.alignment = UIStackViewAlignmentCenter;
        _stackView.distribution = UIStackViewDistributionEqualSpacing;
        _stackView.spacing = spacing;
        _stackView.translatesAutoresizingMaskIntoConstraints = NO;
        [self addSubview:_stackView];
        [_stackView.heightAnchor constraintGreaterThanOrEqualToConstant:size.width].active = YES;
        AnchorFill(_stackView, self, padding, 0, -padding, 0)
        
        _stackShell = [StackViewShell new];
        [_stackShell shell:_stackView];
        
        _selectShell = [ButtonSelectShell new];
    }
    return self;
}

- (void)setOnClicked:(void (^)(NSString * _Nonnull, BOOL))onClicked{
    _selectShell.onClicked = onClicked;
}
- (void (^)(NSString * _Nonnull, BOOL))onClicked{
    return _selectShell.onClicked;
}
-(void)addBarView:(UIView*)view name:(NSString*)name size:(CGSize)size{
    [_stackShell add:name size:size view:view];
}
-(UIButton*)addBarButton:(NSString*)name size:(CGSize)size{
    UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
    [_stackShell add:name size:size view:button];
    [_selectShell addButton:button name:name];
    return button;
}
-(UIButton*)addBarButton:(NSString*)name size:(CGSize)size normalImage:(NSString*)normal selectedImage:(nullable NSString*)selected disabledImage:(nullable NSString*)disabled{
    UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
    [button setImage:[UIImage imageNamed:normal] forState:UIControlStateNormal];
    if(selected) [button setImage:[UIImage imageNamed:selected] forState:UIControlStateSelected];
    if(disabled) [button setImage:[UIImage imageNamed:disabled] forState:UIControlStateDisabled];
    [_stackShell add:name size:size view:button];
    if(selected){
        [_selectShell addSelectableButton:button name:name];
    }
    else{
        [_selectShell addButton:button name:name];
    }
    return button;
}
-(void)removeBarView:(NSString*)name{
    [_stackShell remove:name];
    [_selectShell remove:name];
}
-(void)enabledBarView:(BOOL)enabled name:(NSString*)name{
    UIView *view = [_stackShell view:name];
    if([view respondsToSelector:@selector(setEnabled:)]){
        UIControl *control = (UIControl*)view;
        [control setEnabled:enabled];
    }
    else{
        view.userInteractionEnabled = enabled;
    }
}

@end
