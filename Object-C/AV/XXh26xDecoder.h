/**
 * 20200-03-28
 * 1、删除冗余的Base和Protocol，简化层次
 * 2、加上必要性注释
 */

#import <Foundation/Foundation.h>
#import <VideoToolbox/VideoToolbox.h>

NS_ASSUME_NONNULL_BEGIN
@interface XXh26xDecoder : NSObject
/**
 * @brief    创建h264或者h265硬件解码器
 * @param    h264 YES：创建h264解码器，NO：创建h265解码器
 * @return   返回nil，创建失败；否则返回解码器实例
 */
+ (XXh26xDecoder*)decoderWithH264:(BOOL)h264;

/**
 * @brief    解码数据，同步返回，并返回【CVPixelBufferRef】,需要自主对【CVPixelBufferRetain】和【CVPixelBufferRelease】
 * @param    data 待解码的原始数据，数据中的nalu之间需要有【0x00 0x00 0x00 0x01】
 * @param    length 原始数据长度
 * @return   失败返回nil
 */
- (CVPixelBufferRef)decodeData:(uint8_t*)data length:(int)length;

/**
 * @brief    解码数据，同步返回，需要预先分配destBuffer
 * @param    srcData 待解码的原始数据，数据中的nalu之间需要有【0x00 0x00 0x00 0x01】
 * @param    srcLength 原始数据长度
 * @param    destBuffer 存放解码后数据的buffer
 * @param    destLength 解码后数据长度
 * @return   失败返回NO，否则返回YES
 */
- (BOOL)decodeSrcData:(uint8_t*)srcData srcLength:(int)srcLength destBuffer:(uint8_t*)destBuffer destLength:(int*)destLength;
@end
NS_ASSUME_NONNULL_END
