#import "XXcomboBox.h"
#import "XXlistViewShell.h"
#import "XXmaskView.h"
#import "XXshadowCreator.h"

@interface XXcomboBox()
/** textField所在的页面的顶层view,一般是对应页的UIViewController.view */
@property (weak,nonatomic) UIView *rootView;
/** 对应的textField控件 */
@property (weak,nonatomic) UITextField *textField;
/** 弹出的tableView弹框 */
@property (strong,nonatomic) UITableView *tableView;

/** tableView的列表组件 */
@property (strong,nonatomic) XXlistViewShell *listViewShell;
/** 弹出框的蒙层 */
@property (strong,nonatomic) XXmaskView *maskView;

/** 弹出框的高度 */
@property (assign,nonatomic) NSUInteger popupHeight;
/** 弹出框选项关键字列表 */
@property (strong,nonatomic) NSMutableArray *itemKeyArray;
/** 弹出框选项文本列表 */
@property (strong,nonatomic) NSMutableArray *itemTextArray;

/** 右边栏指示器 */
@property (strong,nonatomic) UIImageView *imageIndicator;
@property (assign,nonatomic) CGFloat indicatorMargin;
/** 不同的状态对应的右边栏指示器图标 */
@property (strong,nonatomic) NSMutableDictionary *state2IndicatorImage;
@end

@implementation XXcomboBox

#pragma mark - 初始化
- (instancetype) initWithTextField:(UITextField*) textField RootView:(UIView*)rootView{
    self = [super init];
    
    if (self) {
        __weak XXcomboBox *weakSelf = self;
        // [1] 初始化textField
        self.textField = textField;
        self.textField.delegate = self;
        
        // [2] 保存根view
        self.rootView = rootView;
        
        // [3] 初始化tableView
        self.tableView = [[UITableView alloc] init];
        self.tableView.backgroundColor = [UIColor whiteColor];
        self.tableView.bounces = NO;
        self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
        self.tableView.layer.cornerRadius = 10;
        self.tableView.layer.masksToBounds = YES;
        
        // [4] 初始化tableView的列表组件
        self.listViewShell = [[XXlistViewShell alloc] initWithContext:self.tableView];
        [self.listViewShell setRowTouchBlock:^(NSInteger row, id cellData) {
            //[weakSelf.textField setText:weakSelf.itemKeyArray[row]];
            weakSelf.currentKey = weakSelf.itemKeyArray[row];
            [weakSelf.maskView hide];
        }];
        
        // [5] 初始化蒙层
        self.maskView = [[XXmaskView alloc] initWithContent:self.tableView WillHideBlock:^BOOL{
            if (nil != [weakSelf.state2IndicatorImage objectForKey:[weakSelf stringFromState:XXComboBoxPackupState]]) {
                [weakSelf.imageIndicator setImage:[weakSelf.state2IndicatorImage objectForKey:[weakSelf stringFromState:XXComboBoxPackupState]]];
            }
            return YES;
        } Alpha:0.0];
        
        // [6] 添加阴影
        [XXshadowCreator makeShadowWithTarget:self.tableView ShadowBgColor:[UIColor blackColor] ShadowColor:[UIColor blackColor] ShadowOffset:CGSizeZero ShadowOpacity:0.5 ShadowRadius:5];
        
        // [7] 指示器
        self.indicatorMargin = 10.0;
        self.imageIndicator = [[UIImageView alloc] init];
        [self.textField addSubview:self.imageIndicator];
        self.imageIndicator.contentMode = UIViewContentModeCenter;
        self.state2IndicatorImage = [NSMutableDictionary dictionary];
        [self updateIndicatorFrame];
        
        // [8] 监听横竖屏
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(application_onStatusBarOrientationChanged:) name:UIApplicationDidChangeStatusBarOrientationNotification object:nil];
    }
    return self;
}

#pragma mark - 公开属性的get/set重载
- (void) setCurrentKey:(NSString *)currentKey{
    if (nil == currentKey) {
        self.textField.text = @"";
        _currentKey = @"";
        return;
    }
    
    if ([self.itemKeyArray containsObject:currentKey]) {
        NSUInteger index = [self.itemKeyArray indexOfObject:currentKey];
        self.textField.text = [self.itemTextArray objectAtIndex:index];
        _currentKey = currentKey;
    }
}

#pragma mark - 公开方法: <弹出框的高度> <可选择项信息> <设置右侧图标>
// 设置弹框高度
- (void) setPopupHeight:(NSUInteger)popupHeight ItemHeight:(NSUInteger)itemHeight{
    _popupHeight = popupHeight;
    [self.listViewShell setRowHeight:itemHeight];
}
// 设置可选择项信息
- (void) setItemKey:(NSArray*)key Text:(NSArray*)text{
    if (key.count == 0 || key.count != text.count) {
        return;
    }
    
    self.itemKeyArray = [key mutableCopy];
    self.itemTextArray = [text mutableCopy];
    
    [self.listViewShell setXxdata:self.itemTextArray];
}
// 设置不同状态下的右侧图标
- (void) setIndicatorImage:(UIImage*)image WithState:(XXComboBoxState)state{
    [self.state2IndicatorImage setObject:image forKey:[NSString stringWithFormat:@"%lu", (unsigned long)state]];
    if (state == XXComboBoxPackupState) {
        [self.imageIndicator setImage:image];
    }
}

#pragma mark - 代理: <UITextFieldDelegate>
- (BOOL)textFieldShouldBeginEditing:(UITextField *)textField{
    if([self.maskView isHidden]){
        if (nil != self.willPopupBlock) {
            self.willPopupBlock();
        }
        [self popup];
    }
    else{
        if (nil != self.willPackupBlock) {
            self.willPackupBlock();
        }
        [self packup];
    }
    return NO;
}

#pragma mark - 私有方法: <更新弹出框位置> <更新指示器位置> <弹出> <收起>
- (void) updatePopupFrame{
    if (nil != self.textField) {
        CGRect textFieldRelativeRect = self.textField.frame;
        CGRect textFieldAbsolutelyRect = [self.textField.superview convertRect:textFieldRelativeRect toView:self.rootView];
        
        textFieldAbsolutelyRect.origin.y += textFieldAbsolutelyRect.size.height;
        textFieldAbsolutelyRect.size.height = _popupHeight;
        self.tableView.frame = textFieldAbsolutelyRect;
    }
}
- (void) updateIndicatorFrame{
    CGRect frame        = self.textField.frame;
    frame.size.height   = frame.size.height - self.indicatorMargin * 2;
    frame.origin.x      = frame.size.width - self.indicatorMargin - frame.size.height;
    frame.origin.y      = self.indicatorMargin;
    frame.size.width    = frame.size.height;
    self.imageIndicator.frame = frame;
}
- (void) popup{
    if (nil != [self.state2IndicatorImage objectForKey:[self stringFromState:XXComboBoxPopupState]]) {
        [self.imageIndicator setImage:[self.state2IndicatorImage objectForKey:[self stringFromState:XXComboBoxPopupState]]];
    }
    
    [self updatePopupFrame];
    [self.maskView show];
}
- (void) packup{
    if (nil != [self.state2IndicatorImage objectForKey:[self stringFromState:XXComboBoxPackupState]]) {
        [self.imageIndicator setImage:[self.state2IndicatorImage objectForKey:[self stringFromState:XXComboBoxPackupState]]];
    }
    [self.maskView hide];
}
- (NSString*) stringFromState:(XXComboBoxState)state{
    return [NSString stringWithFormat:@"%lu", (unsigned long)state];
}

#pragma mark - 私有方法: <横竖屏通知处理>
- (void)application_onStatusBarOrientationChanged:(NSNotification *)notification{
    __weak typeof(self) weakSelf = self;
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [weakSelf updateIndicatorFrame];
        [weakSelf updatePopupFrame];
    });
}
@end
