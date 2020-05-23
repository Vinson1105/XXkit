#import "UIView+TapToVisible.h"
#import <objc/runtime.h>
#import "../Utility/XXtimer.h"
#import "../XXocUtils.h"
#import "UIView+Popup.h"

#define kXXvisible          @"tapToVisiable_visible"
#define kXXholdon           @"tapToVisiable_holdon"
#define kXXvisibleViews     @"tapToVisiable_visibleViews"
#define kTapGesture         @"tapToPopup_tapGesture"
#define kTimer              @"tapToPopup_timer"


@interface UIView()
@property (nonatomic,strong) UITapGestureRecognizer *tapGesture;
@property (nonatomic,strong) XXtimer *timer;
@end

@implementation UIView(TapToVisible)
- (void)setXx_visible:(BOOL)xx_visible{
    objc_setAssociatedObject(self, kXXvisible, @(xx_visible), OBJC_ASSOCIATION_COPY_NONATOMIC);
    [self toVisible:xx_visible];
}
- (BOOL)xx_visible{
    return [objc_getAssociatedObject(self, kXXvisible) boolValue];
}

- (void)setXx_visibleViews:(NSArray<UIView *> *)xx_visibleViews{
    objc_setAssociatedObject(self, kXXvisibleViews, xx_visibleViews, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
- (NSArray<UIView *> *)xx_visibleViews{
    return objc_getAssociatedObject(self, kXXvisibleViews);
}

- (void)setXx_holdon:(BOOL)holdon{
    objc_setAssociatedObject(self, kXXholdon, @(holdon), OBJC_ASSOCIATION_COPY_NONATOMIC);
}
- (BOOL)xx_holdon{
    return [objc_getAssociatedObject(self, kXXholdon) boolValue];
}

- (void)setTapGesture:(UITapGestureRecognizer *)tapGesture{
    objc_setAssociatedObject(self, kTapGesture, tapGesture, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
- (UITapGestureRecognizer *)tapGesture{
    return objc_getAssociatedObject(self, kTapGesture);
}

- (void)setTimer:(XXtimer *)timer{
    objc_setAssociatedObject(self, kTimer, timer, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
- (XXtimer *)timer{
    return objc_getAssociatedObject(self, kTimer);
}

- (void)xx_installTapToVisible:(BOOL)install{
    [self installTapGesture:install];
}

-(void)installTapGesture:(BOOL)install{
    if(install){
        if(nil == self.tapGesture){
            self.tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onTapGesture:)];
            self.tapGesture.cancelsTouchesInView = NO;  /// 不拦截touch事件，处理完之后，分发到目标的view中
        }
        [self addGestureRecognizer:self.tapGesture];
        
        if(nil == self.timer){
            XXtimer *timer  = [XXtimer timerWithDelay:0 interval:5 singleShot:YES];
            self.timer      = timer;
            
            XXOC_WS;
            timer.onTimeout = ^(XXtimer * _Nonnull timer, int index) {
                XXOC_SS;
                if(!ss.xx_holdon){
                    ss.xx_visible = NO;
                }
            };
        }
    }
    else{
        if(nil == self.tapGesture)
            return;
        [self removeGestureRecognizer:self.tapGesture];
        if(nil != self.timer){
            [self.timer stop];
        }
    }
}
-(void)onTapGesture:(UITapGestureRecognizer*)gesture{
    if(self.xx_holdon || self.xx_visible){return;}
    self.xx_visible = YES;
}
- (void)toVisible:(BOOL)visible{
    XXOC_WS;
    [XXocUtils mainThreadProcess:^{
        XXOC_SS;
        
        NSEnumerator *enumer = ss.xx_visibleViews.objectEnumerator;
        UIView *view = nil;
        while (nil != (view = enumer.nextObject)) {
            if(view.xx_popupConstraint && view.xx_popdownConstraint){
                view.xx_popup = visible;
            }
            else{
                view.hidden = !visible;
            }
        }
        
        if(visible && self.timer){
            [self.timer start];
        }
    }];
}
@end
