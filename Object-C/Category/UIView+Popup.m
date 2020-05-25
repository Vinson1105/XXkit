#import "UIView+Popup.h"
#import "../XXocUtils.h"
#import <objc/runtime.h>

#define kXXpopupConstraint "xx_popupConstraint"
#define kXXpopdownConstraint "xx_popdownConstraint"
#define kXXpopup "xx_popup"

@implementation UIView(Popup)
- (void)setXx_popupConstraint:(NSArray *)popupConstraint{
    objc_setAssociatedObject(self, kXXpopupConstraint, popupConstraint, OBJC_ASSOCIATION_RETAIN);
}
- (NSArray *)xx_popupConstraint{
    return objc_getAssociatedObject(self, kXXpopupConstraint);
}
- (void)setXx_popdownConstraint:(NSArray *)popdownConstraint{
    objc_setAssociatedObject(self, kXXpopdownConstraint, popdownConstraint, OBJC_ASSOCIATION_RETAIN);
}
- (NSArray *)xx_popdownConstraint{
    return objc_getAssociatedObject(self, kXXpopdownConstraint);
}
- (void)setXx_popup:(BOOL)popup{
    if(self.xx_popup == popup) return;
    objc_setAssociatedObject(self, kXXpopup, @(popup), OBJC_ASSOCIATION_COPY);
    [self toPopup:popup];
}
- (BOOL)xx_popup{
    id value = objc_getAssociatedObject(self, kXXpopup);
    return nil == value ? NO : [value boolValue];
}

- (void)xx_initPopup:(BOOL)popup{
    objc_setAssociatedObject(self, kXXpopup, @(popup), OBJC_ASSOCIATION_COPY);
}

- (void)toPopup:(BOOL)popup{
    NSArray *deactive   = popup ? self.xx_popdownConstraint : self.xx_popupConstraint;
    NSArray *active     = popup ? self.xx_popupConstraint : self.xx_popdownConstraint;
    
    __weak typeof(self) weakSelf = self;
    [XXocUtils mainThreadProcess:^{
        [NSLayoutConstraint deactivateConstraints:deactive];
        [NSLayoutConstraint activateConstraints:active];
        [UIView animateWithDuration:0.3 animations:^{
            __strong typeof(weakSelf) strongSelf = weakSelf;
            [strongSelf.superview layoutIfNeeded];
        }];
    }];
}
@end
