#import "XXlimitButtonShell.h"
#import "../Utility/XXtimer.h"
#import "../XXocUtils.h"

@interface XXlimitButtonShell()
@property (nonatomic,strong) XXtimer *timer;
@property (nonatomic,assign) int duration;
@end

@implementation XXlimitButtonShell
#pragma mark - <Init>
- (instancetype)init{
    self = [super init];
    if (self) {
        _duration = 60;
        _timer = [XXtimer timerWithDelay:0 interval:1 singleShot:NO];
        XXOC_WS;
        _timer.onTimeout = ^(XXtimer * _Nonnull timer, int times) {
            dispatch_async(dispatch_get_main_queue(), ^{
                XXOC_SS;
                int remainder = ss.duration - times;
                if(remainder <= 0){
                    [timer stop];
                    ss.button.enabled = YES;
                }
                else{
                    [ss.button setTitle:[NSString stringWithFormat:@"%ds",remainder] forState:UIControlStateDisabled];
                }
            });
        };
    }
    return self;
}
- (void)dealloc{
    [_timer stop];
}

#pragma mark - <Config>
- (void)shell:(UIButton*)button{
    _button = button;
    [_button addTarget:self action:@selector(onButtonTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
}
- (void)configDuration:(int)duration{
    _duration = duration;
}
- (void)configNormalColor:(UIColor*)normalColor limitColor:(UIColor*)limitColor{
    [_button setTitleColor:normalColor forState:UIControlStateNormal];
    [_button setTitleColor:limitColor forState:UIControlStateDisabled];
}

#pragma mark - <Event>
- (void)onButtonTouchUpInside:(UIButton*)sender{
    [_button setTitle:[NSString stringWithFormat:@"%ds",_duration] forState:UIControlStateDisabled];
    _button.enabled = NO;
    [_timer start];
}
@end
