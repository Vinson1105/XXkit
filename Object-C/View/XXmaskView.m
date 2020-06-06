//
//  XXMaskView.m
//
//
//  Created by VINSON on 2018/11/16.
//  Copyright © 2018 Macrovideo. All rights reserved.
//

#import "XXmaskView.h"
#import "../XXocUtils.h"

typedef BOOL (^WillHideBlock)(void);      // 隐藏事件处理block类型

static XXmaskView *_instance = nil;


@interface XXmaskView()
@property (nonatomic,weak) UIView *content;
@property (nonatomic,strong) UITapGestureRecognizer *tapGestureRecognizer;
@end

@implementation XXmaskView
+ (XXmaskView*)sharedInstance{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instance = [XXmaskView new];
    });
    return _instance;
}
- (instancetype)init{
    self = [super init];
    if (self) {
        self.backgroundColor = [UIColor colorWithWhite:0 alpha:0.5];
        self.translatesAutoresizingMaskIntoConstraints = NO;
        self.tapGestureRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onTap:)];
        self.tapGestureRecognizer.numberOfTouchesRequired = 1;
        self.tapGestureRecognizer.numberOfTapsRequired = 1;
        self.tapGestureRecognizer.delegate = self;
        [self addGestureRecognizer:self.tapGestureRecognizer];
        //self.userInteractionEnabled = YES;
    }
    return self;
}
- (void)show:(UIView*)view animated:(BOOL)animated{
    if(_content){
        [_content removeFromSuperview];
        _content = nil;
    }
    _content = view;
    _content.translatesAutoresizingMaskIntoConstraints = NO;
    
    [self addSubview:view];
    [XXocUtils view:view centerAt:self];
     
    UIWindow *window = [UIApplication sharedApplication].delegate.window;
    [window addSubview:self];
    [XXocUtils view:self margin:0 fillAt:window];
    
    if(animated){
        [UIView animateWithDuration:3 animations:^{
            self.hidden = NO;
        }];
    }
    else{
        self.hidden = NO;
    }
}
- (void)setHidden:(BOOL)hidden{
    [super setHidden:hidden];
    if(hidden && _content){
        [_content removeFromSuperview];
        _content = nil;
    }
}

#pragma mark - <手势>
-(void)onTap:(UITapGestureRecognizer*)recognizer{
    self.hidden = YES;
}

#pragma mark - <UIGestureRecognizerDelegate>
- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch{
    if (_content && [touch.view isDescendantOfView:_content]) {
        return NO;
    }
    return YES;
}
@end
