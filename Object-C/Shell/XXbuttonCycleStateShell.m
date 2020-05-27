//
//  XXloopButtonShell.m
//  iCamSee
//
//  Created by VINSON on 2020/5/11.
//  Copyright © 2020 Macrovideo. All rights reserved.
//

#import "XXbuttonCycleStateShell.h"

@interface XXbuttonCycleStateShell()
@property (nonatomic,strong) NSMutableArray *keys;
@property (nonatomic,strong) NSMutableDictionary *keyToImage;
@property (nonatomic,copy) NSString *currentKey;
@end

@implementation XXbuttonCycleStateShell
- (instancetype)init
{
    self = [super init];
    if (self) {
        _keyToImage = [NSMutableDictionary new];
        _keys = [NSMutableArray new];
    }
    return self;
}

- (void)shell:(UIButton*)button{
    _button = button;
    [_button addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
}
- (void)configCycleState:(NSString *)key image:(UIImage *)image{
    _keyToImage[key] = image;
    if(![_keys containsObject:key]){
        [_keys addObject:key];
    }
    if(nil == _currentKey){
        _currentKey = key;
        [_button setImage:image forState:UIControlStateNormal];
    }
}
- (void)resetCycleState:(NSString *)key{
    if(_currentKey == key){
        return;
    }
    UIImage *image = [_keyToImage objectForKey:key];
    if(nil == image){
        return;
    }
    _currentKey = key;
    [_button setImage:image forState:UIControlStateNormal];
}

- (void)onButtonTouchUpInside:(UIButton*)button{
    if(nil == _currentKey || _keys.count <= 1){
        return;
    }
    
    NSUInteger index = [_keys indexOfObject:_currentKey];
    if(NSNotFound == index){
        NSLog(@"[XXbuttonCycleStateShell] key(%@)不存在在已有列表中",_currentKey);
        return;
    }
    
    if(++index >= _keys.count){
        index = 0;
    }
    
    NSString *previous = _currentKey;
    _currentKey = _keys[index];
    UIImage *image = [_keyToImage objectForKey:_currentKey];
    [_button setImage:image forState:UIControlStateNormal];
    
    if(_onCycleStateChanged){
        _onCycleStateChanged(self,previous,_currentKey);
    }
}
@end
