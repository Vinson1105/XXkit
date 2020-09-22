/**
 ===== Demo =====
 XXtableViewShell *tableViewShell = [XXtableViewShell new];
 [tableViewShell shell:self.tableView];
 
 ===== Cell =====
 1 在使用自定义cell时，需要实现遵循协议<XXtableViewCellDelegate>，并可以通过config配置rowHeight为-1时，可以通过计算自定义Cell约束高度，
 并可以修改约束去动态更新Cell的高度；
    nib自定义时，[awakeFromNib]中初始化；
    code自定义时，[initWithStyle: reuseIdentifier:]中初始化；
 
    Height - 用于某一行高度，若没有该键值，则使用通过config设置的height；
 
 2 在使用系统cell是，可以使用以下的'键'来初始化，并可以通过config配置rowHeight为-1时，若titlelabel有换行，可以根据Cell中titlelabel内容自适应高度
    Title - UITableViewCell.textLabel.text
    Detail -  UITableViewCell.detailTextLabel.text
    Image - UITableViewCell.imageView.image
    AccessoryType -  UITableViewCell.accessoryType
    Height - 用于某一行高度，若没有该键值，则使用通过config设置的height
  
 ===== Section Header/Footer =====
 1 Header/Footer没有对应方法去动态刷新，reloadData、reloadSection、reloadRows都不能触发viewForHeader/Footer；
    不太建议在Header/Footer动态修改高度，但是生成时初始化高度还是可以使用自适应；
 
 2 在使用自定义Header/Footer，需要继承UITableViewHeaderFooterView，并遵循协议<XXtableViewCellDelegate>；
 
 3 在使用系统Header/Footer，可以使用以下的'键'来初始化
    Title - 通过titleForHeader、titleForFooter返回显示
 
 ===== 关于Shell和Section Header/Footer/Row之间的调用关系 =====
 Header/Footer/Row的数据由自身修改发起者，Shell为数据的最终存放点，即当Header/Footer/Row有数据修改的需求时，需要将数据更新到Shell中；
 当数据修改的同时，有约束更新的需求同时，Row可以在内部直接reload，而Header/Footer则不能亦无法很好地动态刷新；（暂时没有找到方法）
 
 // TODO: 目前需要完善的功能
 1 row/header/footer如何修改自身数据
    - 引用数据表（在JSON数据输入时如何把数据转换到Mutable）
    - 回调到shell中
    - 通知
 2 不同row/header/footer可以使用不同的自定义类型，是以一整个section为单位？还是每一个row都可以设置？？
 3 供调用者使用的接口简化，调用者进行row/header/footer部分数据更新
    reset - 重置
    update - 部分覆盖
 4 JSON数据接口，用户数据和配置数据的划分？
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

typedef enum : NSUInteger {
    XXtableViewShellLoadTypeNib,
    XXtableViewShellLoadTypeCode,
} XXtableViewShellLoadType;

extern NSString * const kXXtableViewShellKeySectionHeader;
extern NSString * const kXXtableViewShellKeySectionRow;
extern NSString * const kXXtableViewShellKeySectionFooter;

extern NSString * const kXXtableViewShellKeyTitle;
extern NSString * const kXXtableViewShellKeyDetail;
extern NSString * const kXXtableViewShellKeyImage;
extern NSString * const kXXtableViewShellKeyAccessoryType;
extern NSString * const kXXtableViewShellKeyHeight;

@interface XXtableViewShell : NSObject<UITableViewDelegate,UITableViewDataSource>
#pragma mark - Shell的基本属性
/** 目标UITableView */
@property (nonatomic,weak,readonly) UITableView *tableView;
/** TableView数据 */
@property (nonatomic,strong,readonly) NSMutableArray *sectionDatas;

#pragma mark - SectionRow的相关属性
/** SectionRow的自定义类型，为nil时，使用则使用系统UITableViewCell */
@property (nonatomic,copy,nullable) NSString *sectionRowClass;
/** 自定义SectionRow的加载方式，有nib和code两种方式 */
@property (nonatomic,assign,readonly) XXtableViewShellLoadType sectionRowLoadType;
/** SectionRow的统一高度 */
@property (nonatomic,assign,readonly) CGFloat sectionRowHeight;
/** 系统SectionRow的样式 */
@property (nonatomic,assign,readonly) UITableViewCellStyle sectionRowSystemStyle;
/** SectionRow的选中方式，默认是none */
@property (nonatomic,assign) UITableViewCellSelectionStyle sectionRowSelectionStyle;
/** SectionRow是否可以通过滑动删除，当设置tableView.edit=YES之后无效 */
@property (nonatomic,assign) BOOL sectionRowSlideDeletable;

#pragma mark - SectionHeader的相关属性
/** SectionHeader的自定义类型，nil使用则使用系统SectionHeader */
@property (nonatomic,copy,nullable) NSString *sectionHeaderClass;
/** 自定义SectionHeader的加载方式，有nib和code两种方式 */
@property (nonatomic,assign,readonly) XXtableViewShellLoadType sectionHeaderLoadType;
/** SectionHeader的统一高度，设置0可以使用单次自适应（注意不能动态自适应） */
@property (nonatomic,assign,readonly) CGFloat sectionHeaderHeight;

#pragma mark - SectionFooter的相关属性
/** SectionFooter的自定义类型，nil使用则使用系统SectionFooter */
@property (nonatomic,copy,nullable) NSString *sectionFooterClass;
/** 自定义SectionFooter的加载方式，有nib和code两种方式 */
@property (nonatomic,assign,readonly) XXtableViewShellLoadType sectionFooterLoadType;
/** SectionFooter的统一高度，设置-1可以使用单次自适应（注意不能动态自适应） */
@property (nonatomic,assign,readonly) CGFloat sectionFooterHeight;

#pragma mark - SectionRow的相关事件回调
/** SectionRow点击回调，其中data是当前SectionRow的数据 */
@property (nonatomic,copy,nullable) void(^onSectionRowClicked)(XXtableViewShell *shell, NSIndexPath *indexPath, id data);
/** SectionRow删除回调，通过返回NO：取消删除，返回YES：确认删除并清除shell中数据，其中data是当前SectionRow的数据 */
@property (nonatomic,copy,nullable) BOOL(^onSectionRowEditingDelete)(XXtableViewShell *shell, NSIndexPath *indexPath, id data);
/** SectionRow的事件回调，一般是自定义SectionRow发起的事件回调，其中event是事件标识，data是当前SectionRow的数据，info是携带的额外信息 */
@property (nonatomic,copy,nullable) void(^onSectionRowEvent)(XXtableViewShell *shell, NSIndexPath *indexPath, NSString *event, id data);

#pragma mark - SectionHeader的相关事件回调
/** SectionHeader的事件回调，一般是自定义SectionHeader发起的事件回调，其中event是事件标识，data是当前SectionHeader的数据，info是携带的额外信息 */
@property (nonatomic,copy,nullable) void(^onSectionHeaderEvent)(XXtableViewShell *shell, NSUInteger section, NSString *event, id data);

#pragma mark - SectionFooter的相关事件回调
/** SectionFooter的事件回调，一般是自定义SectionFooter发起的事件回调，其中event是事件标识，data是当前SectionFooter的数据，info是携带的额外信息 */
@property (nonatomic,copy,nullable) void(^onSectionFooterEvent)(XXtableViewShell *shell, NSUInteger section, NSString *event, id data);

#pragma mark - shell的初始化
/**
 设置shell的目标TableView
 @param tableView 目标的TableView
 */
- (void)shell:(UITableView*)tableView;

#pragma mark - SectionRow的配置
/**
 配置TableView的自定义SectionRow参数
 @param cls SectionRow的自定义类型
 @param loadType 自定义SectionRow的加载方式
 @param height 全局SectionRow高度，-1：自适应，否则指定该高度
*/
- (void)configSectionRowClass:(NSString*)cls loadType:(XXtableViewShellLoadType)loadType height:(CGFloat)height;

/**
 配置TableView的系统SectionRow
 @param style 系统SectionRow的样式
 @param height 全局SectionRow高度，-1：自适应，否则指定该高度
 */
- (void)configSectionRowSystemStyle:(UITableViewCellStyle)style height:(CGFloat)height;

#pragma mark - SectionHeader的配置
/**
 配置TableView的自定义SectionHeader
 @param cls 自定义SectionHeader的类名
 @param loadType 自定义SectionHeader的加载方式
 @param height 全局SectionHeader的高度，-1：单次自适应，否则指定该高度
 */
- (void)configSectionHeaderClass:(NSString*)cls loadType:(XXtableViewShellLoadType)loadType height:(CGFloat)height;

#pragma mark - SectionFooter的配置
/**
 配置TableView的自定义SectionFooter
 @param cls 自定义SectionFooter的类名
 @param loadType 自定义SectionFooter的加载方式
 @param height 全局SectionFooter的高度，-1：单次自适应，否则指定该高度
 */
- (void)configSectionFooterClass:(NSString*)cls loadType:(XXtableViewShellLoadType)loadType height:(CGFloat)height;

#pragma mark - 初始数据的配置
/**
 配置TableView的所有section的数据，调用后会触发TableView的刷新，headers和footers可以为nil，若两者不为nil时，则长度需要与rows数量相同，rows的数量视作为TableView的section数量；
 @param headers 当中元素是每一个section的header的数据
 @param rows 当中元素是每一个section的row的数据，如果该section的row数量为0
 @param footers 当中元素是每一个section的footer的数据
 */
- (void)configSectionHeaders:(nullable NSArray*)headers rows:(NSArray*)rows footers:(nullable NSArray*)footers;

/**
 配置/增加单个TableView的section数据，调用后不会触发TableView的刷新，需要配合【XXtableViewShell configFinished】使用
 @param header section的header数据
 @param row section的row数据，为nil时，该section的row数量为0
 @param footer section的footer数据
 */
- (void)configSectionHeader:(nullable id)header row:(nullable NSArray*)row footer:(nullable id)footer;

/**
 配置完成，触发刷新
 */
- (void)configFinished;

/**
 循环配置Table的数据
 @param block 用于循环配置数据，
            形参type，Row、Header、Footer的其中一种，用于指示当前配置的类型，参考kXXtableViewShellKeySectionHeader/Row/Footer
            形参indexPath，所在的位置，Header和Footer的indexPath只有section值有效；
            返回值，返回NSNull切换下一个type进行配置，返回nil完成配置
 */
- (void)configDataUsingBlock:(id(^)(NSString *type, NSIndexPath *indexPath))block;

#pragma mark - Section的增删改查
/**
 在最后追加一个section
 @param header section的header数据
 @param row section的row数据，为nil时，该section的row数量为0
 @param footer section的footer数据
 */
- (void)addSectionHeader:(nullable id)header row:(nullable NSArray*)row footer:(nullable id)footer;

/**
 在指定位置插入一个section
 @param header section的header数据
 @param row section的row数据，为nil时，该section的row数量为0
 @param footer section的footer数据
 */
- (void)insertSectionWithHeader:(nullable id)header row:(nullable NSArray*)row footer:(id)footer atIndex:(int)index;

/**
 移除指定位置的section
 @param index 目标section的位置
 */
- (void)removeSectionAtIndex:(int)index;

/**
 修改指定的Section数据
 @param header SectionHeader的数据
 @param row SectionRow的数据
 @param footer SectionFooter的数据
 @param index 指定的SectionIndex
 */
- (void)setSectionHeader:(nullable id)header row:(nullable NSArray*)row footer:(nullable id)footer atIndex:(NSInteger)index;

/**
 获取指定SectionData，并同步使用Block返回
 @param index 指定的SectionIndex
 @param ret 同步回调的block
 */
- (void)getSectionSyncAtIndex:(NSInteger)index ret:(void(^)(id header, NSArray *row, id footer))ret;

#pragma mark - Section中某一个或者多个Row的增删改查
/**
 在指定index中的增Section加若干个Row
 @param rows 追加的row的数据
 @param index 目标section的位置
 */
- (void)addSectionRows:(NSArray*)rows atIndex:(NSInteger)index;

/**
 在指定index中的Section中删除所有Row
 @param index 指定的Section的index
 */
- (void)removeSectionRowsAtIndex:(NSInteger)index;

/**
 删除指定indexPath的SectionRow
 @param indexPath 需要删除SectionRow的位置
 */
- (void)removeSectionRowAtIndexPath:(NSIndexPath*)indexPath;

/**
 设置指定indexPath的SectionRow数据
 @param row 单个SectionRow的数据
 @param indexPath 需要重置row（cell）的位置
 */
- (void)setSectionRow:(id)row atIndexPath:(NSIndexPath*)indexPath;

/**
 在指定index的Section设置所有Row的数据
 @param rows 重置后的数据，为nil时，该section的row数量为0
 @param index 目标section的位置
 */
- (void)setSectionRows:(nullable NSArray*)rows atIndex:(NSInteger)index;

/**
 获取指定indexPath的Row数据
 @param indexPath 需要重置row（cell）的位置
 */
- (id)getSectionRowAtIndexPath:(NSIndexPath*)indexPath;

#pragma mark - SectionHeader/SectionRow/SectionFooter的执行任务
/**
 要求row进行某些操作，这个跟resetData有所区别，resetData意指需要重置或者设置row的本身数据，
 doSomething需要执行某些动作，shell的本身不会对data进行修改，需要又row/cell本身去判断这个操作是否需要进行data修改
 @param event 需要执行的事件名称
 @param info 执行需要携带的参数
 @param indexPath cell所在的位置
 */
- (void)rowDoSomething:(NSString*)event info:(nullable id)info atIndex:(NSIndexPath*)indexPath;
@end


@protocol XXtableViewCellDelegate
@required
@property (nonatomic,copy,nullable) void(^onEvent)(id<XXtableViewCellDelegate> obj, NSString *event, id info);
@property (nonatomic,weak) XXtableViewShell *tableViewShell;
@property (nonatomic,strong) NSIndexPath *indexPath;
- (void)resetData:(id)data;
- (void)event:(NSString*)event info:(nullable id)info;
@end
NS_ASSUME_NONNULL_END
