#import "XXencoder.h"

@implementation XXvideoEncoder
@synthesize name;
- (instancetype) initWithName:(NSString*)name{
    self = [super init];
    if (self) {
        self.name = name;
    }
    return self;
}
- (BOOL) isVideoEncoder{
    NSAssert(YES, @"需要重载");
    return YES;
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

@end

