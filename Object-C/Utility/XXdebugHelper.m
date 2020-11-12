//
//  XXdebugHelper.m
//  XXkitProject
//
//  Created by VINSON on 2020/11/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "XXdebugHelper.h"
#import "../XXocUtils.h"
#import "../Shell/XXtableViewShell.h"
#import "../Category/UIView+Touchable.m"
#import "../Category/UIView+Movable.h"

static XXdebugHelper *_instance = nil;

// MARK: 调试器数据模型
@interface DebugSettingModel : NSObject
@end
@implementation DebugSettingModel
@end

// MARK: 调试器设置页面
@interface DebugSettingView : UIView
@property (nonatomic,weak) UIWindow *window;
@property (nonatomic,strong) UITableView *tableView;
@property (nonatomic,strong) UIButton *okButton;
@property (nonatomic,strong) UIButton *retButton;

@property (nonatomic,assign,readonly) BOOL popup;
@property (nonatomic,assign) CGRect popupFromRect;
@end
@implementation DebugSettingView
- (instancetype)init{
    self = [super init];
    if (self) {
        _popup = NO;
        self.backgroundColor = UIColor.whiteColor;
        
        // table
        
        // ok
        self.okButton = [UIButton buttonWithType:UIButtonTypeCustom];
        self.okButton.translatesAutoresizingMaskIntoConstraints = NO;
        [self.okButton setTitle:@"OK" forState:UIControlStateNormal];
        [self.okButton setTitleColor:UIColor.blueColor forState:UIControlStateNormal];
        [self addSubview:self.okButton];
        [self.okButton.leadingAnchor constraintEqualToAnchor:self.leadingAnchor].active = YES;
        [self.okButton.bottomAnchor constraintEqualToAnchor:self.bottomAnchor].active = YES;
        [self.okButton.widthAnchor constraintEqualToAnchor:self.widthAnchor multiplier:0.5].active = YES;
        [self.okButton.heightAnchor constraintEqualToConstant:45].active = YES;
        
        // cancel
        self.retButton = [UIButton buttonWithType:UIButtonTypeCustom];
        self.retButton.translatesAutoresizingMaskIntoConstraints = NO;
        [self.retButton setTitle:@"Cancel" forState:UIControlStateNormal];
        [self.retButton setTitleColor:UIColor.grayColor forState:UIControlStateNormal];
        [self.okButton.trailingAnchor constraintEqualToAnchor:self.trailingAnchor].active = YES;
        [self.okButton.bottomAnchor constraintEqualToAnchor:self.bottomAnchor].active = YES;
        [self.okButton.widthAnchor constraintEqualToAnchor:self.widthAnchor multiplier:0.5].active = YES;
        [self.okButton.heightAnchor constraintEqualToConstant:45].active = YES;
    }
    return self;
}
-(void)popupFromRect:(CGRect)rect{
    if(self.popup) return;
    _popup = YES;
    
    self.popupFromRect = rect;
    self.frame = rect;
    self.alpha = 0;
    [self.window addSubview:self];
    [self.window bringSubviewToFront:self];
    
    CGRect endRect = self.window.bounds;
    [UIView animateWithDuration:0.2 animations:^{
        self.alpha = 1;
        self.frame = endRect;
    }];
}
-(void)popdown{
    if(!self.popup) return;
    _popup = NO;
    
    [UIView animateWithDuration:0.2 animations:^{
        self.alpha = 0;
        self.frame = self.popupFromRect;
        } completion:^(BOOL finished) {
            [self removeFromSuperview];
        }];
}
- (void)setWindow:(UIWindow *)window{
    if(window == _window) return;
    _window = window;
    if(self.popup){
        if(window){
            self.frame = window.bounds;
            [window addSubview:self];
            [window bringSubviewToFront:self];
        }
        else{
            _popup = NO;
            [self removeFromSuperview];
        }
    }
}
@end

// MARK: 调试器主控
@interface XXdebugHelper()
@property (nonatomic,weak) UIWindow *window;
@property (nonatomic,strong) UIView *indicaterView;
@property (nonatomic,strong) DebugSettingView *settingView;
//@property (nonatomic,strong) XXviewMovableShell *viewMovableShell;
@end

@implementation XXdebugHelper
+(XXdebugHelper*)sharedInstance{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instance = [XXdebugHelper new];
    });
    return _instance;
}
+(void)install:(UIWindow*)window{
    [XXdebugHelper sharedInstance].window = window;
}

- (instancetype)init{
    self = [super init];
    if (self) {
        XXOC_WS;
        
        // Indicater小组件
        self.indicaterView = [[UIView alloc] initWithFrame:CGRectMake(20, 80, 50, 50)];
        self.indicaterView.translatesAutoresizingMaskIntoConstraints = NO;
        self.indicaterView.backgroundColor = [UIColor colorWithRed:0.3 green:0.3 blue:0.3 alpha:0.5];
        self.indicaterView.layer.cornerRadius = 5;
        [self.indicaterView touchable_install:^{
            XXOC_SS;
            [ss onIndicaterViewTouched];
        }];
        [self.indicaterView movable_install:20];        
        
        // SettingView
        self.settingView = [DebugSettingView new];
    }
    return self;
}
- (void)setWindow:(UIWindow *)window{
    if(window == _window) return;
    _window = window;
    if(_window){
        [_window addSubview:self.indicaterView];
    }
    else{
        [self.indicaterView removeFromSuperview];
    }
    self.settingView.window = window;
}
- (void)onIndicaterViewTouched{
    [self.settingView popupFromRect:self.indicaterView.frame];
}
@end
