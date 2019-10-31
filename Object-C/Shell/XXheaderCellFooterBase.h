#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXheaderCellFooterBase : NSObject
/** cell的位置属性 */
@property (nonatomic,strong) NSIndexPath *indexPath;
@property (nonatomic,nullable,strong) NSDictionary *sytle;
/**
 * @brief    设置header/cell/footer数据,派生类需要重载这个函数
 * @param    jdata 目标数据,需要符合json格式,nil时需要派生类重置对应属性
 */
-(void)setJData:(nullable NSDictionary*)jdata;
/**
 * @brief    获取header/cell/footer数据,派生类需要重载这个函数,否则直接返回nil
 * @return   返回的数据不一定与之前设置的一致,实际的键值需要派生类按需写入,需要符合json格式
 */
-(nullable NSDictionary*)getJData;
/**
 * @brief    发出通知,内部包装了userInfo,其中包含了IndexPath和JData(可省缺)的键值
 * @param    name 通知的名称,最好统一命名为'sigXXX'
 */
-(void)postNotificationName:(NSString*)name;
/**
 * @brief   发出一个内部同步数据通知,
 */
-(void)emitSigDataChanged;
@end
NS_ASSUME_NONNULL_END
