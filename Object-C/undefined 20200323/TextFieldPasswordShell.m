//
//  PassswordTextField.m
//  jsonApp
//
//  Created by VINSON on 2019/11/12.
//  Copyright © 2019 xxx. All rights reserved.
//

#import "TextFieldPasswordShell.h"

@interface TextFieldPasswordShell()
@property (nonatomic,strong,nullable) UIButton *secureButton;
@property (nonatomic,strong) UIButton *clearButton;
@property (nonatomic,strong) UIStackView *rightStackView;
@property (nonatomic,strong) UIView *rightBgView;
@end

@implementation TextFieldPasswordShell
#pragma mark - 构建析构
-(void)shell:(UITextField*)target{
    [super shell:target];
    [self setupUI];
    
    _secureON = YES;
    target.secureTextEntry = YES;
    target.keyboardType = UIKeyboardTypeAlphabet;
    [_clearButton addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
    [_secureButton addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
}
-(void)setupUI{
    _rightBgView = [UIView new];
    _rightBgView.translatesAutoresizingMaskIntoConstraints = NO;
    
    _rightStackView = [[UIStackView alloc] init];
    _rightStackView.alignment = UIStackViewAlignmentFill;
    _rightStackView.distribution = UIStackViewDistributionFillEqually;
    _rightStackView.spacing = 5;
    _rightStackView.translatesAutoresizingMaskIntoConstraints = NO;
    [_rightBgView addSubview:_rightStackView];
    [_rightStackView.topAnchor constraintEqualToAnchor:_rightBgView.topAnchor].active = YES;
    [_rightStackView.leadingAnchor constraintEqualToAnchor:_rightBgView.leadingAnchor constant:5].active = YES;
    [_rightStackView.bottomAnchor constraintEqualToAnchor:_rightBgView.bottomAnchor].active = YES;
    [_rightStackView.trailingAnchor constraintEqualToAnchor:_rightBgView.trailingAnchor constant:-5].active = YES;
    
    _clearButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [_rightStackView addArrangedSubview:_clearButton];
    _clearButton.hidden = YES;
    //_clearButton.backgroundColor = UIColor.redColor;
    
    _secureButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [_rightStackView addArrangedSubview:_secureButton];
    //_secureButton.backgroundColor = UIColor.greenColor;

    __strong typeof(self.target) strongTarget = self.target;
    strongTarget.rightView = _rightBgView;
    strongTarget.rightViewMode = UITextFieldViewModeAlways;
}

- (void)setSecureON:(BOOL)secureON{
    if(_secureON == secureON) return;
    
    __strong typeof(self.target) strongTarget = self.target;
    strongTarget.secureTextEntry = secureON;
    _secureButton.selected = !secureON;

    _secureON = secureON;
}

#pragma mark - 属性读写: <clearImage> <secureONImage> <secureOFFImage>
- (void)setClearImage:(UIImage *)clearImage{
    _clearImage = clearImage;
    [_clearButton setImage:clearImage forState:UIControlStateNormal];
}
- (void)setSecureONImage:(UIImage *)secureONImage{
    _secureONImage = secureONImage;
    [_secureButton setImage:secureONImage forState:UIControlStateNormal];
}
- (void)setSecureOFFImage:(UIImage *)secureOFFImage{
    _secureOFFImage = secureOFFImage;
    [_secureButton setImage:secureOFFImage forState:UIControlStateSelected];
}

#pragma mark - 私有函数: <onButtonTouchupInside>
-(void)onButtonTouchUpInside:(id)sender{
    UIButton *button = (UIButton*)sender;
    __strong typeof(self.target) strongTarget = self.target;
    if(button == _clearButton){
        strongTarget.text = @"";
    }
    else if(button == _secureButton){
        self.secureON = _secureButton.isSelected;
    }
    else{
        
    }
}

#pragma mark - 协议函数: <UITextFieldDelegate>
- (void)textFieldDidBeginEditing:(UITextField *)textField{
    if(_clearImage){
        _clearButton.hidden = NO;
    }
}
- (void)textFieldDidEndEditing:(UITextField *)textField{
    if(_clearImage){
        _clearButton.hidden = YES;
    }
}
@end
