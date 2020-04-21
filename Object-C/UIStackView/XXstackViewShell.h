/**
 * StackViewShell是一个UIStackView封装类，主要是管理StackView的元素动态删除和添加（注意：应用场景是动态）
 *
 * 2020-04-07
 * 1、添加必要性注释
 * 2、修改部分函数名
 */

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXstackViewShell : NSObject
@property (nonatomic,weak,readonly) UIStackView *target;
@property (nonatomic,strong,readonly) NSMutableDictionary<NSString*,UIView*> *nameToView;

/**
 * @brief 设置shell的目标StackView
 * @param target 目标的StackView
 */
- (void)shell:(UIStackView*)target;

/**
 * @brief 添加一个view到UIStackView中，如果StackView已有标签相同的view，将无法加入
 * @param view 需要添加的view
 * @param name 用于标识这个添加的view的标签
 */
- (void)addView:(UIView*)view forName:(NSString*)name;

/**
 * @brief 添加一个view到UIStackView中，如果StackView已有标签相同的view，将无法加入
 * @param view 需要添加的view
 * @param size 初始化view的高宽约束（一般是固定大小，动态大小需要外部设置后再addView，高度或者宽度可以单独设置-1，以不设置约束）
 * @param name 用于标识这个添加的view的标签
 */
- (void)addView:(UIView*)view size:(CGSize)size forName:(NSString*)name;

/**
 * @brief 在UIStackView删除一个标签为‘name’的view
 * @param name 需要删除的目标view的标签
 */
- (void)removeViewForName:(NSString*)name;

/**
 * @brief 删除UIStackView中的所有View
 */
- (void)removeViewAll;

/**
 * @brief 在UIStackView中的固定位置插入一个view
 * @param view 需要插入的目标view
 * @param name 目标view的标签
 * @param index 插入的位置
 */
- (void)insertView:(UIView*)view forName:(NSString*)name atIndex:(int)index;

/**
 * @brief 在UIStackView中的固定位置插入一个view
 * @param view 需要插入的目标view
 * @param size 初始化view的高宽约束（一般是固定大小，动态大小需要外部设置后再addView）
 * @param name 目标view的标签
 * @param index 插入的位置
*/
- (void)insertView:(UIView*)view size:(CGSize)size forName:(NSString*)name atIndex:(int)index;

/**
 * @brief 隐藏/显示一个在UIStackView中已有的view
 * @param hidden YES：隐藏，NO：显示
 * @param name 目标view的标签
 */
- (void)hidden:(BOOL)hidden forName:(NSString*)name;

/**
 * @brief 使能/禁能一个在UIStackView中已有的view（）
 * @param enable YES：使能，NO：禁能
 * @param name 目标view的标签
 */
- (void)enableUserInterface:(BOOL)enable forName:(NSString*)name;

/**
 * @brief 获取一个UIStackView已有的view
 * @param name 目标view的标签
 */
- (nullable UIView*)viewForName:(NSString*)name;
@end

NS_ASSUME_NONNULL_END
