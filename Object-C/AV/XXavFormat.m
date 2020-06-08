#import "XXavFormat.h"

@implementation XXaudioFormat
- (nonnull id)copyWithZone:(nullable NSZone *)zone {
    XXaudioFormat *obj = [[[self class] allocWithZone:zone] init];
    obj.sampleRate = self.sampleRate;
    obj.channels = self.channels;
    obj.sampleBitSize = self.sampleBitSize;
    obj.formatID = self.formatID;
    obj.formatFlags = self.formatFlags;
    return obj;
}

@implementation XXvideoFormat
- (nonnull id)copyWithZone:(nullable NSZone *)zone {
    XXvideoFormat *obj = [[[self class] allocWithZone:zone] init];
    obj.width = self.width;
    obj.height = self.height;
    obj.frameRate = self.frameRate;
    obj.bitRate = self.bitRate;
    obj.pixelID = self.pixelID;
    return obj;
}
@end
