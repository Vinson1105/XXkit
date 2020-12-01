//
//  XXmodalView.m
//  teset
//
//  Created by VINSON on 2020/9/29.
//  Copyright © 2020 XX. All rights reserved.
//

#import "XXmodalView.h"
#import <objc/runtime.h>
#import "../Category/UIView+Popup.h"
#import "../XXocUtils.h"

#define kAnimateDuration 0.3

@interface XXmodalView()
//@property (nonatomic,strong) NSArray *popupConstraint;
//@property (nonatomic,strong) NSArray *popdownConstraint;
@property (nonatomic,strong) UITapGestureRecognizer *tapGesture;
@end

@implementation XXmodalView
- (instancetype)init{
    self = [super init];
    if (self) {
        self.translatesAutoresizingMaskIntoConstraints = NO;
        self.backgroundColor = [UIColor colorWithWhite:0 alpha:0.0];
    }
    return self;
}
- (void)setPopup:(BOOL)popup{
    if(_popup == popup){
        return;
    }
    _popup = popup;
    if(popup){
        [self popupAtFirstWindow];
    }
    else{
        [self popdownFromFirstWindow];
    }
}
- (void)setTouchToPopdown:(BOOL)touchToPopdown{
    if(touchToPopdown==_touchToPopdown){
        return;
    }
    _touchToPopdown = touchToPopdown;
    if(touchToPopdown){
        if(nil==self.tapGesture){
            self.tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onTapGesture:)];
            self.tapGesture.numberOfTapsRequired = 1;
            self.tapGesture.numberOfTouchesRequired = 1;
        }
        [self addGestureRecognizer:self.tapGesture];
    }
    else{
        if(self.tapGesture){
            [self removeGestureRecognizer:self.tapGesture];
        }
    }
}

-(void)configContentView:(UIView*)contentView popupConstraint:(NSArray*)popup popdownConstraint:(NSArray*)popdown{
    if(contentView!=self.contentView && self.contentView && self.contentView.superview == self){
        [self.contentView removeFromSuperview];
        self.contentView.popup_active = NO;
        _contentView = nil;
    }
    if(nil == contentView){
        return;
    }
    if(self.contentView == contentView){
        [NSLayoutConstraint deactivateConstraints:self.contentView.popup_upLayoutConstraints];
        [NSLayoutConstraint deactivateConstraints:self.contentView.popup_downLayoutConstraints];
        [self removeConstraints:self.contentView.popup_upLayoutConstraints];
        [self removeConstraints:self.contentView.popup_downLayoutConstraints];
    }
    
    _contentView = contentView;
    contentView.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:contentView];
    
    self.contentView.popup_upLayoutConstraints = [popup copy];
    self.contentView.popup_downLayoutConstraints = [popdown copy];
    self.contentView.popup_active = YES;
    
    [NSLayoutConstraint activateConstraints:self.self.contentView.popup_downLayoutConstraints];
    if(self.popup){
        [NSLayoutConstraint deactivateConstraints:self.contentView.popup_downLayoutConstraints];
        [NSLayoutConstraint activateConstraints:self.contentView.popup_downLayoutConstraints];
        
        [UIView animateWithDuration:0.3 animations:^{
            [self layoutIfNeeded];
        }];
    }
    
    XXOC_WS;
    self.contentView.popup_blockWhenWillUp = ^{
        XXOC_SS;
        if(ss.blockWhenWillUp){
            ss.blockWhenWillUp();
        }
    };
    self.contentView.popup_blockWhenUping = ^{
        XXOC_SS;
        if(!ss.backgroundColorTransparent){
            ss.backgroundColor = [UIColor colorWithWhite:0 alpha:0.5];
        }
        if(ss.blockWhenUping){
            ss.blockWhenUping();
        }
    };
    self.contentView.popup_blockWhenDidUpFinished = ^{
        XXOC_SS;
        if(ss.blockWhenDidUpFinished){
            ss.blockWhenDidUpFinished();
        }
    };
    
    self.contentView.popup_blockWhenWillDown = ^{
        XXOC_SS;
        if(ss.blockWhenWillDown){
            ss.blockWhenWillDown();
        }
    };
    self.contentView.popup_blockWhenDowning = ^{
        XXOC_SS;
        if(!ss.backgroundColorTransparent){
            ss.backgroundColor = [UIColor colorWithWhite:0 alpha:0];
        }
        if(ss.blockWhenDowning){
            ss.blockWhenDowning();
        }
    };
    self.contentView.popup_blockWhenDidDownFinished = ^{
        XXOC_SS;
        [ss removeFromSuperview];
        if(ss.blockWhenDidDownFinished){
            ss.blockWhenDidDownFinished();
        }
    };
}
-(void)configContentViewAtCenter:(UIView*)contentView size:(CGSize)size margin:(CGFloat)margin{
    if(nil == contentView){
        [self configContentView:contentView popupConstraint:@[] popdownConstraint:@[]];
    }
    else{
        // common constraint
        NSMutableArray *commonConstraint = [NSMutableArray new];
        if(size.width>0){
            [commonConstraint addObject:[contentView.widthAnchor constraintEqualToConstant:size.width]];
        }
        if(size.height>0){
            [commonConstraint addObject:[contentView.heightAnchor constraintEqualToConstant:size.height]];
        }
        if(margin>0){
            [commonConstraint addObject:[contentView.leadingAnchor constraintEqualToAnchor:self.leadingAnchor constant:margin]];
            [commonConstraint addObject:[contentView.trailingAnchor constraintEqualToAnchor:self.trailingAnchor constant:-margin]];
        }
        [commonConstraint addObject:[contentView.centerXAnchor constraintEqualToAnchor:self.centerXAnchor]];
        
        // popup constraint
        NSMutableArray *popupConstraint = [[NSMutableArray alloc] initWithArray:commonConstraint];
        [popupConstraint addObject:[contentView.centerYAnchor constraintEqualToAnchor:self.centerYAnchor]];
        
        // popdown constraint
        NSMutableArray *popdownConstraint = [[NSMutableArray alloc] initWithArray:commonConstraint];
        [popdownConstraint addObject:[contentView.topAnchor constraintEqualToAnchor:self.bottomAnchor constant:20]];
        [self configContentView:contentView popupConstraint:popupConstraint popdownConstraint:popdownConstraint];
    }
}
-(void)configContentViewAtBottom:(UIView*)contentView size:(CGSize)size margin:(CGFloat)margin{
    if(nil == contentView){
        [self configContentView:contentView popupConstraint:@[] popdownConstraint:@[]];
    }
    else{
        // common constraint
        NSMutableArray *commonConstraint = [NSMutableArray new];
        if(size.width>0){
            [commonConstraint addObject:[contentView.widthAnchor constraintEqualToConstant:size.width]];
        }
        else{
            [commonConstraint addObject:[contentView.leadingAnchor constraintEqualToAnchor:self.leadingAnchor constant:margin]];
            [commonConstraint addObject:[contentView.trailingAnchor constraintEqualToAnchor:self.trailingAnchor constant:-margin]];
        }
        if(size.height>0){
            [commonConstraint addObject:[contentView.heightAnchor constraintEqualToConstant:size.height]];
        }
        [commonConstraint addObject:[contentView.centerXAnchor constraintEqualToAnchor:self.centerXAnchor]];
        
        // popup constraint
        NSMutableArray *popupConstraint = [[NSMutableArray alloc] initWithArray:commonConstraint];
        if(@available(iOS 11.0, *)){
            [popupConstraint addObject:[contentView.bottomAnchor constraintEqualToAnchor:self.safeAreaLayoutGuide.bottomAnchor constant:-margin]];
        }
        else{
            [popupConstraint addObject:[contentView.bottomAnchor constraintEqualToAnchor:self.bottomAnchor constant:-margin]];
        }
        
        // popdown constraint
        NSMutableArray *popdownConstraint = [[NSMutableArray alloc] initWithArray:commonConstraint];
        [popdownConstraint addObject:[contentView.topAnchor constraintEqualToAnchor:self.bottomAnchor constant:margin]];
        
        [self configContentView:contentView popupConstraint:popupConstraint popdownConstraint:popdownConstraint];
    }
}

-(void)popupAtFirstWindow{
    UIWindow *window = [UIApplication sharedApplication].windows.firstObject;
    [window addSubview:self];
    [self.topAnchor constraintEqualToAnchor:window.topAnchor].active = YES;
    [self.leadingAnchor constraintEqualToAnchor:window.leadingAnchor].active = YES;
    [self.bottomAnchor constraintEqualToAnchor:window.bottomAnchor].active = YES;
    [self.trailingAnchor constraintEqualToAnchor:window.trailingAnchor].active = YES;
    self.backgroundColor = [UIColor colorWithWhite:0 alpha:0.0];
    [window layoutIfNeeded];
    self.contentView.popup_up = YES;
}
-(void)popdownFromFirstWindow{
    self.contentView.popup_up = NO;
}

-(void)onTapGesture:(UITapGestureRecognizer*)tap{
    CGPoint point = [tap locationInView:self];
    if(self.contentView && !CGRectContainsPoint(self.contentView.frame, point)){
//        [self popdownFromFirstWindon];
        self.popup = NO;
    }
}
@end
