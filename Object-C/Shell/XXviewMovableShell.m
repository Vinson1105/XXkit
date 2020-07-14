//
//  XXviewMovableShell.m
//  XXkitProject
//
//  Created by VINSON on 2020/7/13.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "XXviewMovableShell.h"

@interface XXviewMovableShell()
@property (nonatomic,strong) UIPanGestureRecognizer *panGesture;
@end

@implementation XXviewMovableShell
- (instancetype)init{
    self = [super init];
    if (self) {
        _panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(onPanGesture:)];
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
        NSLog(@"FlyElephant---视图拖动开始");
    }
    else if (recognizer.state == UIGestureRecognizerStateChanged) {
        CGPoint translation = [recognizer translationInView:self.view.superview];
        recognizer.view.center = CGPointMake(recognizer.view.center.x + translation.x,recognizer.view.center.y + translation.y);
        [recognizer setTranslation:CGPointZero inView:self.view.superview];
    }
    else if (recognizer.state == UIGestureRecognizerStateEnded || recognizer.state == UIGestureRecognizerStateCancelled) {
        NSLog(@"FlyElephant---视图拖动结束");
    }
    else{
        
    }
}
@end
