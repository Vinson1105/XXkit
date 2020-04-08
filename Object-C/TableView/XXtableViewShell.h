/**
 * 2020-04-08
 * UITableView的封装（第三版），集成以下功能
 * 1、内部管理cell（header、row、footer）
 * 2、动态增删section
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface XXtableViewShell : NSObject<UITableViewDelegate,UITableViewDataSource>
@property (nonatomic,weak,readonly) UITableView *target;
@property (nonatomic,copy,nullable) NSString *rowType;      // 元素类型，为nil
@property (nonatomic,copy,nullable) NSString *headerType;   // 页头类型
@property (nonatomic,copy,nullable) NSString *footerType;   // 页脚类型
@property (nonatomic,assign) int rowHeight;                 // 设置行高，默认是自适应

/**
 * @brief 设置shell的目标TableView
 * @param target 目标的TableView
 */
- (void)shell:(UITableView*)target;

/**
 * @brief 配置TableView的所有section的数据，调用后会触发TableView的刷新
 * @param headers 当中元素是每一个section的header的数据
 * @param rows 当中元素是每一个section的row的数据，如果该section的row数量为0
 * @param footers 当中元素是每一个section的footer的数据
 *
 * headers和footers可以为nil，若两者不为nil时，则长度需要与rows数量相同，rows的数量视作为TableView的section数量；
 *
 */
- (void)configSectionWithHeaders:(nullable NSArray*)headers rows:(NSArray*)rows footers:(nullable NSArray*)footers;

/**
 * @brief 配置单个TableView的section数据，调用后不会触发TableView的刷新，需要配合【XXtableViewShell configFinished】使用
 * @param header section的header数据
 * @param row section的row数据，为nil时，该section的row数量为0
 * @param footer section的footer数据
 */
- (void)configSectionWithHeader:(nullable id)header row:(nullable NSArray*)row footer:(nullable id)footer;

/**
 * @brief 配置完成，触发刷新
 */
- (void)configFinished;

/**
 * @brief 在最后追加一个section
 * @param header section的header数据
 * @param row section的row数据，为nil时，该section的row数量为0
 * @param footer section的footer数据
 */
- (void)addSectionWithHeader:(nullable id)header row:(nullable NSArray*)row footer:(nullable id)footer;

/**
 * @brief 在指定位置插入一个section
 * @param header section的header数据
 * @param row section的row数据，为nil时，该section的row数量为0
 * @param footer section的footer数据
 */
- (void)insertSectionWithHeader:(nullable id)header row:(nullable NSArray*)row footer:(id)footer atIndex:(int)index;

/**
 * @brief 移除指定位置的section
 * @param index 目标section的位置
 */
- (void)removeSectionAtIndex:(int)index;

/**
 * @brief 重置指定section的row数据
 * @param row 重置后的数据，为nil时，指示该section的row数量为0
 * @param section 目标section的位置
 */
- (void)resetRow:(nullable NSArray*)row atSection:(int)section;
@end

NS_ASSUME_NONNULL_END
