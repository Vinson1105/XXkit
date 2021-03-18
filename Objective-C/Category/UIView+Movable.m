//
//  UIView+Movable.m
//  XXkitProject
//
//  Created by VINSON on 2020/11/12.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "UIView+Movable.h"
#import <objc/runtime.h>

@interface UIView()
@property (nonatomic,strong,readonly) UIPanGestureRecognizer *movable_panGesture;
@property (nonatomic,assign) CGFloat movable_margin;
@end

@implementation UIView(Movable)
-(void)movable_install:(CGFloat)margin{
    self.movable_margin = margin;
    [self addGestureRecognizer:self.movable_panGesture];
}
- (void)setMovable_margin:(CGFloat)movable_margin{
    objc_setAssociatedObject(self, "movable_margin", @(movable_margin), OBJC_ASSOCIATION_COPY_NONATOMIC);
}
- (CGFloat)movable_margin{
    return [objc_getAssociatedObject(self, "movable_margin") floatValue];
}
- (UIPanGestureRecognizer *)movable_panGesture{
    UIPanGestureRecognizer *recognizer = objc_getAssociatedObject(self, "movable_panGesture");
    if(nil == recognizer){
        recognizer = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(onPanGestureRecognizer:)];
        objc_setAssociatedObject(self, "movable_panGesture", recognizer, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return recognizer;
}
- (void)onPanGestureRecognizer:(UIPanGestureRecognizer*)recognizer{
    UIView *superview = self.superview;
    CGFloat margin = self.movable_margin;
    
    recognizer.cancelsTouchesInView = YES;
    if (recognizer.state == UIGestureRecognizerStateBegan) {
        // 拖动开始
    }
    else if (recognizer.state == UIGestureRecognizerStateChanged) {
        // 拖动中
        CGPoint translation = [recognizer translationInView:superview];
        recognizer.view.center = CGPointMake(recognizer.view.center.x + translation.x,recognizer.view.center.y + translation.y);
        [recognizer setTranslation:CGPointZero inView:superview];
    }
    else if (recognizer.state == UIGestureRecognizerStateEnded || recognizer.state == UIGestureRecognizerStateCancelled) {
        // 拖动结束
        BOOL changed = NO;
        CGRect superFrame = superview.bounds;
        CGRect currentFrame = recognizer.view.frame;
        CGRect transFrame   = currentFrame;
        if(currentFrame.origin.x<margin){
            transFrame.origin.x = margin;
            changed = YES;
        }
        else if(currentFrame.origin.x > margin){
            // 当前处于视图中间需要判断靠近那个边界
            CGFloat dToLeft      = currentFrame.origin.x;
            CGFloat dToRight   = CGRectGetMaxX(superFrame) - CGRectGetMaxX(currentFrame);
            if(dToLeft>dToRight){
                transFrame.origin.x = CGRectGetMaxX(superFrame)-currentFrame.size.width-margin;
            }
            else{
                transFrame.origin.x = margin;
            }
            changed = YES;
        }
        else{
            
        }
        
        if(currentFrame.origin.y<margin){
            transFrame.origin.y = margin;
            changed = YES;
        }
        else if(CGRectGetMaxY(currentFrame) - CGRectGetMaxY(superFrame) + margin > 0){
            CGFloat offset = CGRectGetMaxY(currentFrame) - CGRectGetMaxY(superFrame) + margin;
            transFrame.origin.y -= offset;
            changed = YES;
        }
//        else if(currentFrame.origin.y > _margin){
//            // 当前处于视图中间需要判断靠近那个边界
//            CGFloat dToTop      = currentFrame.origin.y;
//            CGFloat dToBottom   = CGRectGetMaxY(superFrame) - CGRectGetMaxY(currentFrame);
//            if(dToTop>dToBottom){
//                transFrame.origin.y = CGRectGetMaxY(superFrame)-currentFrame.size.height-_margin;
//            }
//            else{
//                transFrame.origin.y = _margin;
//            }
//            changed = YES;
//        }
        else{
            
        }
        
        if(changed){
            [UIView animateWithDuration:0.3 animations:^{
                recognizer.view.frame = transFrame;
            }];
        }
    }
    else{
        
    }
    recognizer.cancelsTouchesInView = NO;
}
@end
