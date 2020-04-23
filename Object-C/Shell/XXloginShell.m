//
//  XXloginShell.m
//  XXkitProject
//
//  Created by VINSON on 2020/4/23.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "XXloginShell.h"
#import "../XXocUtils.h"

@interface XXloginShell()
@property (nonatomic,strong) UIButton *accountClearButton;
@property (nonatomic,strong) UIButton *passwordClearButton;
@property (nonatomic,strong) UIButton *passwordSecureButton;
@property (nonatomic,strong) UIButton *okButton;
@end

@implementation XXloginShell
#pragma mark - <Init>
- (void)shellWithAccount:(UITextField*)account password:(UITextField*)password{
    _accountShell = [XXtextFieldShell new];
    [_accountShell shell:account];
    
    _passwordShell = [XXtextFieldShell new];
    [_passwordShell shell:password];
    password.secureTextEntry = YES;
}

#pragma mark - <Config>
- (void)configAccountWithClearButton:(UIButton*)clearButton{
    _accountClearButton = clearButton;
    [clearButton addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
}
- (void)configPasswordWithClearButton:(UIButton*)clearButton secureButton:(UIButton*)secureButton secureON:(UIImage*)secureON secureOFF:(UIImage*)secureOFF{
    _passwordClearButton = clearButton;
    _passwordSecureButton = secureButton;
    [XXocUtils button:secureButton norImg:secureON selImg:secureOFF];
    [clearButton addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
    [secureButton addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
}

- (void)configAccountWithLogoImage:(nullable UIImage*)logoImage clearImage:(UIImage*)clearImage size:(CGSize)size{
    
}
- (void)configPasswordWithLogoImage:(nullable UIImage*)logoImage
                         ClearImage:(UIButton*)clearImage
                           secureON:(UIImage*)secureON
                          secureOFF:(UIImage*)secureOFF
                               size:(CGSize)size
                             margin:(CGFloat)margin
                            spacing:(CGFloat)spacing{
    
}


#pragma mark - <Event>
- (void)onButtonTouchUpInside:(UIButton*)button{
    if(button == _accountClearButton){
        _accountShell.target.text = @"";
    }
    else if(button == _passwordClearButton){
        _passwordShell.target.text = @"";
    }
    else if(button == _passwordSecureButton){
        BOOL on = !_passwordShell.target.isSecureTextEntry;
        _passwordSecureButton.selected = !on;
        _passwordShell.target.secureTextEntry = on;
    }
    else{
        
    }
}
@end
