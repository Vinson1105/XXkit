#import "XXtextFieldShell.h"
#import "../XXocUtils.h"

@interface XXtextFieldShell()
@property (nonatomic,copy,nullable) NSString *lastText;     // 上一次string
@property (nonatomic,strong) NSPredicate *predicate;        // 需要匹配的正则表达式
@property (nonatomic,strong) UIButton *clearButton;
@property (nonatomic,strong) UIButton *secureButton;
@property (nonatomic,strong) UIButton *popButton;
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
    target.clearButtonMode = UITextFieldViewModeWhileEditing;
    strongTarget = _target;
    if(strongTarget){
        strongTarget.delegate = self;
        [strongTarget addTarget:self action:@selector(onTextFieldEditingChanged:) forControlEvents:UIControlEventEditingChanged];
    }
}

#pragma mark - <Public>
- (void)configLogo:(UIImage*)image size:(CGSize)size{
    UIImageView *imageView = [[UIImageView alloc] initWithImage:image];
    imageView.translatesAutoresizingMaskIntoConstraints = NO;
    imageView.contentMode = UIViewContentModeCenter;
    [XXocUtils view:imageView size:size];
    _target.leftView = imageView;
    _target.leftViewMode = UITextFieldViewModeAlways;
}
- (void)configClear:(UIImage*)image size:(CGSize)size margin:(CGFloat)margin{
    UIView *bgView = [UIView new];
    bgView.translatesAutoresizingMaskIntoConstraints = NO;
    [XXocUtils view:bgView size:CGSizeMake(size.width+margin*2, size.height+margin*2)];
    
    UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
    button.translatesAutoresizingMaskIntoConstraints = NO;
    [button setImage:image forState:UIControlStateNormal];
    button.hidden = YES;
    [bgView addSubview:button];
    [XXocUtils view:button size:size];
    [XXocUtils view:button left:margin centerYAt:bgView];
    [button addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
    
    _target.rightView = bgView;
    _target.rightViewMode = UITextFieldViewModeAlways;
    _target.clearButtonMode = UITextFieldViewModeNever;
    _clearButton = button;
}
- (void)configClear:(UIImage*)clear
              popup:(UIImage*)popup
            popdown:(UIImage*)popdown
               size:(CGSize)size
             margin:(CGFloat)margin
            spacing:(CGFloat)spacing{
    UIView *bgView = [UIView new];
    bgView.translatesAutoresizingMaskIntoConstraints = NO;
    [XXocUtils view:bgView size:CGSizeMake(size.width*2+margin*2+spacing, size.height+margin*2)];
    
    UIButton *clearButton = [UIButton buttonWithType:UIButtonTypeCustom];
    clearButton.translatesAutoresizingMaskIntoConstraints = NO;
    [clearButton setImage:clear forState:UIControlStateNormal];
    clearButton.hidden = YES;
    [bgView addSubview:clearButton];
    [XXocUtils view:clearButton size:size];
    [XXocUtils view:clearButton left:margin centerYAt:bgView];
    [clearButton addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
    
    UIButton *popButton = [UIButton buttonWithType:UIButtonTypeCustom];
    popButton.translatesAutoresizingMaskIntoConstraints = NO;
    [XXocUtils button:popButton norImg:popup selImg:popdown];
    [bgView addSubview:popButton];
    [XXocUtils view:popButton size:size];
    [XXocUtils view:popButton right:-margin centerYAt:bgView];
    [popButton addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
    
    _target.rightView = bgView;
    _target.rightViewMode = UITextFieldViewModeAlways;
    _target.secureTextEntry = YES;
    _clearButton = clearButton;
    _popButton = popButton;
}
- (void)configClear:(UIImage*)clear
           secureON:(UIImage*)secureON
          secureOFF:(UIImage*)secureOFF
               size:(CGSize)size
             margin:(CGFloat)margin
            spacing:(CGFloat)spacing{
    UIView *bgView = [UIView new];
    bgView.translatesAutoresizingMaskIntoConstraints = NO;
    [XXocUtils view:bgView size:CGSizeMake(size.width*2+margin*2+spacing, size.height+margin*2)];
    
    UIButton *clearButton = [UIButton buttonWithType:UIButtonTypeCustom];
    clearButton.translatesAutoresizingMaskIntoConstraints = NO;
    [clearButton setImage:clear forState:UIControlStateNormal];
    clearButton.hidden = YES;
    [bgView addSubview:clearButton];
    [XXocUtils view:clearButton size:size];
    [XXocUtils view:clearButton left:margin centerYAt:bgView];
    [clearButton addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
    
    UIButton *secureButton = [UIButton buttonWithType:UIButtonTypeCustom];
    secureButton.translatesAutoresizingMaskIntoConstraints = NO;
    [XXocUtils button:secureButton norImg:secureON selImg:secureOFF];
    [bgView addSubview:secureButton];
    [XXocUtils view:secureButton size:size];
    [XXocUtils view:secureButton right:-margin centerYAt:bgView];
    [secureButton addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
    
    _target.rightView = bgView;
    _target.rightViewMode = UITextFieldViewModeAlways;
    _target.secureTextEntry = YES;
    _target.clearButtonMode = UITextFieldViewModeNever;
    _clearButton = clearButton;
    _secureButton = secureButton;
}

#pragma mark - <Property>
- (void)setExpression:(NSString *)expression{
    _expression = expression;
    _predicate = nil == expression ? nil : [NSPredicate predicateWithFormat:@"SELF MATCHES %@", expression];
}

#pragma mark - <Event>
-(void)onTextFieldEditingChanged:(id)sender{
    UITextField *textField = (UITextField*)sender;
    NSString *currentText = textField.text;
    if(nil != _predicate){
        NSString *currentText = textField.text;
        if([_predicate evaluateWithObject:currentText]){
            _lastText = textField.text;
            if (_onTextChanged) {
                _onTextChanged(self,textField.text);
            }
            if(_clearButton){
                _clearButton.hidden = nil==currentText || 0==currentText.length;
            }
        }
        else{
            textField.text = _lastText;
        }
    }
    else{
        if (_onTextChanged) {
            _onTextChanged(self,textField.text);
        }
        if(_clearButton){
            _clearButton.hidden = nil==currentText || 0==currentText.length;
        }
    }
}
- (void)onButtonTouchUpInside:(UIButton*)button{
    if(button == _clearButton){
        _target.text = @"";
        _clearButton.hidden = YES;
    }
    else if(button == _secureButton){
        _secureButton.selected = !_secureButton.isSelected;
        BOOL on = !_secureButton.isSelected;
        _target.secureTextEntry = on;
    }
    else{
        
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
    if(_clearButton){
        _clearButton.hidden = nil==textField.text||textField.text.length==0;
    }
    if(_onEditBegin){
        _onEditBegin(self);
    }
}
- (void)textFieldDidEndEditing:(UITextField *)textField{
    if(_clearButton){
        _clearButton.hidden = YES;
    }
    if(_onEditEnd){
        _onEditEnd(self);
    }
}
@end
