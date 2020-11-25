//
//  XXmodalView.m
//  teset
//
//  Created by VINSON on 2020/9/29.
//  Copyright © 2020 XX. All rights reserved.
//

#import "XXmodalView.h"
#import <objc/runtime.h>

#define kAnimateDuration 0.3

@interface XXmodalView()
@property (nonatomic,strong) NSArray *popupConstraint;
@property (nonatomic,strong) NSArray *popdownConstraint;
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
        _contentView = nil;
    }
    if(nil == contentView){
        return;
    }
    if(self.contentView == contentView){
        [NSLayoutConstraint deactivateConstraints:self.popupConstraint];
        [NSLayoutConstraint deactivateConstraints:self.popdownConstraint];
        [self removeConstraints:self.popupConstraint];
        [self removeConstraints:self.popdownConstraint];
    }
    
    _contentView = contentView;
    contentView.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:contentView];
    
    self.popupConstraint = [popup copy];
    self.popdownConstraint = [popdown copy];
    
    [NSLayoutConstraint activateConstraints:self.popdownConstraint];
    if(self.popup){
        [NSLayoutConstraint deactivateConstraints:self.popdownConstraint];
        [NSLayoutConstraint activateConstraints:self.popupConstraint];
        
        [UIView animateWithDuration:0.3 animations:^{
            [self layoutIfNeeded];
        }];
    }
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
        
        // popup constraint
        NSMutableArray *popupConstraint = [[NSMutableArray alloc] initWithArray:@[
            [contentView.centerXAnchor constraintEqualToAnchor:self.centerXAnchor],
            [contentView.centerYAnchor constraintEqualToAnchor:self.centerYAnchor],
        ]];
        [popupConstraint addObjectsFromArray:commonConstraint];
        
        // popdown constraint
        NSMutableArray *popdownConstraint = [[NSMutableArray alloc] initWithArray:@[
            [contentView.centerXAnchor constraintEqualToAnchor:self.centerXAnchor],
            [contentView.topAnchor constraintEqualToAnchor:self.bottomAnchor constant:20],
        ]];
        [popupConstraint addObjectsFromArray:commonConstraint];
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
        NSMutableArray *popupConstraint = [[NSMutableArray alloc] initWithArray:@[
            [contentView.bottomAnchor constraintEqualToAnchor:self.bottomAnchor constant:-margin],
        ]];
        [popupConstraint addObjectsFromArray:commonConstraint];
        
        // popdown constraint
        NSMutableArray *popdownConstraint = [[NSMutableArray alloc] initWithArray:@[
            [contentView.topAnchor constraintEqualToAnchor:self.bottomAnchor constant:margin],
        ]];
        [popdownConstraint addObjectsFromArray:commonConstraint];
        
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
    
    [self willPopup];
    [UIView animateWithDuration:kAnimateDuration animations:^{
        if(!self.backgroundColorTransparent){
            self.backgroundColor = [UIColor colorWithWhite:0 alpha:0.5];
        }
        [self popuping];
    }];
}
-(void)popdownFromFirstWindow{
    [self willPopdown];
    [UIView animateWithDuration:kAnimateDuration animations:^{
        if(!self.backgroundColorTransparent){
            self.backgroundColor = [UIColor colorWithWhite:0 alpha:0];
        }
        [self popdowning];
    } completion:^(BOOL finished) {
        if(finished)
            [self removeFromSuperview];
    }];
}

-(void)willPopup{
    if(self.popdownConstraint && self.popupConstraint){
        [NSLayoutConstraint deactivateConstraints:self.popdownConstraint];
        [NSLayoutConstraint activateConstraints:self.popupConstraint];
    }
}
-(void)popuping{
    if(self.popdownConstraint && self.popupConstraint){
        [self layoutIfNeeded];
    }
}
-(void)willPopdown{
    if(self.popdownConstraint && self.popupConstraint){
        [NSLayoutConstraint deactivateConstraints:self.popupConstraint];
        [NSLayoutConstraint activateConstraints:self.popdownConstraint];
    }
}
-(void)popdowning{
    if(self.popdownConstraint && self.popupConstraint){
        [self layoutIfNeeded];
    }
}

-(void)onTapGesture:(UITapGestureRecognizer*)tap{
    CGPoint point = [tap locationInView:self];
    if(self.contentView && !CGRectContainsPoint(self.contentView.frame, point)){
//        [self popdownFromFirstWindon];
        self.popup = NO;
    }
}
@end
