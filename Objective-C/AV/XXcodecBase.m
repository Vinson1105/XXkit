#import "XXcodecBase.h"

@implementation XXvideoEncoderBase
- (instancetype) initWithName:(NSString*)name Param:(id)param Events:(id<XXvideoEncoderEvents> _Nullable)events{
    self = [super init];
    if (self) {
        _name = name;
        _events = events;
        [self configParam:param];
    }
    return self;
}
- (BOOL) start{
    NSAssert(YES, @"需要重载");
    return YES;
}
- (BOOL) encode:(CVImageBufferRef)buffer Timestamp:(NSTimeInterval)timestamp{
    NSAssert(YES, @"需要重载");
    return NO;
}
- (void) stop{
    NSAssert(YES, @"需要重载");
}

- (void) configParam:(id)param{}
@end

@implementation XXvideoDecoderBase
- (instancetype) initWithName:(NSString*)name Param:(id)param Events:(id<XXvideoDecoderEvents> _Nullable)events{
    self = [super init];
    if (self) {
        _name = name;
        _events = events;
        [self configParam:param];
    }
    return self;
}
- (void) decode:(NSData*)data{
    NSAssert(YES, @"需要重载");
}

- (void) configParam:(id)param{}
@end

