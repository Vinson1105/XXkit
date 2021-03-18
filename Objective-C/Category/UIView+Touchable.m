//
//  UIView+Touchable.m
//  XXkitProject
//
//  Created by VINSON on 2020/11/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "UIView+Touchable.h"
#import <objc/runtime.h>

@interface UIView()
@property (nonatomic,strong,readonly) UITapGestureRecognizer *touchable_tapGesture;
@property (nonatomic,copy) void(^touchable_handler)(void);
@end

@implementation UIView(Touchable)
-(void)touchable_install:(void (^)(void))handler{
    self.touchable_handler = handler;
    [self addGestureRecognizer:self.touchable_tapGesture];
}
- (UITapGestureRecognizer *)touchable_tapGesture{
    UITapGestureRecognizer *tap = objc_getAssociatedObject(self, "touchable_tapGesture");
    if(nil == tap){
        tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onTouchable_tapGesture:)];
        tap.numberOfTapsRequired = 1;
        tap.numberOfTouchesRequired = 1;
        tap.cancelsTouchesInView = NO;
        objc_setAssociatedObject(self, "touchable_tapGesture", tap, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return tap;
}
- (void)setTouchable_handler:(void (^)(void))touchable_handler{
    objc_setAssociatedObject(self, "touchable_handler", touchable_handler, OBJC_ASSOCIATION_COPY);
}
- (void (^)(void))touchable_handler{
    return objc_getAssociatedObject(self, "touchable_handler");
}
-(void)onTouchable_tapGesture:(UITapGestureRecognizer*)recognizer{
    if(self.touchable_handler){
        self.touchable_handler();
    }
}
@end
