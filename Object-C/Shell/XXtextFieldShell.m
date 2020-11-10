#import "XXtextFieldShell.h"
#import "XXocUtils.h"

CONST_STR(Expression)
CONST_STR(StrongMatchMode)
CONST_STR(MaxLength)
CONST_STR(Shell)

CONST_STR(Image)
CONST_STR(Size)
CONST_STR(Margin)
CONST_STR(Spacing)
CONST_STR(SecureON)
CONST_STR(SecureOFF)
CONST_STR(Clear)
CONST_STR(LeftView)
CONST_STR(RightView)


@interface XXtextFieldShell()
@property (nonatomic,copy,nullable) NSString *lastText;     // 上一次string
@property (nonatomic,strong) NSPredicate *predicate;        // 需要匹配的正则表达式
@property (nonatomic,strong) UIButton *clearButton;
@property (nonatomic,strong) UIButton *secureButton;
@property (nonatomic,strong) UIButton *popButton;
@end

@implementation XXtextFieldShell
#pragma mark - <Init>
- (instancetype)init{
    self = [super init];
    if(self){
        self.maxLength = 0;
    }
    return self;
}
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
    [XXocUtils view:button containsLeft:margin centerYAt:bgView];
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
    [XXocUtils view:clearButton containsLeft:margin centerYAt:bgView];
    [clearButton addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
    
    UIButton *popButton = [UIButton buttonWithType:UIButtonTypeCustom];
    popButton.translatesAutoresizingMaskIntoConstraints = NO;
    [XXocUtils button:popButton norImg:popup selImg:popdown];
    [bgView addSubview:popButton];
    [XXocUtils view:popButton size:size];
    [XXocUtils view:popButton containsRight:-margin centerYAt:bgView];
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
    [XXocUtils view:clearButton containsLeft:margin centerYAt:bgView];
    [clearButton addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
    
    UIButton *secureButton = [UIButton buttonWithType:UIButtonTypeCustom];
    secureButton.translatesAutoresizingMaskIntoConstraints = NO;
    [XXocUtils button:secureButton norImg:secureON selImg:secureOFF];
    [bgView addSubview:secureButton];
    [XXocUtils view:secureButton size:size];
    [XXocUtils view:secureButton containsRight:-margin centerYAt:bgView];
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
        _isMatching = [_predicate evaluateWithObject:currentText];
        
        if(_maxLength>0 && currentText.length>_maxLength){
            textField.text = self.lastText;
            return;
        }
        
        if(self.strongMatchMode){
            if(_isMatching){
                [self acceptChanged:currentText];
            }
            else{
                textField.text = self.lastText;
            }
        }
        else{
            [self acceptChanged:currentText];
        }
    }
    else{
        if(_maxLength>0 && currentText.length>_maxLength){
            textField.text = self.lastText;
            return;
        }
        [self acceptChanged:currentText];
    }
}
- (void)onButtonTouchUpInside:(UIButton*)button{
    if(button == _clearButton){
        self.target.text = @"";
        _clearButton.hidden = YES;
        if(_predicate){
            _isMatching = [_predicate evaluateWithObject:@""];
        }
        [self acceptChanged:@""];
    }
    else if(button == _secureButton){
        _secureButton.selected = !_secureButton.isSelected;
        BOOL on = !_secureButton.isSelected;
        _target.secureTextEntry = on;
    }
    else{
        
    }
}
- (void)acceptChanged:(NSString*)currentText{
    self.lastText = currentText;
    if (_onTextChanged) {
        _onTextChanged(self, currentText);
    }
    if(_clearButton){
        _clearButton.hidden = nil==currentText || 0==currentText.length;
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

#pragma mark - <Quick>
+ (NSString *)targetClass{
    return NSStringFromClass(XXtextFieldShell.class);
}
+ (void)obj:(id)obj key:(NSString *)key value:(id)value{
    XXtextFieldShell *shell = obj;
    
    // MARK: Expression
    if(IS_KEY_MATCH(kExpression)){
        if([value isKindOfClass:NSString.class]){
            shell.expression = value;
        }
        else{
            [self unexecutedKey:key value:value];
        }
    }
    
    // MARK: StrongMatchMode
    else if(IS_KEY_MATCH(kStrongMatchMode)){
        if(IS_VALUE_KIND(NSNumber)){
            shell.strongMatchMode = [value boolValue];
        }
        else{
            [self unexecutedKey:key value:value];
        }
    }
    
    // MARK: Shell
    else if(IS_KEY_MATCH(kShell)){
        if(IS_VALUE_KIND(UITextField)){
            [shell shell:value];
        }
        else{
            [self unexecutedKey:key value:value];
        }
    }
    
    // MARK: LeftView
    else if(IS_KEY_MATCH(kLeftView)){
        if(IS_VALUE_KIND(NSDictionary) && value[kImage] && value[kSize]){
            UIImage *image = [XXocUtils autoImage:value[kImage]];
            CGSize size = CGSizeFromString(value[kSize]);
            
            [shell configLogo:image size:size];
        }
        else{
            [super obj:obj key:key value:value];
        }
    }
    
    // MARK: RightView
    else if (IS_KEY_MATCH(kRightView)){
        if(IS_VALUE_KIND(NSDictionary)){
            NSDictionary *dict = value;
            UIImage *clearImage = dict[kClear] ? [XXocUtils autoImage:dict[kClear]] : nil;
            UIImage *secureONImage = dict[kSecureON] ? [XXocUtils autoImage:dict[kSecureON]] : nil;
            UIImage *secureOFFImage = dict[kSecureOFF] ? [XXocUtils autoImage:dict[kSecureOFF]] : nil;
            CGSize size = dict[kSize] ? CGSizeFromString(dict[kSize]) : CGSizeMake(30, 30);
            CGFloat margin = dict[kMargin] ? [dict[kMargin] floatValue] : 0;
            CGFloat spacing = dict[kSpacing] ? [dict[kSpacing] floatValue] : 0;
            
            if(secureONImage && secureOFFImage){
                [shell configClear:clearImage
                          secureON:secureONImage
                         secureOFF:secureOFFImage
                              size:size
                            margin:margin
                           spacing:spacing];
            }
            else{
                [shell configClear:clearImage size:size margin:margin];
            }            
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
