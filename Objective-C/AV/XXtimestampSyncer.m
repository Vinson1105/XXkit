#import "XXtimestampSyncer.h"

#define MIN_MSEC_TIMESTAMP  31507200000     // 毫秒级别时间戳最小值
#define MAX_MSEC_TIMESTAMP  4687730032000   // 毫秒级别时间戳最大值
#define IS_MSEC(timestamp)  (MIN_MSEC_TIMESTAMP < timestamp && timestamp  < MAX_MSEC_TIMESTAMP)

@interface XXtimestampSyncer()
@property (nonatomic,assign) CFAbsoluteTime syncTimeMin;
@property (nonatomic,assign) CFAbsoluteTime syncTimeMax;
@property (nonatomic,assign) CFAbsoluteTime timeliveRefer;      // 实时时间参照点
@property (nonatomic,assign) CFAbsoluteTime timestampRefer;     // 时间戳参照点
@end

@implementation XXtimestampSyncer
- (instancetype)init
{
    self = [super init];
    if (self) {
        _syncTimeMin    = 0.005;
        _syncTimeMax    = 2;
        _timeliveRefer  = 0;
        _timestampRefer = 0;
    }
    return self;
}
- (instancetype)initWithSyncTimeMin:(CFAbsoluteTime)min SyncTimeMax:(CFAbsoluteTime)max{
    self = [super init];
    if (self) {
        _syncTimeMin    = min;
        _syncTimeMax    = max;
        _timeliveRefer  = 0;
        _timestampRefer = 0;
    }
    return self;
}
- (void)sync:(int64_t)msec{
    [self sync:msec Min:_syncTimeMin Max:_syncTimeMax];
}
- (void)sync:(int64_t)msec Min:(CFAbsoluteTime)min Max:(CFAbsoluteTime)max{
    if (!IS_MSEC(msec)) {
        return;
    }
    
    if (_timeliveRefer <= 0) {
        _timeliveRefer = CFAbsoluteTimeGetCurrent();
        _timestampRefer = msec;
    }
    else{
        // 第一帧到当前帧,所需要的绝对时间差(实际值)
        CFAbsoluteTime takeTimelive     = CFAbsoluteTimeGetCurrent() - _timeliveRefer;
        // 第一帧到当前帧,所需要的时间戳差(目标值)
        CFAbsoluteTime takeTimestamp    = (CFAbsoluteTime)msec / 1000.0 - _timestampRefer;
        // 两者的时间差(修正值)
        CFAbsoluteTime takeDiff             = takeTimestamp - takeTimelive;
        if (takeDiff >= min && takeDiff <= max) {
            [NSThread sleepForTimeInterval:takeDiff];
        }
    }
}
- (void)reset{
    _timeliveRefer  = 0;
    _timestampRefer = 0;
}
@end
