#import "XXloginShell.h"
#import "../XXocUtils.h"

@interface XXloginShell()
@property (nonatomic,strong) UIButton *accountClearButton;
@property (nonatomic,strong) UIButton *passwordClearButton;
@property (nonatomic,strong) UIButton *passwordSecureButton;

@property (nonatomic,strong) UIButton *okButton;
@property (nonatomic,copy) void(^onOK)(NSString *account, NSString *password);
@end

@implementation XXloginShell
#pragma mark - <Init>
- (void)shellAccount:(UITextField*)account password:(UITextField*)password{
    _accountShell = [XXtextFieldShell new];
    [_accountShell shell:account];
    
    _passwordShell = [XXtextFieldShell new];
    [_passwordShell shell:password];
    password.secureTextEntry = YES;
}

#pragma mark - <Config>
- (void)configAccountLogo:(nullable UIImage*)accountLogo passwordLogo:(nullable UIImage*)passwordLogo size:(CGSize)size{
    if(accountLogo){
        [_accountShell configLogo:accountLogo size:size];
    }
    if(passwordLogo){
        [_passwordShell configLogo:passwordLogo size:size];
    }
}
- (void)configClearImage:(UIImage*)clearImage
                secureON:(nullable UIImage*)secureON
               secureOFF:(nullable UIImage*)secureOFF
                    size:(CGSize)size
                  margin:(CGFloat)margin
                 spacing:(CGFloat)spacing{
    [_accountShell configClear:clearImage size:size margin:margin];
    if(secureON && secureOFF){
        [_passwordShell configClear:clearImage secureON:secureON secureOFF:secureOFF size:size margin:margin spacing:spacing];
    }
    else{
        [_passwordShell configClear:clearImage size:size margin:margin];
    }
}

#pragma mark - <Config>
- (void)configAccountClearButton:(UIButton*)accountClear{
    _accountClearButton = accountClear;
    [accountClear addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
}
- (void)configPasswordClearButton:(nullable UIButton*)clearButton secureButton:(UIButton*)secureButton secureON:(UIImage*)secureON secureOFF:(UIImage*)secureOFF{
    if(clearButton){
        _passwordClearButton = clearButton;
        [clearButton addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
    }
    _passwordSecureButton = secureButton;
    [XXocUtils button:secureButton norImg:secureON selImg:secureOFF];
    [secureButton addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
}

#pragma mark - <Config - OKButton>
- (void)configOKButton:(UIButton*)button action:(void(^)(NSString *account,NSString *password))action{
    _okButton = button;
    _onOK = action;
    [button addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
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
    else if(button == _okButton){
        if(_onOK){
            _onOK(_accountShell.target.text, _passwordShell.target.text);
        }
    }
    else{
        
    }
}
@end
