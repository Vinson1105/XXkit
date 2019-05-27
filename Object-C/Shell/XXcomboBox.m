#import "XXcomboBox.h"
#import "XXlistViewShell.h"
#import "../View/XXmaskView.h"
#import "../View/XXshadowCreator.h"
#import "../XXocDefine.h"

@interface XXcomboBox()
@property (nonatomic,weak) UITextField *textField;              // 对应的textField控件
@property (nonatomic,weak) UIView *rootView;                    // textField所在的页面的顶层view,一般是对应页的UIViewController.view
@property (nonatomic,strong) UITableView *tableView;            // 弹出的tableView弹框
@property (nonatomic,strong) XXlistViewShell *listViewShell;    // tableView的列表组件
@property (nonatomic,strong) XXmaskView *maskView;              // 弹出框的背景蒙层

@property (nonatomic,strong) NSMutableArray *itemKeyArray;      // 弹出框选项关键字列表
@property (nonatomic,strong) NSMutableArray *itemTextArray;     // 弹出框选项文本列表
@property (nonatomic,strong) UIImageView *imageIndicator;       // 右边栏指示器图控件
@property (nonatomic,strong) NSMutableDictionary *state2IndicatorImage; // 不同的状态对应右边栏指示器图标对应表

@property (nonatomic,strong) NSLayoutConstraint *indicatorHeightConstraint;       // 指示器的高度约束
@property (nonatomic,strong) NSLayoutConstraint *indicatorWidthConstraint;        // 指示器的宽度约束
@property (nonatomic,strong) NSLayoutConstraint *indicatorRightConstraint;  // 指示器的右边距约束
@property (nonatomic,strong) NSLayoutConstraint *indicatorYConstraint;            // 指示器的水平约束
@end

@implementation XXcomboBox

#pragma mark - 构建析构
- (instancetype) initWithTextField:(UITextField*) textField RootView:(UIView*)rootView{
    self = [super init];
    
    XXOC_WS;
    if (self) {
        // [1] 初始化textField
        _textField          = textField;
        _textField.delegate = self;
        
        // [2] 保存根view
        _rootView = rootView;
        
        // [3] 初始化tableView
        _tableView                      = [[UITableView alloc] init];
        _tableView.backgroundColor      = [UIColor whiteColor];
        _tableView.bounces              = NO;
        _tableView.separatorStyle       = UITableViewCellSeparatorStyleNone;
        _tableView.layer.cornerRadius   = 10;   // 圆角
        _tableView.layer.masksToBounds  = YES;  // 子空间不能超出父控件
        
        // [4] 初始化tableView的列表组件
        _listViewShell = [[XXlistViewShell alloc] initWithContext:_tableView];
        _listViewShell.onRowTouchEvent = ^(NSInteger row, id cellData) {
            ws.currentKey = ws.itemKeyArray[row];
            [ws packup];
        };
        
        // [5] 初始化蒙层
        _maskView = [[XXmaskView alloc] initWithContent:_tableView WillHideBlock:^BOOL{
            if (nil != [ws.state2IndicatorImage objectForKey:[ws stringFromState:XXcomboBoxState_OnPackup]]) {
                [ws.imageIndicator setImage:[ws.state2IndicatorImage objectForKey:[ws stringFromState:XXcomboBoxState_OnPackup]]];
            }
            return YES;
        } Alpha:0.0];
        
        // [6] 添加阴影
        [XXshadowCreator makeShadowWithTarget:_tableView
                                ShadowBgColor:[UIColor blackColor]
                                  ShadowColor:[UIColor blackColor]
                                 ShadowOffset:CGSizeZero
                                ShadowOpacity:0.5
                                 ShadowRadius:5];
        
        // [7] 指示器
        _imageIndicator                 = [[UIImageView alloc] init];
        _imageIndicator.contentMode     = UIViewContentModeCenter;
        _imageIndicator.translatesAutoresizingMaskIntoConstraints = NO; // 使用约束必要项
        [_textField addSubview:_imageIndicator];

        // [8] 指示器约束
        _indicatorSize          = CGSizeMake(_textField.frame.size.height / 3 * 2, _textField.frame.size.height / 3 * 2);
        _indicatorRightMargin   = _textField.frame.size.height / 6;
        _indicatorWidthConstraint = [NSLayoutConstraint constraintWithItem:_imageIndicator
                                                                 attribute:NSLayoutAttributeWidth
                                                                 relatedBy:NSLayoutRelationEqual
                                                                    toItem:nil
                                                                 attribute:0
                                                                multiplier:1.0
                                                                  constant:_indicatorSize.width];
        _indicatorHeightConstraint = [NSLayoutConstraint constraintWithItem:_imageIndicator
                                                                  attribute:NSLayoutAttributeHeight
                                                                  relatedBy:NSLayoutRelationEqual
                                                                     toItem:nil
                                                                  attribute:0
                                                                 multiplier:1.0
                                                                   constant:_indicatorSize.height];
        
        _indicatorRightConstraint = [NSLayoutConstraint constraintWithItem:_imageIndicator
                                                                       attribute:NSLayoutAttributeTrailing
                                                                       relatedBy:NSLayoutRelationEqual
                                                                          toItem:_textField
                                                                       attribute:NSLayoutAttributeTrailing
                                                                      multiplier:1.0
                                                                        constant:-_indicatorRightMargin];
        _indicatorYConstraint = [NSLayoutConstraint constraintWithItem:_imageIndicator
                                                             attribute:NSLayoutAttributeCenterY
                                                             relatedBy:NSLayoutRelationEqual
                                                                toItem:_textField
                                                             attribute:NSLayoutAttributeCenterY
                                                            multiplier:1.0
                                                              constant:0];
        [_textField addConstraints:@[_indicatorRightConstraint,_indicatorYConstraint]];
        [_imageIndicator addConstraints:@[_indicatorWidthConstraint,_indicatorHeightConstraint]];
        
        // [8] 监听横竖屏
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(application_onStatusBarOrientationChanged:) name:UIApplicationDidChangeStatusBarOrientationNotification object:nil];
        
        // [9] 其余变量初始化
        _state2IndicatorImage = [NSMutableDictionary dictionary];
    }
    return self;
}

#pragma mark - 属性读写: <currentKey写> <indicatorSize写> <indicatorRightMargin写> <itemHeight写>
// currentKey写函数
- (void) setCurrentKey:(NSString *)currentKey{
    if (nil == currentKey) {
        _textField.text = nil;
        _currentKey     = nil;
        return;
    }
    
    if ([_itemKeyArray containsObject:currentKey]) {
        NSUInteger index        = [_itemKeyArray indexOfObject:currentKey];
        _textField.text         = [_itemTextArray objectAtIndex:index];
        NSString *previousKey   = _currentKey;
        _currentKey             = currentKey;
        // 注意:直接对属性修改也会触发changed事件
        if (_currentKeyChangedBlock != nil) {
            _currentKeyChangedBlock(previousKey, currentKey);
        }
    }
}
// indicatorSize写函数
- (void)setIndicatorSize:(CGSize)indicatorSize{
    _indicatorSize                      = indicatorSize;
    _indicatorWidthConstraint.constant  = _indicatorSize.width;
    _indicatorHeightConstraint.constant = _indicatorSize.height;
}
// indicatorRightMargin写函数
- (void)setIndicatorRightMargin:(CGFloat)indicatorRightMargin{
    _indicatorRightMargin                       = indicatorRightMargin;
    _indicatorRightConstraint.constant    = -_indicatorRightMargin;
}
// itemHeight写函数
- (void)setItemHeight:(NSUInteger)itemHeight{
    _itemHeight = itemHeight;
    [_listViewShell setRowHeight:itemHeight];
}

#pragma mark - 公开函数: <设置可选择项信息> <清空可选项> <设置右侧图标>
// 设置可选择项信息
- (void) setItemKey:(NSArray*)key Text:(NSArray*)text CurrentKey:(NSString* _Nullable)currentKey{
    if (key.count == 0 || key.count != text.count) {
        return;
    }
    
    _itemKeyArray           = [key mutableCopy];
    _itemTextArray          = [text mutableCopy];
    _listViewShell.data   = _itemTextArray;
    
    if ([_itemKeyArray containsObject:currentKey]) {
        _currentKey = currentKey;
    }
}
// 清空可选项
- (void) removeAllItem{
    [_itemKeyArray removeAllObjects];
    [_itemTextArray removeAllObjects];
    _listViewShell.data   = nil;
    _currentKey             = nil;
    _textField.text         = nil;
}
// 设置不同状态下的右侧图标
- (void) setIndicatorImage:(UIImage*)image WithState:(XXcomboBoxState)state{
    [_state2IndicatorImage setObject:image forKey:[self stringFromState:state]];
    if (state == XXcomboBoxState_OnPackup) {
        [_imageIndicator setImage:image];
    }
}

#pragma mark - 代理函数: <UITextFieldDelegate>
- (BOOL)textFieldShouldBeginEditing:(UITextField *)textField{
    if([_maskView isHidden]){
        [self popup];
    }
    else{
        [self packup];
    }
    return NO;
}

#pragma mark - 私有函数: <更新弹出框位置> <更新指示器位置> <弹出> <收起>
- (void) updatePopupFrame{
    if (nil != _textField) {
        CGRect textFieldRelativeRect = _textField.frame;
        CGRect textFieldAbsolutelyRect = [_textField.superview convertRect:textFieldRelativeRect toView:_rootView];
        
        textFieldAbsolutelyRect.origin.y += textFieldAbsolutelyRect.size.height;
        textFieldAbsolutelyRect.size.height = _popupHeight;
        _tableView.frame = textFieldAbsolutelyRect;
    }
}
- (void) popup{
    if (nil != [_state2IndicatorImage objectForKey:[self stringFromState:XXcomboBoxState_OnPopup]]) {
        [_imageIndicator setImage:[_state2IndicatorImage objectForKey:[self stringFromState:XXcomboBoxState_OnPopup]]];
    }
    
    [self updatePopupFrame];
    [_maskView show];
}
- (void) packup{
    if (nil != [_state2IndicatorImage objectForKey:[self stringFromState:XXcomboBoxState_OnPackup]]) {
        [_imageIndicator setImage:[_state2IndicatorImage objectForKey:[self stringFromState:XXcomboBoxState_OnPackup]]];
    }
    [_maskView hide];
}
- (NSString*) stringFromState:(XXcomboBoxState)state{
    return [NSString stringWithFormat:@"%lu", (unsigned long)state];
}

#pragma mark - 私有函数: <横竖屏通知处理>
- (void)application_onStatusBarOrientationChanged:(NSNotification *)notification{
    XXOC_WS;
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [ws updatePopupFrame];
    });
}
@end
