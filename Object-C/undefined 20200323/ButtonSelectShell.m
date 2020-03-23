//
//  ButtonShell.m
//  iCamSee
//
//  Created by VINSON on 2019/11/19.
//  Copyright © 2019 Macrovideo. All rights reserved.
//

#import "ButtonSelectShell.h"

@interface ButtonSelectShell()
@property (nonatomic,strong) NSMutableDictionary<NSString*,UIButton*> *nameToButton; // 注意与button的引用问题
@property (nonatomic,strong) NSMapTable *buttonToName;
@property (nonatomic,weak) UIButton *lastSelectedButton;
@end

@implementation ButtonSelectShell
- (instancetype)init{
    self = [super init];
    if (self) {
        _nameToButton = [NSMutableDictionary new];
        _buttonToName = [NSMapTable mapTableWithKeyOptions:NSPointerFunctionsWeakMemory valueOptions:NSPointerFunctionsCopyIn];
    }
    return self;
}
- (BOOL)addButton:(UIButton *)button name:(NSString *)name{
    if(nil != _nameToButton[name]) return NO;
    _nameToButton[name] = button;
    [_buttonToName setObject:name forKey:button];
    [button addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
    return YES;
}
-(BOOL)addSelectableButton:(UIButton*)button name:(NSString*)name{
    if(nil != _nameToButton[name]) return NO;
    _nameToButton[name] = button;
    [_buttonToName setObject:name forKey:button];
    [button addTarget:self action:@selector(onSelectableButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
    return YES;
}
- (void)remove:(NSString *)name{
    UIButton *button = _nameToButton[name];
    if(!button){
        return;
    }
    if(button == _lastSelectedButton){
        _lastSelectedButton = nil;
    }
    
    [_nameToButton removeObjectForKey:name];
    [_buttonToName removeObjectForKey:button];
}
-(void)removeAll{
    [_nameToButton removeAllObjects];
    [_buttonToName removeAllObjects];
    _lastSelectedButton = nil;
}
- (void)selected:(BOOL)selected name:(NSString *)name{
    [self selected:selected name:name update:NO];
}
-(void)selected:(BOOL)selected name:(NSString*)name update:(BOOL)update{
    if([NSThread currentThread].isMainThread){
        UIButton *button = _nameToButton[name];
        if(!button || button.selected == selected){
            return;
        }
        if(self.exclusive){
            if(_lastSelectedButton == button){
                return;
            }
            else{
                button.selected = YES;
                if(_lastSelectedButton){
                    _lastSelectedButton.selected = NO;
                }
            }
            self.lastSelectedButton = button;
        }
        else{
            button.selected = selected;
        }
        
        if(update){
            [button setNeedsLayout];
            [button layoutIfNeeded];
        }
    }
    else{
        __weak typeof(self) weakSelf = self;
        dispatch_async(dispatch_get_main_queue(), ^{
            __weak typeof(weakSelf) strongSelf = weakSelf;
            
            UIButton *button = strongSelf.nameToButton[name];
            if(!button || button.selected == selected){
                return;
            }
            if(strongSelf.exclusive){
                if(strongSelf.lastSelectedButton == button){
                    return;
                }
                else{
                    button.selected = YES;
                    if(strongSelf.lastSelectedButton){
                        strongSelf.lastSelectedButton.selected = NO;
                    }
                }
                strongSelf.lastSelectedButton = button;
            }
            else{
                button.selected = selected;
            }
            
            if(update){
                [button setNeedsLayout];
                [button layoutIfNeeded];
            }
        });
    }
}
-(void)enabled:(BOOL)enabled name:(NSString*)name{
    UIButton *button = _nameToButton[name];
    if(button){
        button.enabled = enabled;
    }
}
-(void)enabled:(BOOL)enabled names:(NSArray*)names{
    NSEnumerator *enumer = names.objectEnumerator;
    NSString *name = nil;
    while (nil != (name = enumer.nextObject)) {
        UIButton *button = _nameToButton[name];
        if(button){
            button.enabled = enabled;
        }
    }
}
-(void)hidden:(BOOL)hidden name:(NSString*)name{
    UIButton *button = _nameToButton[name];
    if(button){
        button.hidden = hidden;
    }
}

- (void)setLastSelectedButton:(UIButton *)lastSelectedButton{
    if(lastSelectedButton == _lastSelectedButton) return;
    if(_lastSelectedButton){
        _lastSelectedButton.selected = NO;
        [_lastSelectedButton setNeedsLayout];
    }
    _lastSelectedButton = lastSelectedButton;
    _lastSelectedButton.selected = YES;
}

-(void)onButtonTouchUpInside:(id)sender{
    UIButton *button = sender;
    NSString *name = [_buttonToName objectForKey:button];
    if(nil == name){
        return;
    }
    
    if(_onClicked){
        _onClicked(name, button.selected);
    }
}
-(void)onSelectableButtonTouchUpInside:(id)sender{
    UIButton *button = sender;
    NSString *name = [_buttonToName objectForKey:button];
    if(nil == name){
        return;
    }
    
    BOOL toSelected = !button.selected;
    if(self.exclusive){
        if(_lastSelectedButton == button){
            return;
        }
        else{
            button.selected = YES;
            if(_lastSelectedButton){
                _lastSelectedButton.selected = NO;
            }
        }
        self.lastSelectedButton = button;
    }
    else{
        button.selected = toSelected;
    }
    
    if(_onClicked){
        _onClicked(name, button.selected);
    }
}
@end

