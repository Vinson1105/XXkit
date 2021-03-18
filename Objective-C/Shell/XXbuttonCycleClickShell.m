//
//  XXloopButtonShell.m
//  iCamSee
//
//  Created by VINSON on 2020/5/11.
//  Copyright © 2020 Macrovideo. All rights reserved.
//

#import "XXbuttonCycleClickShell.h"

@interface XXbuttonCycleClickShell()
@property (nonatomic,strong) NSMutableArray *keys;
@property (nonatomic,strong) NSMutableDictionary *keyToImage;
@property (nonatomic,copy) NSString *currentKey;
@end

@implementation XXbuttonCycleClickShell
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
- (void)add:(NSString *)key image:(UIImage *)image{
    _keyToImage[key] = image;
    if(![_keys containsObject:key]){
        [_keys addObject:key];
    }
    if(nil == _currentKey){
        _currentKey = key;
        [_button setImage:image forState:UIControlStateNormal];
    }
}
- (void)reset:(NSString *)key{
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
        return;
    }
    
    if(++index >= _keys.count){
        index = 0;
    }
    
    NSString *previous = _currentKey;
    _currentKey = _keys[index];
    UIImage *image = [_keyToImage objectForKey:_currentKey];
    [_button setImage:image forState:UIControlStateNormal];
    
    if(_onClicked){
        _onClicked(self,previous,_currentKey);
    }
}
@end
