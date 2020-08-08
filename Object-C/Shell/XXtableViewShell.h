/**
 2020.05.28
 1、新增row的editing状态删除处理、回调
 
 2020.05.10
 1、新增对section中的row数据追加接口
 2、addRow和resetRow的参数section为当前section编号的最大值+1，则创建一个新section
 
 2020.04.20
 1、新增[XXtableViewShell resetData:(id)data atIndexPath:(NSIndexPath*)indexPath]，用于重置对应row
 2、新增<XXtableViewCellDelegate>，主要是规范自定义cell的接口
 3、对于[cell resetData:data]中的data，如果是需要在cell中对数据进行修改，则这个data需要是Mutable，这可能跟原生的设计有点出入
    （原生在数据修改是由Table这层控制的，但是XXtable可能是偏向的cell中管理自己的数据）
 
 2020.04.08
 UITableView的封装（第三版），集成以下功能
 1、内部管理cell（header、row、footer）
 2、动态增删section
 
 -- CELL --
 在使用自定义cell时，需要实现遵循协议<XXtableViewCellDelegate>
    nib自定义时，[awakeFromNib]中初始化
    code自定义时，[initWithStyle: reuseIdentifier:]中初始化
 
 在使用系统cell是，可以使用以下的'标识'来设置对应的值
    Title - UITableViewCell.textLabel.text
    Detail -  UITableViewCell.detailTextLabel.text
    Image - UITableViewCell.imageView.image
    AccessoryType -  UITableViewCell.accessoryType
    Height - 用于某一行高度，若没有该键值，则使用通过config设置的rowHeight
 
 -- HEADER/FOOTER --
 不太建议使用约束动态更改高度，建议使用heightForHeader、heightForFooter返回对其动态修改
 
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
@property (nonatomic,copy,nullable) NSString *headerClass;
@property (nonatomic,assign,readonly) XXtableViewShellLoadType headerLoadType;
@property (nonatomic,assign,readonly) CGFloat headerHeight;

#pragma mark - Property For Footer
@property (nonatomic,copy,nullable) NSString *footerClass;
@property (nonatomic,assign,readonly) XXtableViewShellLoadType footerLoadType;
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
