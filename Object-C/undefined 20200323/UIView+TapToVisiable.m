#import "UIView+TapToVisiable.h"
#import <objc/runtime.h>
#import "XXtimer.h"

@interface UIView()
@property (nonatomic,strong) UITapGestureRecognizer *tapGesture;
@property (nonatomic,strong) XXtimer *timer;
@end
@implementation UIView(TapToVisiable)
#pragma mark - 属性读写: <>
- (void)setViews:(NSArray<UIView *> *)views{
    objc_setAssociatedObject(self, @"XXviews", views, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
- (NSArray<UIView *> *)views{
    return objc_getAssociatedObject(self, @"XXviews");
}
- (void)setViewsHidden:(BOOL)viewsHidden{
    //if(self.viewsHidden == viewsVisiable) return;
    objc_setAssociatedObject(self, @"XXviewsHidden", @(viewsHidden), OBJC_ASSOCIATION_COPY_NONATOMIC);
    [self toShowViews:!viewsHidden];
}
- (BOOL)viewsHidden{
    return [objc_getAssociatedObject(self, @"XXviewsHidden") boolValue];
}
- (void)setHoldon:(BOOL)holdon{
    objc_setAssociatedObject(self, @"XXholdon", @(holdon), OBJC_ASSOCIATION_COPY_NONATOMIC);
}
- (BOOL)holdon{
    return [objc_getAssociatedObject(self, @"XXholdon") boolValue];
}
- (void)setTapGesture:(UITapGestureRecognizer *)tapGesture{
    objc_setAssociatedObject(self, @"XXtapGesture", tapGesture, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
- (UITapGestureRecognizer *)tapGesture{
    return objc_getAssociatedObject(self, @"XXtapGesture");
}
- (void)setTimer:(XXtimer *)timer{
    objc_setAssociatedObject(self, @"XXtimer", timer, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
- (XXtimer *)timer{
    return objc_getAssociatedObject(self, @"XXtimer");
}

#pragma mark - 公共接口: <>
-(void)installTapToVisiable:(BOOL)install{
    [self installTapGesture:install];
}

#pragma mark - 私有函数: <>
-(void)installTapGesture:(BOOL)install{
    if(install){
        if(nil == self.tapGesture){
            self.tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onTapGesture:)];
            self.tapGesture.cancelsTouchesInView = NO;
        }
        [self addGestureRecognizer:self.tapGesture];
        
        if(nil == self.timer){
            self.timer = [XXtimer timerWithDelay:0 interval:5 singleShot:NO];
            __weak typeof(self) weakSelf = self;
            self.timer.onTimeout = ^(int times) {
                __weak typeof(weakSelf) strongSelf = weakSelf;
                if(!strongSelf.holdon && times > 1){
                    strongSelf.viewsHidden = YES;
                    [strongSelf.timer stop];
                }
            };
        }
    }
    else{
        if(nil == self.tapGesture)
            return;
        [self removeGestureRecognizer:self.tapGesture];
    }
}
-(void)onTapGesture:(UITapGestureRecognizer*)gesture{
    if(self.holdon || !self.viewsHidden){return;}
    self.viewsHidden = NO;
}
-(void)toShowViews:(BOOL)show{
    if(!self.views) return;
    
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        GWXLog(@"[TapToVisiable] show:%d", show);
        __strong typeof(weakSelf) strongSelf = self;
        NSEnumerator *enumer = strongSelf.views.objectEnumerator;
        UIView *view = nil;
        while (nil != (view = enumer.nextObject)) {
            view.hidden = !show;
        }
//        [strongSelf setNeedsLayout];
//        [strongSelf layoutIfNeeded];
    });
    if(show){
        if(!self.timer.isRunning){
            GWXLog(@"[TapToVisiable] timer start");
            [self.timer start];
        }
    }
    else{
        [self.timer stop];
    }
}
@end
