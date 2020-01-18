#import "TextFieldExpressShell.h"

@interface TextFieldExpressShell()
@property (nonatomic,copy,nullable) NSString *lastText;     // 上一次string
@property (nonatomic,strong) NSPredicate *predicate;        // 需要匹配的正则表达式
@end

@implementation TextFieldExpressShell
#pragma mark - 构建析构
-(void)shell:(UITextField*)target{
    __strong typeof(_target) strongTarget = _target;
    if(strongTarget){
        strongTarget.delegate = nil;
        [strongTarget removeTarget:self action:@selector(onTextFieldEditingChanged:) forControlEvents:UIControlEventEditingChanged];
    }
    _target = target;
    strongTarget = _target;
    if(strongTarget){
        strongTarget.delegate = self;
        strongTarget.returnKeyType = UIReturnKeyDone;
        [strongTarget addTarget:self action:@selector(onTextFieldEditingChanged:) forControlEvents:UIControlEventEditingChanged];
    }
}

#pragma mark - 属性读写: <express>
- (void)setExpress:(NSString *)express{
    _express = express;
    _predicate = nil == express ? nil : [NSPredicate predicateWithFormat:@"SELF MATCHES %@", express];
}

#pragma mark - 事件函数: <UIControlEventEditingChanged>
-(void)onTextFieldEditingChanged:(id)sender{
    if(nil == _predicate) return;
    
    UITextField *textField = (UITextField*)sender;
    NSString *currentText = textField.text;
    if([_predicate evaluateWithObject:currentText]){
        _lastText = textField.text;
    }
    else{
        textField.text = _lastText;
    }
}

#pragma mark - 协议函数: <UITextFieldDelegate>
- (BOOL)textFieldShouldReturn:(UITextField *)textField{
    [textField resignFirstResponder];
    return YES;
}
- (BOOL)textFieldShouldClear:(UITextField *)textField{
    return YES;
}
@end
