#import "XXverificationCodeShell.h"
#import "XXocUtils.h"

#define kEmaillExpression @"[\\w!#$%&'*+/=?^_`{|}~-]+(?:\\.[\\w!#$%&'*+/=?^_`{|}~-]+)*@(?:[\\w](?:[\\w-]*[\\w])?\\.)+[\\w](?:[\\w-]*[\\w])?"
#define kPhoneNumberExpression @""

CONST_STR(Shell)
CONST_STR(Account)
CONST_STR(Code)
CONST_STR(Request)
CONST_STR(Mode)

@implementation XXverificationCodeShell
- (void)shellAccount:(UITextField*)account code:(UITextField*)code request:(UIButton*)request mode:(XXverificationCodeShellMode)mode{
    XXOC_WS
    
    _accountShell = [XXtextFieldShell new];
    [_accountShell shell:account];
    switch (mode) {
        case XXverificationCodeShellModeEmail:
            _accountShell.expression = kEmaillExpression;
            break;
        case XXverificationCodeShellModePhoneNumber:
            _accountShell.expression = kPhoneNumberExpression;
            break;
        default:
            break;
    }
    _accountShell.onTextChanged = ^(XXtextFieldShell * _Nonnull shell, NSString * _Nonnull text) {
        XXOC_SS
        ss.requestShell.enabled = shell.isMatching;
        if(ss.onTextChanged){
            ss.onTextChanged(ss);
        }
    };
    
    _codeShell = [XXtextFieldShell new];
    [_codeShell shell:code];
    _codeShell.onTextChanged = ^(XXtextFieldShell * _Nonnull shell, NSString * _Nonnull text) {
      XXOC_SS
        if(ss.onTextChanged){
            ss.onTextChanged(ss);
        }
    };
    
    _requestShell = [XXbuttonLimitShell new];
    [_requestShell shell:request];
    [request addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
}

- (void)onButtonTouchUpInside:(UIButton*)sender{
    if(_onRequest){
        self.onRequest(self, self.accountShell.target.text);
    }
}

#pragma mark - <Quick>
+ (NSString *)targetClass{
    return NSStringFromClass([self class]);
}
+ (void)obj:(id)obj key:(NSString *)key value:(id)value{
    XXverificationCodeShell *shell = obj;
    
    // MARK: Shell
    if(IS_KEY_MATCH(kShell) && IS_VALUE_KIND(NSDictionary)){
        if(value[kAccount] && value[kCode] && value[kRequest] && value[kMode]){
        }
        else{
            [super obj:obj key:key value:value];
        }
    }
    else{
        [super obj:obj key:key value:value];
    }
}
@end
