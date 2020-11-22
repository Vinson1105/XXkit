#import "UIView+Popup.h"
#import "../XXocUtils.h"
#import <objc/runtime.h>

@implementation UIView(Popup)
- (void)setPopup_upLayoutConstraints:(NSArray *)constraints{
    objc_setAssociatedObject(self, "popup_upLayoutConstraints", constraints, OBJC_ASSOCIATION_RETAIN);
}
- (NSArray *)popup_upLayoutConstraints{
    return objc_getAssociatedObject(self, "popup_upLayoutConstraints");
}
- (void)setPopup_downLayoutConstraints:(NSArray *)constraints{
    objc_setAssociatedObject(self, "popup_downLayoutConstraints", constraints, OBJC_ASSOCIATION_RETAIN);
}
- (NSArray *)popup_downLayoutConstraints{
    return objc_getAssociatedObject(self, "popup_downLayoutConstraints");
}

- (void)setPopup_blockWhenWillUp:(void (^)(void))popup_blockWhenWillUp{
    objc_setAssociatedObject(self, "popup_blockWhenWillUp", popup_blockWhenWillUp, OBJC_ASSOCIATION_COPY_NONATOMIC);
}
- (void (^)(void))popup_blockWhenWillUp{
    return objc_getAssociatedObject(self, "popup_blockWhenWillUp");
}
- (void)setPopup_blockWhenWillDown:(void (^)(void))popup_blockWhenWillDown{
    objc_setAssociatedObject(self, "popup_blockWhenWillDown", popup_blockWhenWillDown, OBJC_ASSOCIATION_COPY_NONATOMIC);
}
- (void (^)(void))popup_blockWhenWillDown{
    return objc_getAssociatedObject(self, "popup_blockWhenWillDown");
}

- (void)setPopup_blockWhenUping:(void (^)(void))popup_blockWhenUping{
    objc_setAssociatedObject(self, "popup_blockWhenUping", popup_blockWhenUping, OBJC_ASSOCIATION_COPY_NONATOMIC);
}
- (void (^)(void))popup_blockWhenUping{
    return objc_getAssociatedObject(self, "popup_blockWhenUping");
}
- (void)setPopup_blockWhenDowning:(void (^)(void))popup_blockWhenDowning{
    objc_setAssociatedObject(self, "popup_blockWhenDowning", popup_blockWhenDowning, OBJC_ASSOCIATION_COPY_NONATOMIC);
}
- (void (^)(void))popup_blockWhenDowning{
    return objc_getAssociatedObject(self, "popup_blockWhenDowning");
}

- (void)setPopup_blockWhenDidUpFinished:(void (^)(void))popup_blockWhenUpFinished{
    objc_setAssociatedObject(self, "popup_blockWhenDidUpFinished", popup_blockWhenUpFinished, OBJC_ASSOCIATION_COPY_NONATOMIC);
}
- (void (^)(void))popup_blockWhenDidUpFinished{
    return objc_getAssociatedObject(self, "popup_blockWhenDidUpFinished");
}
- (void)setPopup_blockWhenDidDownFinished:(void (^)(void))popup_blockWhenDownFinished{
    objc_setAssociatedObject(self, "popup_blockWhenDidDownFinished", popup_blockWhenDownFinished, OBJC_ASSOCIATION_COPY_NONATOMIC);
}
- (void (^)(void))popup_blockWhenDidDownFinished{
    return objc_getAssociatedObject(self, "popup_blockWhenDidDownFinished");
}

- (void)setPopup_up:(BOOL)popup{
    if(self.popup_up == popup) return;
    objc_setAssociatedObject(self, "popup_up", @(popup), OBJC_ASSOCIATION_COPY);
    [self toPopup:popup];
}
- (BOOL)popup_up{
    id value = objc_getAssociatedObject(self, "popup_up");
    return nil == value ? NO : [value boolValue];
}
- (void)setPopup_active:(BOOL)active{
    if(self.popup_active==active) return;
    objc_setAssociatedObject(self, "popup_active", @(active), OBJC_ASSOCIATION_ASSIGN);
    [self toActive:active];
}
- (BOOL)popup_active{
    id value = objc_getAssociatedObject(self, "popup_active");
    return nil == value ? NO : [value boolValue];
}

- (void)toPopup:(BOOL)popup{
    if(!self.popup_active) return;
    
    NSArray *deactive   = popup ? self.popup_downLayoutConstraints : self.popup_upLayoutConstraints;
    NSArray *active     = popup ? self.popup_upLayoutConstraints : self.popup_downLayoutConstraints;
    [NSLayoutConstraint deactivateConstraints:deactive];
    [NSLayoutConstraint activateConstraints:active];

    XXOC_WS;
    [XXocUtils mainThreadProcess:^{
        XXOC_SS;
        if(popup && ss.popup_blockWhenWillUp){
            ss.popup_blockWhenWillUp();
        }
        else if(!popup && ss.popup_blockWhenWillDown){
            ss.popup_blockWhenWillDown();
        }
        else{
            
        }
        
        [UIView animateWithDuration:0.3 animations:^{
            XXOC_SS;
            if(popup && ss.popup_blockWhenUping){
                ss.popup_blockWhenUping();
            }
            else if(!popup && ss.popup_blockWhenDowning){
                ss.popup_blockWhenDowning();
            }
            else{
                
            }
            [ss.superview layoutIfNeeded];
        } completion:^(BOOL finished) {
            XXOC_SS;
            if(popup && ss.popup_blockWhenDidUpFinished){
                ss.popup_blockWhenDidUpFinished();
            }
            else if(!popup && ss.popup_blockWhenDidDownFinished){
                ss.popup_blockWhenDidDownFinished();
            }
            else{
                
            }
        }];
    }];
}
- (void)toActive:(BOOL)active{
    XXOC_WS;
    [XXocUtils mainThreadProcess:^{
        XXOC_SS;
        if(active){
            if(ss.popup_up && ss.popup_upLayoutConstraints){
                [NSLayoutConstraint activateConstraints:ss.popup_upLayoutConstraints];
                    
                if(ss.popup_blockWhenWillUp){
                    ss.popup_blockWhenWillUp();
                }
                [UIView animateWithDuration:0.3 animations:^{
                    [ss.superview layoutIfNeeded];
                    if(ss.popup_blockWhenUping){
                        ss.popup_blockWhenUping();
                    }
                } completion:^(BOOL finished) {
                    if(ss.popup_blockWhenDidUpFinished){
                        ss.popup_blockWhenDidUpFinished();
                    }
                }];
            }
            else if(!ss.popup_up && ss.popup_downLayoutConstraints){
                [NSLayoutConstraint activateConstraints:ss.popup_downLayoutConstraints];
                
                if(ss.popup_blockWhenWillDown){
                    ss.popup_blockWhenWillDown();
                }
                [UIView animateWithDuration:0.3 animations:^{
                    [ss.superview layoutIfNeeded];
                    if(ss.popup_blockWhenDowning){
                        ss.popup_blockWhenDowning();
                    }
                } completion:^(BOOL finished) {
                    if(ss.popup_blockWhenDidDownFinished){
                        ss.popup_blockWhenDidDownFinished();
                    }
                }];
            }
            else{
            }
        }
        else{
            if(ss.popup_upLayoutConstraints){
                [NSLayoutConstraint deactivateConstraints:ss.popup_upLayoutConstraints];
            }
            if(ss.popup_downLayoutConstraints){
                [NSLayoutConstraint deactivateConstraints:ss.popup_downLayoutConstraints];
            }
        }
    }];
}
@end
