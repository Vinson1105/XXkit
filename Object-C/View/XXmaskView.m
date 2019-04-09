//
//  XXMaskView.m
//
//
//  Created by VINSON on 2018/11/16.
//  Copyright © 2018 Macrovideo. All rights reserved.
//

#import "XXmaskView.h"

typedef BOOL (^WillHideBlock)(void);      // 隐藏事件处理block类型

@interface XXmaskView()
/** 蒙层的父类 */
@property (weak,nonatomic) UIView *superView;
/** 蒙层自身view */
//@property (strong,nonatomic) UIView *maskView;
/** 蒙层的透明度 */
@property (nonatomic) CGFloat maskAlpha;
/** 蒙层中显示的内容 */
@property (weak,nonatomic) UIView *content;
/** 蒙层将要隐藏时执行的回调 */
@property (copy) WillHideBlock willHideBlock;
@end

@implementation XXmaskView

#pragma mark - 构建和析构
- (instancetype)init
{
    self = [super init];
    if (self) {
        // [1] 初始化蒙层,满屏,50%的透明度
        self.frame = [[UIScreen mainScreen] bounds];
        
        // [2] 把蒙层添加到应用的窗口中
        id<UIApplicationDelegate> delegate = [UIApplication sharedApplication].delegate;
        self.superView = delegate.window;
        [self.superView addSubview:self];
        [self setHidden:YES];
        
        // [3] 添加点击事件
        UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(tapAction:)];
        [self addGestureRecognizer:tap];
        tap.delegate = self;
        
        // [4] 监听横竖屏切换
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(application_onStatusBarOrientationChanged:) name:UIApplicationDidChangeStatusBarOrientationNotification object:nil];
    }
    return self;
}
// 以指定的alpha值进行初始化
- (instancetype) initWithContent:(UIView*)content WillHideBlock:(nullable BOOL(^)(void))willHideBlock Alpha:(CGFloat)alpha{
    self = [self init];
    if (self) {
        self.content = content;
        self.willHideBlock = willHideBlock;
        self.maskAlpha = alpha;
        self.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:alpha];
        [self addSubview:content];
    }
    return self;
}
// 析构
- (void)dealloc
{
    if (nil != self.superView) {
        /**
         由于蒙层是添加到应用的窗口中,在整个应用生命周期中,该窗口不会被释放
         需要在本类释放时,将蒙层从应用窗口中移除,避免不是使用单例的情况下内存泄漏
         */
        [self removeFromSuperview];
    }
}

#pragma mark - 布局


#pragma mark - 公开方法: <单例访问> <显示蒙层> <隐藏蒙层> <是否隐藏判断>
// 单例
+ (instancetype) sharedInstance{
    static XXmaskView *instance = nil;
    static dispatch_once_t onceToken;
    
    dispatch_once(&onceToken, ^{
        instance = [[self alloc] init];
    });
    
    return instance;
}
// 显示蒙层
- (void) show:(UIView*)content WillHideBlock:(BOOL(^ __nullable)(void))willHideBlock{
    if(content != self.content){
        // 移除上一个content
        if(nil != self.content){
            [self.content removeFromSuperview];
        }
        
        // 加入新的content
        [self addSubview:content];
        self.content = content;
        self.willHideBlock = willHideBlock;
    }
    [self show];
}
// 显示蒙层
- (void) show{
    if (self.maskAlpha != 0) {
        __weak typeof(self) weakSelf = self;
        weakSelf.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0];
        [UIView animateWithDuration:0.2 animations:^{
            weakSelf.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:weakSelf.maskAlpha];
        }];
    }
    
    [self setHidden:NO];
}
// 隐藏蒙层
- (void) hide{
    if(nil != self.willHideBlock && !self.willHideBlock())
        return;
    
    if (self.maskAlpha != 0) {
        __weak typeof(self) weakSelf = self;
        [UIView animateWithDuration:0.2 animations:^{
            weakSelf.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0];
        } completion:^(BOOL finished){
            [weakSelf setHidden:YES];
        }];
    }
    else{
        [self setHidden:YES];
    }
}
// 是否隐藏判断
//- (BOOL) isHidden{
//    return nil == self.maskView ? YES : self.maskView.isHidden;
//}

#pragma mark - 手势事件
- (void)tapAction:(UITapGestureRecognizer *)selfTap{
    CGPoint point = [selfTap locationInView:self];
    if (nil != self.content) {
        if(!CGRectContainsPoint(self.content.frame, point))
            [self hide];
    }
    else{
        [self hide];
    }
}

#pragma mark - 代理: <UIGestureRecognizerDelegate>
-(BOOL)gestureRecognizer:(UIGestureRecognizer*)gestureRecognizer shouldReceiveTouch:(UITouch*)touch{
    CGPoint point = [touch locationInView:self.superView];
    if (nil != self.content && CGRectContainsPoint(self.content.frame, point)) {
        return NO;
    }
    return YES;
}

#pragma mark - 通知处理: <横竖屏通知>
- (void)application_onStatusBarOrientationChanged:(NSNotification *)notification{
    self.frame = [[UIScreen mainScreen] bounds];
}
@end
