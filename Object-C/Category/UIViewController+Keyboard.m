//
//  UIViewController+Keyboard.m
//  XXkitProject
//
//  Created by VINSON on 2020/11/3.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "UIViewController+Keyboard.h"
#import <objc/runtime.h>

@interface UIViewController()
@property (nonatomic,strong) UITapGestureRecognizer *keyboard_tapGesture;
@property (nonatomic,strong) NSMutableArray *keyboard_responders;
@end

@implementation UIViewController(Keyboard)
-(void)keyboard_addFirstResponder:(id)responder{
    if([self.keyboard_responders containsObject:responder]){
        return;
    }
    [self.keyboard_responders addObject:responder];
}
-(void)keyboard_removeFirstResponder:(id)responder{
    [self.keyboard_responders removeObject:responder];
}
- (void)setKeyboard_tapToHidden:(BOOL)keyboard_tapToHidden{
    objc_setAssociatedObject(self, "keyboard_tapToHidden", @(keyboard_tapToHidden), OBJC_ASSOCIATION_ASSIGN);
}
- (BOOL)keyboard_tapToHidden{
    return objc_getAssociatedObject(self, "keyboard_tapToHidden");
}
- (UITapGestureRecognizer *)keyboard_tapGesture{
    id obj = objc_getAssociatedObject(self, "keyboard_tapGesture");
    if(obj){
        UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onTap:)];
        tap.numberOfTapsRequired = 1;
        tap.numberOfTouchesRequired = 1;
        obj = tap;
        objc_setAssociatedObject(self, "keyboard_tapGesture", obj, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return obj;
}
- (NSMutableArray *)keyboard_responders{
    id obj = objc_getAssociatedObject(self, "keyboard_responders");
    if(obj){
        NSMutableArray *array = [NSMutableArray new];
        obj = array;
        objc_setAssociatedObject(self, "keyboard_responders", obj, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    return obj;
}
-(void)onTap:(UITapGestureRecognizer*)tap{
    for (id obj in self.keyboard_responders) {
        if([obj respondsToSelector:@selector(resignFirstResponder)]){
            [obj resignFirstResponder];
        }
    }
}
@end
