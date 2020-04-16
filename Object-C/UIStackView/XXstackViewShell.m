#import "XXstackViewShell.h"

@interface XXstackViewShell()
@property (nonatomic,strong) NSMutableDictionary<NSString*,UIView*> *nameToView;
@end

@implementation XXstackViewShell
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
-(void)addView:(UIView*)view forName:(NSString*)name{
    if(nil == _target) return;
    if(nil != _nameToView[name]) return;
    
    [_target addArrangedSubview:view];
    _nameToView[name] = view;
}
-(void)addView:(UIView*)view size:(CGSize)size forName:(NSString*)name{
    if(nil == _target) return;
    if(nil != _nameToView[name]) return;
    
    view.translatesAutoresizingMaskIntoConstraints = NO;
    [view.widthAnchor constraintEqualToConstant:size.width].active = YES;
    [view.heightAnchor constraintEqualToConstant:size.height].active = YES;
    [_target addArrangedSubview:view];
    _nameToView[name] = view;
}
-(void)removeViewForName:(NSString*)name{
    if(nil == _target) return;
    if(nil == _nameToView[name]) return;
    
    [_target removeArrangedSubview:_nameToView[name]];
    [_nameToView removeObjectForKey:name];
}
-(void)removeViewAll{
    if(nil == _target) return;
    
    NSArray *views = _nameToView.allValues;
    for (UIView *view in views) {
        [_target removeArrangedSubview:view];
        [view removeFromSuperview];
    }
    [_nameToView removeAllObjects];
}
-(void)insertView:(UIView*)view forName:(NSString*)name atIndex:(int)index{
    if(nil == _target) return;
    if(nil != _nameToView[name]) return;
    
    [_target insertArrangedSubview:view atIndex:index];
    _nameToView[name] = view;
}
-(void)insertView:(UIView*)view size:(CGSize)size forName:(NSString*)name atIndex:(int)index{
    if(nil == _target) return;
    if(nil != _nameToView[name]) return;
    
    view.translatesAutoresizingMaskIntoConstraints = NO;
    [view.widthAnchor constraintEqualToConstant:size.width].active = YES;
    [view.heightAnchor constraintEqualToConstant:size.height].active = YES;
    [_target insertArrangedSubview:view atIndex:index];
    _nameToView[name] = view;
}

-(void)hidden:(BOOL)hidden forName:(NSString*)name{
    if(nil == _target) return;
    if(nil == _nameToView[name]) return;
    
    _nameToView[name].hidden = hidden;
}
-(void)enableUserInterface:(BOOL)enable forName:(NSString*)name{
    if(nil == _target) return;
    if(nil == _nameToView[name]) return;
    
    [_nameToView[name] setUserInteractionEnabled:enable];
}
-(nullable UIView*)viewForName:(NSString*)name{
    return _nameToView[name];
}
@end
