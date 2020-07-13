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
    
}
@end
