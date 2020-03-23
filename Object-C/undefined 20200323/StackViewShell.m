//
//  StackViewShell.m
//  iCamSee
//
//  Created by VINSON on 2019/11/19.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

#import "StackViewShell.h"

NSString * const kSigButtonClicked = @"sigButtonClicked";


@interface StackViewShell()
@property (nonatomic,strong) NSMutableDictionary<NSString*,UIView*> *nameToView;
@end

@implementation StackViewShell
+(NSString*)sigButtonClicked{
    return kSigButtonClicked;
}

- (instancetype)init{
    self = [super init];
    if (self) {
        _nameToView = [NSMutableDictionary new];
    }
    return self;
}

-(void)shell:(UIStackView*)target{
    if(nil != _target) return;
    _target = target;
    [_nameToView removeAllObjects];
}
-(void)add:(NSString*)name view:(UIView*)view{
    if(nil == self.target) return;
    if(nil != _nameToView[name]) return;
    
    [_target addArrangedSubview:view];
    _nameToView[name] = view;
}
-(void)add:(NSString*)name size:(CGSize)size view:(UIView*)view{
    if(nil == self.target) return;
    if(nil != _nameToView[name]) return;
    
    view.translatesAutoresizingMaskIntoConstraints = NO;
    [view.widthAnchor constraintEqualToConstant:size.width].active = YES;
    [view.heightAnchor constraintEqualToConstant:size.height].active = YES;
    [_target addArrangedSubview:view];
    _nameToView[name] = view;
}
-(void)remove:(NSString*)name{
    if(nil == self.target) return;
    if(nil == _nameToView[name]) return;
    
    [_target removeArrangedSubview:_nameToView[name]];
    [_nameToView removeObjectForKey:name];
}
-(void)removeAll{
    NSArray *views = _nameToView.allValues;
    for (UIView *view in views) {
        [_target removeArrangedSubview:view];
        [view removeFromSuperview];
    }
    [_nameToView removeAllObjects];
}
-(void)insert:(NSString*)name view:(UIView*)view atIndex:(int)index{
    if(nil == self.target) return;
    if(nil != _nameToView[name]) return;
    
    [_target insertArrangedSubview:view atIndex:index];
    _nameToView[name] = view;
}
- (void)insert:(NSString *)name size:(CGSize)size view:(UIView *)view atIndex:(int)index{
    if(nil == self.target) return;
    if(nil != _nameToView[name]) return;
    
    view.translatesAutoresizingMaskIntoConstraints = NO;
    [view.widthAnchor constraintEqualToConstant:size.width].active = YES;
    [view.heightAnchor constraintEqualToConstant:size.height].active = YES;
    [_target insertArrangedSubview:view atIndex:index];
    _nameToView[name] = view;
}

-(void)hide:(BOOL)hide Name:(NSString*)name{
    if(nil == _nameToView[name]) return;
    _nameToView[name].hidden = hide;
}
-(nullable UIView*)view:(NSString *)name{
    return _nameToView[name];
}
@end
