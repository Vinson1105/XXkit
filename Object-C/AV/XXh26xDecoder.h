#import <Foundation/Foundation.h>
#import <VideoToolbox/VideoToolbox.h>

NS_ASSUME_NONNULL_BEGIN
@protocol XXdecoderDerived <NSObject>
@optional - (CVPixelBufferRef) toDecodeData:(uint8_t*)data Length:(int)length;
@optional - (BOOL) toDecodeSrcData:(uint8_t*)srcData SrcLength:(int)srcLength DestBuffer:(uint8_t*)destBuffer DestLength:(int*)destLength;
@end

@interface XXdecoderBase : NSObject
@property (nonatomic,copy,readonly) NSString *name;
- (CVPixelBufferRef) decodeData:(uint8_t*)data Length:(int)length;
- (BOOL) decodeSrcData:(uint8_t*)srcData SrcLength:(int)srcLength DestBuffer:(uint8_t*)destBuffer DestLength:(int*)destLength;
@end

@interface XXh26xDecoder : XXdecoderBase
+(XXh26xDecoder*) decoderWithName:(NSString*)name IsH264:(BOOL)isH264;
@end
NS_ASSUME_NONNULL_END
