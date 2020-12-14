//
//  UIViewController+KeyboradAdapter.m
//  XXkitProject
//
//  Created by VINSON on 2020/12/13.
//  Copyright © 2020 郭文轩. All rights reserved.
//

#import "UIView+KeyboradAdapter.h"
#import <objc/runtime.h>

@interface UIView()
@property (nonatomic,assign) CGPoint contentOffsetBeforeShow;
@end

@implementation UIView(KeyboradAdapter)
- (void)keyboradAdapter_install:(BOOL)install{
    NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
    if(install){
        [center addObserver:self selector:@selector(keyboardWillShow:) name:UIKeyboardWillShowNotification object:nil];
        [center addObserver:self selector:@selector(keyboardWillHide:) name:UIKeyboardWillHideNotification object:nil];
    }
    else{
        [center removeObserver:self name:UIKeyboardWillShowNotification object:nil];
        [center removeObserver:self name:UIKeyboardWillHideNotification object:nil];
    }
}

- (void)setContentOffsetBeforeShow:(CGPoint)contentOffsetBeforeShow{
    objc_setAssociatedObject(self, "contentOffsetBeforeShow", @(contentOffsetBeforeShow), OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
- (CGPoint)contentOffsetBeforeShow{
    return [objc_getAssociatedObject(self, "contentOffsetBeforeShow") CGPointValue];
}
- (void)setKeyboradAdapter_vlayout:(NSLayoutConstraint *)keyboradAdapter_vlayout{
    objc_setAssociatedObject(self, "keyboradAdapter_vlayout", keyboradAdapter_vlayout, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}
- (NSLayoutConstraint *)keyboradAdapter_vlayout{
    return objc_getAssociatedObject(self, "keyboradAdapter_vlayout");
}

-(void)keyboardWillShow:(NSNotification *)notification{
    CGRect keyBoardRect=[notification.userInfo[UIKeyboardFrameEndUserInfoKey] CGRectValue];
    if([self isKindOfClass:UIScrollView.class]){
        UIScrollView *scrollView = (UIScrollView*)self;
        self.contentOffsetBeforeShow = scrollView.contentOffset;
        scrollView.contentOffset = CGPointMake(scrollView.contentOffset.x, scrollView.contentOffset.y-keyBoardRect.size.height);
    }
    else{
        CGRect frame = [self convertRect:self.bounds toView:nil];
        NSLog(@"[###] [*] %@ %@ %d",NSStringFromCGRect(keyBoardRect),NSStringFromCGRect(frame),CGRectContainsRect(keyBoardRect, frame));
        if(!CGRectContainsRect(keyBoardRect, frame) || !self.keyboradAdapter_vlayout)
            return;
        CGFloat offset = frame.origin.y - keyBoardRect.size.height+25;
        self.contentOffsetBeforeShow = CGPointMake(0, self.keyboradAdapter_vlayout.constant);
        self.keyboradAdapter_vlayout.constant -= offset;
        [UIView animateWithDuration:0.2 animations:^{
            [self layoutIfNeeded];
        }];
    }
}
-(void)keyboardWillHide:(NSNotification *)notification{
    CGRect keyBoardRect=[notification.userInfo[UIKeyboardFrameEndUserInfoKey] CGRectValue];
    if([self isKindOfClass:UIScrollView.class]){
        UIScrollView *scrollView = (UIScrollView*)self;
        scrollView.contentOffset = self.contentOffsetBeforeShow;
    }
    else{
        NSLog(@"[###] [#] %@",NSStringFromCGRect(keyBoardRect));
        if(!self.keyboradAdapter_vlayout){
            return;
        }
        self.keyboradAdapter_vlayout.constant = self.contentOffsetBeforeShow.y;
        [UIView animateWithDuration:0.2 animations:^{
            [self layoutIfNeeded];
        }];
    }
}
@end
