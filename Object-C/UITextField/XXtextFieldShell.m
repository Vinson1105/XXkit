#import "XXtextFieldShell.h"

@interface XXtextFieldShell()
@property (nonatomic,copy,nullable) NSString *lastText;     // 上一次string
@property (nonatomic,strong) NSPredicate *predicate;        // 需要匹配的正则表达式
@end

@implementation XXtextFieldShell
#pragma mark - <Init>
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
        [strongTarget addTarget:self action:@selector(onTextFieldEditingChanged:) forControlEvents:UIControlEventEditingChanged];
    }
}

#pragma mark - <Public>
- (void)configImage:(UIImage*)image mode:(UITextFieldViewMode)mode left:(BOOL)left{
    UIImageView *imageView = [[UIImageView alloc] initWithImage:image];
    if(left){
        _target.leftView = imageView;
        _target.leftViewMode = mode;
    }
    else{
        _target.rightView = imageView;
        _target.rightViewMode = mode;
    }
}

#pragma mark - <Property>
- (void)setExpression:(NSString *)expression{
    _expression = expression;
    _predicate = nil == expression ? nil : [NSPredicate predicateWithFormat:@"SELF MATCHES %@", expression];
}

#pragma mark - <Event>
-(void)onTextFieldEditingChanged:(id)sender{
    if(nil == _predicate) return;
    
    UITextField *textField = (UITextField*)sender;
    NSString *currentText = textField.text;
    if([_predicate evaluateWithObject:currentText]){
        _lastText = textField.text;
        if (_onTextChanged) {
            _onTextChanged(self,currentText);
        }
    }
    else{
        textField.text = _lastText;
    }
}

#pragma mark - <UITextFieldDelegate>
- (BOOL)textFieldShouldReturn:(UITextField *)textField{
    [textField resignFirstResponder];
    return YES;
}
- (BOOL)textFieldShouldClear:(UITextField *)textField{
    return YES;
}
- (void)textFieldDidBeginEditing:(UITextField *)textField{
    if(_onEditBegin){
        _onEditBegin(self);
    }
}
- (void)textFieldDidEndEditing:(UITextField *)textField{
    if(_onEditEnd){
        _onEditEnd(self);
    }
}
@end
