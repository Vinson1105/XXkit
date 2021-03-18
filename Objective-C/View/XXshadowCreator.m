//
//  XXShadowCreator.m
//  KitDemo
//
//  Created by VINSON on 2018/12/7.
//  Copyright © 2018 VINSON. All rights reserved.
//

#import "XXshadowCreator.h"

@interface XXShadowView : UIView
/** target:要添加阴影的view */
@property(weak,readonly,nonatomic) UIView *target;
- (instancetype) initWithTarget:(UIView*)target;
@end

@implementation XXshadowCreator
+ (void) makeShadowWithTarget:(UIView*)target
                ShadowBgColor:(UIColor*)bgColor
                  ShadowColor:(UIColor*)color
                 ShadowOffset:(CGSize)offset
                ShadowOpacity:(CGFloat)opacity
                 ShadowRadius:(CGFloat)radius
{
    UIView *superView = target.superview;
    if (nil == superView || nil == superView.superview) {
        return ;
    }
    
    XXShadowView *shadowView = [[XXShadowView alloc] initWithTarget:target];
    [superView addSubview:shadowView];
    shadowView.backgroundColor      = bgColor;
    shadowView.clipsToBounds        = NO;
    shadowView.layer.cornerRadius   = target.layer.cornerRadius;
    shadowView.layer.shadowColor    = color.CGColor;
    shadowView.layer.shadowOffset   = offset;
    shadowView.layer.shadowOpacity  = opacity;
    shadowView.layer.shadowRadius   = radius;
    
    [superView bringSubviewToFront:target];
}
@end


@implementation XXShadowView
- (instancetype) initWithTarget:(UIView*)view
{
    self = [super init];
    if (self) {
        _target = view;
        [view addObserver:self forKeyPath:@"frame" options:0 context:nil];
}
    return self;
}
- (void)dealloc
{
    if(nil != _target)
        [_target removeObserver:self forKeyPath:@"frame"];
}
- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    if ([keyPath isEqualToString:@"frame"]) {
        self.frame = [object frame];
    }
    else{
        
    }
}
@end
