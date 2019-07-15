#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXtimestampSyncer : NSObject
/**
 * @brief    初始化时间戳y同步器
 * @param    min 最小有效同步时间(默认0.005)
 * @param    max 最大有效同步时间(默认2.000)
 * @return   对应的实例
 */
- (instancetype)initWithSyncTimeMin:(CFAbsoluteTime)min SyncTimeMax:(CFAbsoluteTime)max;

/**
 * @brief    同步(注意会产生sleep,在主线程谨慎使用)
 * @param    msec 需要同步的时间戳(需要使用毫秒,非毫秒直接返回)
 */
- (void)sync:(int64_t)msec;

/**
 * @brief    同步(注意会产生sleep,在主线程谨慎使用)
 * @param    msec 需要同步的时间戳(需要使用毫秒,非毫秒直接返回)
 * @param    min 最小有效同步时间(临时值,不会覆盖属性值)
 * @param    max 最大有效同步时间(临时值,不会覆盖属性值)
 */
- (void)sync:(int64_t)msec Min:(CFAbsoluteTime)min Max:(CFAbsoluteTime)max;

/**
 * @brief    重置内部使用记录值,不会重置SyncTimeMin和SyncTimeMax
 */
- (void)reset;
@end

NS_ASSUME_NONNULL_END
