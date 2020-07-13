#import "XXbuttonLimitShell.h"
#import "../Utility/XXtimer.h"
#import "../XXocUtils.h"

@interface XXbuttonLimitShell()
@property (nonatomic,strong) XXtimer *timer;
@end

@implementation XXbuttonLimitShell
#pragma mark - <Init>
- (instancetype)init{
    self = [super init];
    if (self) {
        _duration = 60;
        _limited = NO;
        _format = @"%ds";
        _clickEnterLimit = YES;
        
        _timer = [XXtimer timerWithDelay:0 interval:1 singleShot:NO];
        XXOC_WS;
        _timer.onTimeout = ^(XXtimer *timer, int times) {
            dispatch_async(dispatch_get_main_queue(), ^{
                XXOC_SS;
                int remainder = ss.duration - times;
                if(remainder <= 0){
                    ss.limited = NO;
                }
                else{
                    [ss updateLimitText:remainder];
                }
            });
        };
    }
    return self;
}
- (void)dealloc{
    [_timer stop];
}

#pragma mark - <Property>
- (void)setEnabled:(BOOL)enabled{
    if(_enabled == enabled) return;
    _enabled = enabled;
    if(!self.limited && self.button){
        self.button.enabled = enabled;
    }
}
- (void)setLimited:(BOOL)limited{
    if(_limited==limited) return;
    _limited = limited;
    
    XXOC_WS
    [XXocUtils mainThreadProcess:^{
        XXOC_SS
        if(ss.button){
            if(limited){
                [ss updateLimitText:ss.duration];
                [ss.timer start];
                ss.button.enabled = NO;
            }
            else{
                ss.button.enabled = ss.enabled;
                [ss.timer stop];
            }
        }
    }];
}

#pragma mark - <Config>
- (void)shell:(UIButton*)button{
    if(_button){
        return;
    }
    
    _button = button;
    _enabled = button.enabled;
    [_button addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
}
- (void)configDuration:(int)duration{
    _duration = duration;
}
- (void)configNormalColor:(UIColor*)normalColor limitColor:(UIColor*)limitColor{
    [_button setTitleColor:normalColor forState:UIControlStateNormal];
    [_button setTitleColor:limitColor forState:UIControlStateDisabled];
}
- (void)resetLimit{
    self.limited = NO;
}

#pragma mark - <Event>
- (void)onButtonTouchUpInside:(UIButton*)sender{
    if(self.clickEnterLimit){
        self.limited = YES;
    }
    
    if(self.onClicked){
        self.onClicked(self);
    }
}

#pragma mark - <Private>
- (void)updateLimitText:(int)times{
    if(_format){
        [_button setTitle:[NSString stringWithFormat:_format, times] forState:UIControlStateDisabled];
    }
}
@end
