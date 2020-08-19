/**
 -- Cell --
 1 在使用自定义cell时，需要实现遵循协议<XXtableViewCellDelegate>，并可以通过config配置rowHeight为-1时，可以通过计算自定义Cell约束高度，
 并可以修改约束去动态更新Cell的高度
    nib自定义时，[awakeFromNib]中初始化
    code自定义时，[initWithStyle: reuseIdentifier:]中初始化
 
    Height - 用于某一行高度，若没有该键值，则使用通过config设置的height
 
 2 在使用系统cell是，可以使用以下的'键'来初始化，并可以通过config配置rowHeight为-1时，若titlelabel有换行，可以根据Cell中titlelabel内容自适应高度
    Title - UITableViewCell.textLabel.text
    Detail -  UITableViewCell.detailTextLabel.text
    Image - UITableViewCell.imageView.image
    AccessoryType -  UITableViewCell.accessoryType
    Height - 用于某一行高度，若没有该键值，则使用通过config设置的height
  
 -- Section Header/Footer --
 1 Header/Footer没有对应方法去动态刷新，reloadData、reloadSection、reloadRows都不能触发viewForHeader/Footer,
    不太建议在Header/Footer动态修改高度，但是生成时初始化高度还是可以使用自适应
 
 2 在使用自定义Header/Footer，需要继承UITableViewHeaderFooterView，并遵循协议<XXtableViewCellDelegate>
 
 3 在使用系统Header/Footer，可以使用以下的'键'来初始化
    Title - 通过titleForHeader、titleForFooter返回显示
 
 // TODO: 目前需要完善的功能
 1 cell/header/footer如何修改自身数据
    - 引用数据表（在JSON数据输入时如何把数据转换到Mutable）
    - 回调到shell中
    - 通知
 2 不同row/header/footer可以使用不同的自定义类型，是以一整个section为单位？还是每一个row都可以设置？
 3 供调用者使用的接口简化，调用者进行row/header/footer部分数据更新
    reset - 重置
    update - 部分覆盖
 4 JSON数据接口，用户数据和配置数据的划分
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

typedef enum : NSUInteger {
    XXtableViewShellLoadTypeNib,
    XXtableViewShellLoadTypeCode,
} XXtableViewShellLoadType;

@interface XXtableViewShell : NSObject<UITableViewDelegate,UITableViewDataSource>
/** 目标UITableView */
@property (nonatomic,weak,readonly) UITableView *tableView;
/** TableView数据 */
@property (nonatomic,strong,readonly) NSMutableArray *sectionDatas;

#pragma mark - Property For Cell
/** cell的自定义类型，nil使用则使用系统cell */
@property (nonatomic,copy,nullable) NSString *cellClass;
/** 自定义cell的加载方式，有nib和code两种方式 */
@property (nonatomic,assign,readonly) XXtableViewShellLoadType cellLoadType;
/** 系统cell的样式 */
@property (nonatomic,assign,readonly) UITableViewCellStyle cellSystemStyle;
/** cell的选中方式，默认是none */
@property (nonatomic,assign) UITableViewCellSelectionStyle cellSelectionStyle;
/** cell是否可以通过滑动删除，当设置tableView.edit之后无效 */
@property (nonatomic,assign) BOOL canSlideDelete;

#pragma mark - Property For Header
/** sectionHeader的自定义类型，nil使用则使用系统sectionHeader */
@property (nonatomic,copy,nullable) NSString *headerClass;
/** 自定义sectionHeader的加载方式，有nib和code两种方式 */
@property (nonatomic,assign,readonly) XXtableViewShellLoadType headerLoadType;
/** sectionHeader的统一高度，设置-1可以使用自适应 */
@property (nonatomic,assign,readonly) CGFloat headerHeight;

#pragma mark - Property For Footer
/** sectionFooter的自定义类型，nil使用则使用系统sectionFooter */
@property (nonatomic,copy,nullable) NSString *footerClass;
/** 自定义sectionFooter的加载方式，有nib和code两种方式 */
@property (nonatomic,assign,readonly) XXtableViewShellLoadType footerLoadType;
/** sectionFooter的统一高度，设置-1可以使用自适应 */
@property (nonatomic,assign,readonly) CGFloat footerHeight;

#pragma mark - Property For TableView Handle Block
/** 点击回调，其中data是当前cell设置的数据，其类型由调用者输入时确定 */
@property (nonatomic,copy,nullable) void(^onRowClicked)(XXtableViewShell *shell, NSIndexPath *indexPath, id data);
/** 删除回调，通过返回NO：取消删除，返回YES：确认删除并清除shell中数据 */
@property (nonatomic,copy,nullable) BOOL(^onRowEditingDelete)(XXtableViewShell *shell, NSIndexPath *indexPath, id data);

#pragma mark - Property For Cell/Header/Footer Event Handle Block
@property (nonatomic,copy,nullable) void(^onCellEvent)(XXtableViewShell *shell, NSIndexPath *indexPath, NSString *event, id data);
@property (nonatomic,copy,nullable) void(^onHeaderEvent)(XXtableViewShell *shell, NSUInteger section, NSString *event, id data);
@property (nonatomic,copy,nullable) void(^onFooterEvent)(XXtableViewShell *shell, NSUInteger section, NSString *event, id data);

/**
 设置shell的目标TableView
 @param tableView 目标的TableView
 */
- (void)shell:(UITableView*)tableView;

/**
 配置TableView的自定义cell参数
 @param cls cell的自定义类型
 @param loadType 自定义cell的加载方式
 @param height 全局cell高度，0：自适应，否则指定该高度
*/
- (void)configCellClass:(NSString*)cls loadType:(XXtableViewShellLoadType)loadType height:(CGFloat)height;

/**
 配置TableView的系统cell
 @param style 系统cell的样式
 @param height 全局cell高度，0：自适应，否则指定该高度（可以通过）
 */
- (void)configCellSystemStyle:(UITableViewCellStyle)style height:(CGFloat)height;

/**
 配置TableView的自定义header
 @param cls 自定义header的类名
 @param loadType 自定义header的加载方式
 @param height 全局header的高度（不建议使用更改约束自适应动态修改高度）
 */
- (void)configHeaderClass:(NSString*)cls loadType:(XXtableViewShellLoadType)loadType height:(CGFloat)height;

/**
 配置TableView的自定义footer
 @param cls 自定义footer的类名
 @param loadType 自定义footer的加载方式
 @param height 全局footer的高度（不建议使用更改约束自适应动态修改高度）
 */
- (void)configFooterClass:(NSString*)cls loadType:(XXtableViewShellLoadType)loadType height:(CGFloat)height;


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
 重置指定section的中所有row数据
 @param row 重置后的数据，为nil时，该section的row数量为0
 @param section 目标section的位置
 */
- (void)resetRow:(nullable NSArray*)row atSection:(int)section;

/**
 在指定section中增加若干个row
 @param row 追加的row的数据
 @param section 目标section的位置
 */
- (void)addRow:(NSArray*)row atSection:(int)section;

/**
 移除指定indexPath的row
 @param indexPath 需要删除row的位置
 */
- (void)removeRowAtIndexPath:(NSIndexPath*)indexPath;

/**
 重置指定indexPath的row数据
 @param data 单个row（cell）的数据
 @param indexPath 需要重置row（cell）的位置
 */
- (void)resetData:(id)data atIndexPath:(NSIndexPath*)indexPath;
/**
 获取指定indexPath的row数据
 @param indexPath 需要重置row（cell）的位置
 */
- (id)getDataAtIndexPath:(NSIndexPath*)indexPath;

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
- (void)doSomething:(NSString*)event info:(nullable id)info;
@end
NS_ASSUME_NONNULL_END
