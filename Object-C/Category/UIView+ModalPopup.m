//
//  UIView+ModalPopup.m
//  XXkitProject
//
//  Created by 郭文轩 on 2020/11/16.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "UIView+ModalPopup.h"
#import "UIView+Popup.h"
#import <objc/runtime.h>
#import "../XXocUtils.h"

@interface UIView()
@end

@implementation UIView(ModalPopup)
- (void)modalPopup_installWithPosition:(ModalPopupPosition)position{
    self.translatesAutoresizingMaskIntoConstraints = NO;
    self.alpha = 0.0;
    UIView *backgroundView = self.modalPopup_backgroundView;
    [backgroundView addSubview:self];   // 这里会产生循环引用，需要手动removeFromSuperview
    
    NSMutableArray *upLayout = [NSMutableArray array];
    NSMutableArray *downLayout = [NSMutableArray array];
    if(position == ModalPopupPositionCenter){
        [upLayout addObject:[self.centerXAnchor constraintEqualToAnchor:backgroundView.centerXAnchor]];
        [upLayout addObject:[self.centerYAnchor constraintEqualToAnchor:backgroundView.centerYAnchor]];
        
        [downLayout addObject:[self.centerXAnchor constraintEqualToAnchor:backgroundView.centerXAnchor]];
        [downLayout addObject:[self.topAnchor constraintEqualToAnchor:backgroundView.bottomAnchor constant:20]];
    }
    else{
        [upLayout addObject:[self.leadingAnchor constraintEqualToAnchor:backgroundView.leadingAnchor constant:20]];
        [upLayout addObject:[self.trailingAnchor constraintEqualToAnchor:backgroundView.trailingAnchor constant:-20]];
        [upLayout addObject:[self.bottomAnchor constraintEqualToAnchor:backgroundView.bottomAnchor constant:-20]];

        [downLayout addObject:[self.leadingAnchor constraintEqualToAnchor:backgroundView.leadingAnchor constant:20]];
        [downLayout addObject:[self.trailingAnchor constraintEqualToAnchor:backgroundView.trailingAnchor constant:-20]];
        [downLayout addObject:[self.topAnchor constraintEqualToAnchor:backgroundView.bottomAnchor constant:20]];
    }
    self.popup_upLayoutConstraints = upLayout;
    self.popup_downLayoutConstraints = downLayout;
    XXOC_WS;
    self.popup_blockWhenWillUp = ^{
        XXOC_SS;
        ss.modalPopup_backgroundView.frame = ss.window.bounds;
        ss.alpha = 0.0;
        [ss.window addSubview:ss.modalPopup_backgroundView];
    };
    self.popup_blockWhenUping = ^{
        XXOC_SS;
        ss.alpha = 1.0;
        ss.modalPopup_backgroundView.alpha = 1.0;
    };
    self.popup_blockWhenDowning = ^{
        XXOC_SS;
        ss.alpha = 0.0;
        ss.modalPopup_backgroundView.alpha = 0.0;
    };
    self.popup_blockWhenDidDownFinished = ^{
        XXOC_SS;
        [ss.modalPopup_backgroundView removeFromSuperview];
    };
    self.popup_active = YES;
}
- (void)modalPopup_uninstall{
    self.popup_active = NO;
    [self removeFromSuperview];
}
- (void)setModalPopup_up:(BOOL)modalPopup_up{
    self.popup_up = modalPopup_up;
}
- (BOOL)modalPopup_up{
    return self.popup_up;
}
- (UIView *)modalPopup_backgroundView{
    UIView *view = objc_getAssociatedObject(self, "modalPopup_backgroundView");
    if(nil == view){
        view = [UIView new];
        view.backgroundColor = [UIColor colorWithWhite:0 alpha:0.0];
        objc_setAssociatedObject(self, "modalPopup_backgroundView", view, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return view;
}
@end
