//
//  TopTitleView.m
//  iCamSee
//
//  Created by VINSON on 2019/11/21.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

#import "TopBar.h"
@interface TopBar()
@property (nonatomic,strong) UIButton *leftButton;
@property (nonatomic,strong) UILabel *titleLabel;
@end

@implementation TopBar
-(instancetype) initWithSuperview:(UIView*)view size:(CGSize)size returnImage:(UIImage*)image margin:(CGFloat)margin{
    self = [super init];
    if(self){
        [view addSubview:self];
        self.translatesAutoresizingMaskIntoConstraints = NO;
        [self.topAnchor constraintEqualToAnchor:view.topAnchor].active = YES;
        [self.leadingAnchor constraintEqualToAnchor:view.leadingAnchor].active = YES;
        [self.trailingAnchor constraintEqualToAnchor:view.trailingAnchor].active = YES;
        [self.heightAnchor constraintEqualToConstant:size.height].active = YES;
        
        _leftButton = [UIButton buttonWithType:UIButtonTypeCustom];
        _leftButton.titleEdgeInsets = UIEdgeInsetsMake(0, 10, 0, -10);
        _leftButton.translatesAutoresizingMaskIntoConstraints = NO;
        [_leftButton addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
        [_leftButton setImage:image forState:UIControlStateNormal];
        [_leftButton setTitleColor:UIColor.whiteColor forState:UIControlStateNormal];
        [self addSubview:_leftButton];
        [_leftButton.leadingAnchor constraintEqualToAnchor:self.leadingAnchor constant:margin].active = YES;
        [_leftButton.centerYAnchor constraintEqualToAnchor:self.centerYAnchor].active = YES;
        
        _titleLabel = [UILabel new];
        _titleLabel.translatesAutoresizingMaskIntoConstraints = NO;
        _titleLabel.textColor = UIColor.whiteColor;
        [self addSubview:_titleLabel];
        [_titleLabel.centerXAnchor constraintEqualToAnchor:self.centerXAnchor].active = YES;
        [_titleLabel.centerYAnchor constraintEqualToAnchor:self.centerYAnchor].active = YES;
        
        CAGradientLayer *gl = [CAGradientLayer layer];
        gl.frame = CGRectMake(0,0,size.width,size.height);
        gl.startPoint = CGPointMake(0.5, 0);
        gl.endPoint = CGPointMake(0.5, 1);
        gl.colors = @[(__bridge id)[UIColor colorWithRed:1/255.0 green:1/255.0 blue:1/255.0 alpha:0.8].CGColor,(__bridge id)[UIColor colorWithRed:0/255.0 green:0/255.0 blue:0/255.0 alpha:0.0].CGColor];
        gl.locations = @[@(0.0),@(1.0f)];
        [self.layer insertSublayer:gl atIndex:0];
    }
    return self;
}


- (void)setLeftTitle:(NSString *)leftTitle{
    [_leftButton setTitle:leftTitle forState:UIControlStateNormal];
    _leftTitle = leftTitle;
}
- (void)setMidTitle:(NSString *)midTitle{
    _titleLabel.text = midTitle;
}

-(void)onButtonTouchUpInside:(id)sender{
    if(_onReturn){
        _onReturn();
    }
}
@end
