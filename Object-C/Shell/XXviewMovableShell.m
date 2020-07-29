//
//  XXviewMovableShell.m
//  XXkitProject
//
//  Created by VINSON on 2020/7/13.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "XXviewMovableShell.h"

@interface XXviewMovableShell()<UIGestureRecognizerDelegate>
@property (nonatomic,strong) UIPanGestureRecognizer *panGesture;
@property (nonatomic,assign) CGFloat margin;
@end

@implementation XXviewMovableShell
- (instancetype)init{
    self = [super init];
    if (self) {
        _panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(onPanGesture:)];
        _margin = 10;
    }
    return self;
}
- (void)shell:(UIView *)view{
    if(nil != _view){
        return;
    }
    
    _view = view;
    [_view addGestureRecognizer:_panGesture];
}
- (void)onPanGesture:(UIPanGestureRecognizer*)recognizer{
    recognizer.cancelsTouchesInView = YES;
    if (recognizer.state == UIGestureRecognizerStateBegan) {
        // 拖动开始
    }
    else if (recognizer.state == UIGestureRecognizerStateChanged) {
        // 拖动中
        CGPoint translation = [recognizer translationInView:self.view.superview];
        recognizer.view.center = CGPointMake(recognizer.view.center.x + translation.x,recognizer.view.center.y + translation.y);
        [recognizer setTranslation:CGPointZero inView:self.view.superview];
    }
    else if (recognizer.state == UIGestureRecognizerStateEnded || recognizer.state == UIGestureRecognizerStateCancelled) {
        // 拖动结束
        BOOL changed = NO;
        CGRect superFrame = recognizer.view.superview.bounds;
        CGRect currentFrame = recognizer.view.frame;
        CGRect transFrame   = currentFrame;
        if(currentFrame.origin.x<_margin){
            transFrame.origin.x = _margin;
            changed = YES;
        }
        else if(currentFrame.origin.x > _margin){
            // 当前处于视图中间需要判断靠近那个边界
            CGFloat dToLeft      = currentFrame.origin.x;
            CGFloat dToRight   = CGRectGetMaxX(superFrame) - CGRectGetMaxX(currentFrame);
            if(dToLeft>dToRight){
                transFrame.origin.x = CGRectGetMaxX(superFrame)-currentFrame.size.width-_margin;
            }
            else{
                transFrame.origin.x = _margin;
            }
            changed = YES;
        }
        else{
            
        }
        
        if(currentFrame.origin.y<_margin){
            transFrame.origin.y = _margin;
            changed = YES;
        }
        else if(CGRectGetMaxY(currentFrame) - CGRectGetMaxY(superFrame) + _margin > 0){
            CGFloat offset = CGRectGetMaxY(currentFrame) - CGRectGetMaxY(superFrame) + _margin;
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
