/**
 * @author: GWX
 * @date: 20190109
 * @descirption: 容器自定义cell的协议和shell容器类控制器基类.
 *               shell的继承类需要在重载initContext实现context的初始化.
 *               shell中的cellEventBlock主要是对应自定义cell触发的自定义事件.
 * @history:
 *        1.author:
 *          date:
 *          modification:
 */

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

//
// 容器中自定义cell协议
//
@protocol XXcellProtocol <NSObject>
/** cell的数据 */
@property (nonatomic,strong,nullable) NSMutableDictionary *xxdata;
/** cell的位置 */
@property (nonatomic,strong,nullable) NSIndexPath *indexPath;
/** cell的事件处理 */
@property (nonatomic,copy,nullable) void(^eventBlock)(NSString *event,  NSDictionary * _Nullable params);
@end

//
// 容器类控制器基类
//
@interface XXshell : NSObject
/** shell所控制的容器类,主要是tableView和collectView */
@property (nonatomic,weak,readonly) id context;
/** shell内部的整体数据 */
@property (nonatomic,strong,nullable) NSMutableArray *xxdata;
/** cell的类型 */
@property (nonatomic,copy,readonly,nullable) NSString *cellType;
/** cell的加载方式 */
typedef NS_ENUM(NSUInteger, XXcellLoadType){
    XXcellLoadType_Default, // 默认cell
    XXcellLoadType_Class,   // 非xib自定义(代码布局控件)
    XXcellLoadType_Nib,     // xib自定义
};
@property (nonatomic,assign,readonly) XXcellLoadType cellLoadType;
/** cell的事件回调block(默认cell无效) */
@property (nonatomic,copy,nullable) void(^cellEventBlock)(NSString *event, NSDictionary * _Nullable params);

/**
 * @brief    初始化为使用默认cell
 * @param    context 目标容器类
 * @return   instancetype 示例
 */
- (instancetype) initWithContext:(id)context;

/**
 * @brief    初始化为使用手动布局的自定义cell
 * @param    context 目标容器类
 * @param    cellType 自定义cell类型名称
 * @return   instancetype 示例
 */
- (instancetype) initWithContext:(id)context ClassCellType:(NSString*)cellType;

/**
 * @brief    初始化为使用xib布局的自定义cell
 * @param    context 目标容器类
 * @param    cellType 自定义cell类型名称
 * @return   instancetype 示例
 */
- (instancetype) initWithContext:(id)context NIbCellType:(NSString *)cellType;
@end

NS_ASSUME_NONNULL_END
