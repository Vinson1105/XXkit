//
//  UIView+Popup.m
//  iCamSee
//
//  Created by VINSON on 2020/3/23.
//  Copyright © 2020 Macrovideo. All rights reserved.
//

#import "UIView+Popup.h"
#import <objc/runtime.h>

#define kXXlayoutForPopup "XXlayoutForPopup"
#define kXXlayoutForPopdown "XXlayoutForPopdown"
#define kXXpopup "XXpopup"

@implementation UIView(Popup)
- (void)setLayoutForPopup:(NSArray *)layoutForPopup{
    objc_setAssociatedObject(self, kXXlayoutForPopup, layoutForPopup, OBJC_ASSOCIATION_RETAIN);
}
- (NSArray *)layoutForPopup{
    return objc_getAssociatedObject(self, kXXlayoutForPopup);
}
- (void)setLayoutForPopdown:(NSArray *)layoutForPopdown{
    objc_setAssociatedObject(self, kXXlayoutForPopdown, layoutForPopdown, OBJC_ASSOCIATION_RETAIN);
}
- (NSArray *)layoutForPopdown{
    return objc_getAssociatedObject(self, kXXlayoutForPopdown);
}
- (void)setPopup:(BOOL)popup{
    BOOL last = self.popup;
    if(last == popup) return;
    objc_setAssociatedObject(self, kXXpopup, @(popup), OBJC_ASSOCIATION_COPY);
    
    NSArray *deactive = popup ? self.layoutForPopdown : self.layoutForPopup;
    NSArray *active = popup ? self.layoutForPopup : self.layoutForPopdown;
    
    __weak typeof(self) weakSelf = self;
    if([NSThread currentThread].isMainThread){
        [NSLayoutConstraint deactivateConstraints:deactive];
        [NSLayoutConstraint activateConstraints:active];
        [UIView animateWithDuration:0.3 animations:^{
            [weakSelf.superview layoutIfNeeded];
        }];
    }
    else{
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSLayoutConstraint deactivateConstraints:deactive];
            [NSLayoutConstraint activateConstraints:active];
            [UIView animateWithDuration:0.3 animations:^{
                __strong typeof(weakSelf) strongSelf = weakSelf;
                [strongSelf.superview layoutIfNeeded];
            }];
        });
    }
}
- (BOOL)popup{
    id value = objc_getAssociatedObject(self, kXXpopup);
    return nil == value ? NO : [value boolValue];
}
@end
