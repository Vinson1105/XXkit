#import "XXtimer.h"

@interface XXtimer()
@property (nonatomic,strong) dispatch_source_t timer;
@property (nonatomic,assign) NSTimeInterval delay;
@property (nonatomic,assign) NSTimeInterval interval;
@property (nonatomic,assign) BOOL singleShot;
@property (nonatomic,assign) int index;
@end

@implementation XXtimer
+(XXtimer*)timerWithDelay:(NSTimeInterval)delay interval:(NSTimeInterval)interval singleShot:(BOOL)singleShot{
    XXtimer *timer = [[XXtimer alloc] initWithDelay:delay interval:interval singleShot:singleShot];
    return timer;
}

-(instancetype)initWithDelay:(NSTimeInterval)delay interval:(NSTimeInterval)interval singleShot:(BOOL)singleShot{
    self = [super init];
    if(self){
        _delay = delay;
        _interval = interval;
        _singleShot = singleShot;
        _isRunning = NO;
    }
    return self;
}
- (void)dealloc{
    if(_timer){
        [self stop];
    }
}

-(void)resetDelay:(NSTimeInterval)delay interval:(NSTimeInterval)interval singleShot:(BOOL)singleShot{
    [self stop];
    _delay = delay;
    _interval = interval;
    _singleShot = singleShot;
}
-(void)start{
    if(nil != _timer){
        [self stop];
    }
    self.index = -2;    /// 过滤掉delay的触发
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    _timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, queue);
    dispatch_source_set_timer(_timer, dispatch_time(DISPATCH_TIME_NOW, _delay*NSEC_PER_SEC), _interval*NSEC_PER_SEC, 0);

    __weak typeof(self) weakSelf = self;
    dispatch_source_set_event_handler(_timer, ^{
        __strong typeof(self) strongSelf = weakSelf;
        ++strongSelf.index;
        if(strongSelf.index < 0){
            /// 由于delay和interval都会触发这个回调，当delay为1时，interval为2时，start后1s，delay结束会触发一次回调，然后在循环2s触发interval，所以这里第一次触发直接返回
            return;
        }
        
        if(strongSelf.singleShot){
            [strongSelf stop];
        }
        if(strongSelf.onTimeout){
            strongSelf.onTimeout(strongSelf,strongSelf.index);
        }
    });
    dispatch_resume(_timer);
    _isRunning = YES;
}
-(void)stop{
    if(_timer){
        dispatch_source_cancel(_timer);
        _timer = nil;
    }
    _isRunning = NO;
}
@end
