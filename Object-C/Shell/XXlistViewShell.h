/**
 * @author: GWX
 * @date: 20190109
 * @descirption: 简化版XTableViewController,舍弃header和footer的配置,默认只有一个section,只对一个section的row进行配置.
 *               目标的数据结构.
 *               Array[row1,row2,row3].
 *               其中row
 *               {
 *                  // 使用自定义cell时
 *                  key1:property1
 *                  key2:property1
 *                  key3:property1
 *
 *                  // 使用默认的cell时
 *                  _Title_:标题
 *               }
 * @history:
 *        1.author:
 *          date:
 *          modification:
 */

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "XXshellCellBase.h"

NS_ASSUME_NONNULL_BEGIN

@interface XXlistViewShell : XXshell<UITableViewDelegate,UITableViewDataSource>
/** 行点击block */
@property (nonatomic,copy,nullable) void(^rowTouchBlock)(NSInteger row, id cellData);

/**
 * @brief    设置固定行高,默认是自适应
 * @param    rowHeight 行高
 */
- (void) setRowHeight:(NSUInteger)rowHeight;

@end

NS_ASSUME_NONNULL_END
