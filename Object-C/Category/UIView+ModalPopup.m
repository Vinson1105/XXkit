//
//  UIView+ModalPopup.m
//  XXkitProject
//
//  Created by 郭文轩 on 2020/11/16.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "UIView+ModalPopup.h"
#import "../View/XXmodalView.h"
#import <objc/runtime.h>
#import "../XXocUtils.h"

@interface UIView()
@property (nonatomic,strong) XXmodalView *modalPopup_modalView;
@end

@implementation UIView(ModalPopup)
- (BOOL)modalPopup_popup{
    return self.modalPopup_modalView.popup;
}
- (void)setModalPopup_popup:(BOOL)modalPopup_popup{
    self.modalPopup_modalView.popup = modalPopup_popup;
}
- (BOOL)modalPopup_touchBackgroundToPopdown{
    return self.modalPopup_modalView.touchToPopdown;
}
- (void)setModalPopup_touchBackgroundToPopdown:(BOOL)modalPopup_touchBackgroundToPopdown{
    self.modalPopup_modalView.touchToPopdown = modalPopup_touchBackgroundToPopdown;
}
- (BOOL)modalPopup_backgroundColorTransparent{
    return self.modalPopup_modalView.backgroundColorTransparent;
}
- (void)setModalPopup_backgroundColorTransparent:(BOOL)modalPopup_backgroundColorTransparent{
    self.modalPopup_modalView.backgroundColorTransparent = modalPopup_backgroundColorTransparent;
}
- (UIView *)modalPopup_backgroundView{
    return self.modalPopup_modalView;
}

- (void)setModalPopup_blockWhenWillUp:(void (^)(void))modalPopup_blockWhenWillUp{
    self.modalPopup_modalView.blockWhenWillUp = modalPopup_blockWhenWillUp;
}
- (void (^)(void))modalPopup_blockWhenWillUp{
    return self.modalPopup_modalView.blockWhenWillUp;
}
- (void)setModalPopup_blockWhenUping:(void (^)(void))modalPopup_blockWhenUping{
    self.modalPopup_modalView.blockWhenUping = modalPopup_blockWhenUping;
}
- (void (^)(void))modalPopup_blockWhenUping{
    return self.modalPopup_modalView.blockWhenUping;
}
- (void)setModalPopup_blockWhenDidUpFinished:(void (^)(void))modalPopup_blockWhenDidUpFinished{
    self.modalPopup_modalView.blockWhenDidUpFinished = modalPopup_blockWhenDidUpFinished;
}
- (void (^)(void))modalPopup_blockWhenDidUpFinished{
    return self.modalPopup_modalView.blockWhenDidUpFinished;
}

- (void)setModalPopup_blockWhenWillDown:(void (^)(void))modalPopup_blockWhenWillDown{
    self.modalPopup_modalView.blockWhenWillDown = modalPopup_blockWhenWillDown;
}
- (void (^)(void))modalPopup_blockWhenWillDown{
    return self.modalPopup_modalView.blockWhenWillDown;
}
- (void)setModalPopup_blockWhenDowning:(void (^)(void))modalPopup_blockWhenDowning{
    self.modalPopup_modalView.blockWhenDowning = modalPopup_blockWhenDowning;
}
- (void (^)(void))modalPopup_blockWhenDowning{
    return self.modalPopup_modalView.blockWhenDowning;
}
- (void)setModalPopup_blockWhenDidDownFinished:(void (^)(void))modalPopup_blockWhenDidDownFinished{
    self.modalPopup_modalView.blockWhenDidDownFinished = modalPopup_blockWhenDidDownFinished;
}
- (void (^)(void))modalPopup_blockWhenDidDownFinished{
    return self.modalPopup_modalView.blockWhenDidDownFinished;
}

- (XXmodalView *)modalPopup_modalView{
    XXmodalView *view = objc_getAssociatedObject(self, "modalPopup_modalView");
    if(nil == view){
        view = [XXmodalView new];
        objc_setAssociatedObject(self, "modalPopup_modalView", view, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return view;
}

-(void)modalPopup_configPopupConstraint:(NSArray*)popup popdownConstraint:(NSArray*)popdown{
    XXOC_WS;
    [self.modalPopup_modalView configContentView:ws popupConstraint:popup popdownConstraint:popdown];
}
-(void)modalPopup_configAtCenterSize:(CGSize)size margin:(CGFloat)margin{
    XXOC_WS;
    [self.modalPopup_modalView configContentViewAtCenter:ws size:size margin:margin];
}
-(void)modalPopup_configAtBottomSize:(CGSize)size margin:(CGFloat)margin{
    XXOC_WS;
    [self.modalPopup_modalView configContentViewAtBottom:ws size:size margin:margin];
}
- (void)modalPopup_release{
    // MARK: 必须执行，否则会循环引用
    [self.modalPopup_modalView removeFromSuperview];
    [self removeFromSuperview];
    objc_setAssociatedObject(self, "modalPopup_modalView", nil, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
@end
