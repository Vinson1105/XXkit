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
#import "XXshellCellBase.h"

NS_ASSUME_NONNULL_BEGIN
@interface XXtableViewShell : XXshell<UITableViewDataSource, UITableViewDelegate>
@property (nonatomic,assign) CGFloat headerHeight;
@property (nonatomic,assign) CGFloat footerHeight;
@property (nonatomic,assign) CGFloat rowHeight;

@property (nonatomic,strong) UIColor *headerTextColor;
@property (nonatomic,strong) UIFont *headerTextFont;

@property (nonatomic,copy,nullable) void(^onTouchEvent)(NSIndexPath *indexPath, id _Nullable param);

- (void) setClassHeaderType:(NSString*)headerType;
- (void) setNIbHeaderType:(NSString*)headerType;
@end

NS_ASSUME_NONNULL_END
