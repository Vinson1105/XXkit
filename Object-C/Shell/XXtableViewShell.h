/*
 * @author: GWX
 * @date:   20190109
 * @descirption: 实现基础的TableView功能,当需要实现简单的数据显示时,可以省去TabelViewDelegate和TableViewDataSource的实现,
 *               只需要把数据按照一定的格式传入,自动根据cell的类型实现cell的创建以及数据的分发,然后自定义cell中实现.
 *               约定其使用的数据结构如下:
 *               {
 *                  [{  // section1
 *                      _Header_: {
 *                          // 默认header
 *                          _Title_:
 *
 *                          // 使用自定义header
 *                          k1:v1       // section1's header property1
 *                          k2:v2       // section1's header property2
 *                          ...
 *                      }
 *
 *                      _Footer_: {
 *                          // 使用默认footer
 *                          _Title_:
 *
 *                          // 使用自定义footer
 *                          k1:v1       // section1's footer property1
 *                          k2:v2       // section1's footer property2
 *                          ...
 *                      }
 *
 *                      _Row_:[{      // row1
 *                          // 使用默认的cell
 *                          _Title_ // 默认cell使用到
 *
 *                          // 使用自定义cell
 *                          k1:v1   // row1's property1
 *                          k2:v2   // row1's property2
 *                          ...
 *                      },
 *                      {row2},{row3},{row4}]
 *                  },
 *                  {section2},{section3},{section4}]
 *              }
 * @history:
 *        1.author:
 *          date:
 *          modification:
 */

#import <UIKit/UIKit.h>
#import "XXshellCellBase.h"

NS_ASSUME_NONNULL_BEGIN
@interface XXtableViewShell : NSObject<UITableViewDataSource, UITableViewDelegate>

// [1] 目标tableView设置
- (instancetype) initWithTableView:(UITableView*)tableView;
- (void) shell:(UITableView*)tabelView;

// [2] section/row相关配置
- (void) setRowType:(NSString*)type;        // 如果是自定义的话,使用自定义cell的类名,使用默认类型可以不调用此函数
- (void) setHeaderHeight:(CGFloat)height;   // 统一设置header的高度
- (void) setfooterHeight:(CGFloat)height;   // 统一设置footer的高度
- (void) setRowHeight:(CGFloat)height;      // 统一设置row的高度(如果后续需要个性化,可以在自定义cell中新增一个 [-(NSInteger) height:(NSIndexPath)] 的接口)
- (void) setHeaderHeight:(CGFloat)headerHeight RowHeight:(CGFloat)rowHeight FooterHeight:(CGFloat)footerHeight;

// [3] 使用默认section/row样式设置
- (void) setHeaderTextColor:(UIColor*)textColor Font:(UIFont*)font;

// [3] 设置cell事件回调
- (void) setCellEventHandle:(void(^)(NSString *event, NSDictionary *params))block;  // 供cell使用的事件回调,event:触发的事件,params:事件携带参数

// [4] 数据设置
- (void) setTableData:(NSArray*)data;       // 设置tableView数据
@end

NS_ASSUME_NONNULL_END
