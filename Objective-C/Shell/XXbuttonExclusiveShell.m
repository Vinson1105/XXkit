#import "XXbuttonExclusiveShell.h"
#import "../XXocUtils.h"

@interface XXbuttonExclusiveShell()
@property (nonatomic,strong) NSMutableDictionary<NSString*,UIButton*> *nameToButton; // 注意与button的引用问题
@property (nonatomic,strong) NSMapTable *buttonToName;
@end

@implementation XXbuttonExclusiveShell
#pragma mark - <Init>
- (instancetype)init{
    self = [super init];
    if (self) {
        _nameToButton = [NSMutableDictionary new];
        _buttonToName = [NSMapTable mapTableWithKeyOptions:NSPointerFunctionsWeakMemory valueOptions:NSPointerFunctionsCopyIn];
    }
    return self;
}

#pragma mark - <添加、移除button>
- (void)addButton:(UIButton *)button name:(NSString *)name{
    if(nil != _nameToButton[name]) {
        [self remove:name];
    }
    _nameToButton[name] = button;
    [_buttonToName setObject:name forKey:button];
    [button addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
}
- (void)remove:(NSString *)name{
    UIButton *button = _nameToButton[name];
    if(nil == button){
        return;
    }
    
    if(nil != _selected && [_selected isEqualToString:name]){
        _selected = nil;
    }
    
    [_nameToButton removeObjectForKey:name];
    [_buttonToName removeObjectForKey:button];
    [button removeTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
}
- (void)removeAll{
    [_nameToButton removeAllObjects];
    [_buttonToName removeAllObjects];
    _selected = nil;
}

#pragma mark - <设置当前select>
- (void)reset:(NSString*)name{
    if([_selected isEqualToString:name] || nil == [_nameToButton objectForKey:name]){
        return;
    }
    
    UIButton *button = nil;
    if(nil != _selected){
        button = _nameToButton[_selected];
        [XXocUtils mainThreadProcess:^{
            button.selected = NO;
        }];
    }
    
    _selected = name;
    button = _nameToButton[_selected];
    [XXocUtils mainThreadProcess:^{
        button.selected = YES;
    }];
}
- (void)select:(NSString*)name{
    if([_selected isEqualToString:name] || nil == [_nameToButton objectForKey:name]){
        return;
    }
    
    UIButton *button = nil;
    if(nil != _selected){
        button = _nameToButton[_selected];
        [XXocUtils mainThreadProcess:^{
            button.selected = NO;
        }];
    }
    
    NSString *previous = _selected;
    _selected = name;
    button = _nameToButton[_selected];
    [XXocUtils mainThreadProcess:^{
        button.selected = YES;
    }];
    
    if(_onSelectChanged){
        XXOC_WS;
        [XXocUtils mainThreadProcess:^{
            XXOC_SS;
            ss.onSelectChanged(ss, previous, name);
        }];
    }
}

#pragma mark - <查询button>
- (nullable UIButton*)button:(NSString*)name{
    return [_nameToButton objectForKey:name];
}

#pragma mark - <button事件>
- (void)onButtonTouchUpInside:(UIButton*)button{
    NSString *name = [_buttonToName objectForKey:button];
    if(nil != name){
        [self select:name];
    }
}
@end

