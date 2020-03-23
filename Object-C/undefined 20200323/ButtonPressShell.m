//
//  XXButtonPressShell.m
//  iCamSee
//
//  Created by VINSON on 2020/3/23.
//  Copyright © 2020 Macrovideo. All rights reserved.
//

#import "ButtonPressShell.h"
#import "XXtimer.h"

@interface ButtonPressShell()
@property (nonatomic,strong) NSMutableDictionary *nameToButton;
@property (nonatomic,strong) NSMutableArray *buttonsPressed;
@property (nonatomic,strong) XXtimer *timer;
@end

@implementation ButtonPressShell
- (instancetype)init{
    self = [super init];
    if (self) {
        _interval = 0.2;
        _nameToButton = [NSMutableDictionary new];
        _buttonsPressed = [NSMutableArray new];
    }
    return self;
}
- (BOOL)addButton:(UIButton*)button withName:(NSString*)name{
    if(nil != [_nameToButton objectForKey:name]) return NO;
    _nameToButton[name] = button;
    
    [button addTarget:self action:@selector(onButtonTouchDown:) forControlEvents:UIControlEventTouchDown];
    [button addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
    [button addTarget:self action:@selector(onButtonTouchUpOutside:) forControlEvents:UIControlEventTouchUpOutside];
    
    return YES;
}
- (void)removeButtonForName:(NSString*)name{
    UIButton *button = [_nameToButton objectForKey:name];
    if(nil == button){
        return;
    }
    
    [_buttonsPressed removeObject:button];
    if(_buttonsPressed.count == 0 && _timer.isRunning){
        [_timer stop];
    }
    
    [button removeTarget:self action:@selector(onButtonTouchDown:) forControlEvents:UIControlEventTouchDown];
    [button removeTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
    [button removeTarget:self action:@selector(onButtonTouchUpOutside:) forControlEvents:UIControlEventTouchUpOutside];
}
- (void)stop{
    [_timer stop];
    [_buttonsPressed removeAllObjects];
}

- (void)onButtonTouchDown:(id)sender{
    if(nil == _timer){
        _timer = [XXtimer timerWithDelay:0 interval:_interval singleShot:NO];
        __weak typeof(self) weakSelf = self;
        _timer.onTimeout = ^(int times) {
            __strong typeof(weakSelf) strongSelf = weakSelf;
            if(!strongSelf.onPressed){
                return;
            }
            
            NSArray *buttons        = [strongSelf.buttonsPressed copy];
            NSDictionary *all       = [strongSelf.nameToButton copy];
            NSEnumerator *enumer    = [all keyEnumerator];
            NSString *name          = nil;
            NSMutableArray *names   = [NSMutableArray new];
            while (nil != (name = enumer.nextObject)) {
                UIButton *button = all[name];
                if([buttons containsObject:button]){
                    [names addObject:name];
                }
            }
            strongSelf.onPressed(names);
        };
    }
    if(!_timer.isRunning){
        [_timer start];
        if(self.onBegin){
            self.onBegin();
        }
    }
    
    [_buttonsPressed addObject:sender];
    NSArray *keys = [_nameToButton allKeysForObject:sender];
    if(keys.count > 0 && self.onPressed){
        self.onPressed(@[keys.firstObject]);
    }
}
- (void)onButtonTouchUpInside:(id)sender{
    if(_buttonsPressed.count == 0) return;
    [_buttonsPressed removeObject:sender];
    if(_buttonsPressed.count == 0){
        [_timer stop];
        if(self.onEnd){
            self.onEnd();
        }
    }
}
- (void)onButtonTouchUpOutside:(id)sender{
    if(_buttonsPressed.count == 0) return;
    [_buttonsPressed removeObject:sender];
    if(_buttonsPressed.count == 0){
        [_timer stop];
        if(self.onEnd){
            self.onEnd();
        }
    }
}
@end
