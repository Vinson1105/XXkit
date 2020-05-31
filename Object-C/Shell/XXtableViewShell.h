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
 
 在使用自定义cell时，需要实现遵循协议<XXtableViewCellDelegate>
    nib自定义时，[awakeFromNib]中初始化
    code自定义时，[initWithStyle: reuseIdentifier:]中初始化
 
 在使用系统cell是，可以使用以下的'标识'来设置对应的值
    @"Title"：           UITableViewCell.textLabel.text
    @"Detail"：          UITableViewCell.detailTextLabel.text
    @"Image"：           UITableViewCell.imageView.image
    @"AccessoryType"：   UITableViewCell.accessoryType
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

typedef enum : NSUInteger {
    XXtableViewShellRowLoadTypeNib,
    XXtableViewShellRowLoadTypeCode,
} XXtableViewShellRowLoadType;

@interface XXtableViewShell : NSObject<UITableViewDelegate,UITableViewDataSource>
@property (nonatomic,weak,readonly) UITableView *tableView;                     // 目标UITableView
@property (nonatomic,strong,readonly) NSMutableArray *sectionDatas;             // TableView数据
@property (nonatomic,copy,nullable) NSString *rowType;                          // row（cell）的类型，nil为使用系统组件
@property (nonatomic,assign,readonly) XXtableViewShellRowLoadType rowLoadType;  // 自定义row（cell）的加载方式
@property (nonatomic,assign,readonly) UITableViewCellStyle rowSystemStyle;      // 系统row（cell）的样式


@property (nonatomic,copy,nullable) void(^onRowClicked)(XXtableViewShell *shell, NSIndexPath *indexPath, id data);          // row点击回调
@property (nonatomic,copy,nullable) BOOL(^onRowEditingDelete)(XXtableViewShell *shell, NSIndexPath *indexPath, id data);    // editing状态下，row删除编辑回调，通过返回NO：取消删除，返回YES：确认删除

/**
 设置shell的目标TableView
 @param tableView 目标的TableView
 */
- (void)shell:(UITableView*)tableView;

/**
 配置TableView的row（cell）参数
 @param type TableView的row（cell）的类型，自定义则传入自定义的类名；系统则传入nil
 @param loadType TableView的row（cell）的自定义方式，有【xib、code】两种类型，使用系统类型该参数传入无效
 @param systemStyle 使用系统的row（cell）时，可以指定系统样式，使用自定义类型该参数传入无效
 @param height row（cell）的高度，0：自适应，否则指定该高度
 */
- (void)configRowType:(nullable NSString*)type loadType:(XXtableViewShellRowLoadType)loadType systemStyle:(UITableViewCellStyle)systemStyle height:(CGFloat)height;

/**
 配置TableView的所有section的数据，调用后会触发TableView的刷新，headers和footers可以为nil，若两者不为nil时，则长度需要与rows数量相同，rows的数量视作为TableView的section数量；
 @param headers 当中元素是每一个section的header的数据
 @param rows 当中元素是每一个section的row的数据，如果该section的row数量为0
 @param footers 当中元素是每一个section的footer的数据
 */
- (void)configSectionWithHeaders:(nullable NSArray*)headers rows:(NSArray*)rows footers:(nullable NSArray*)footers;

/**
 配置单个TableView的section数据，调用后不会触发TableView的刷新，需要配合【XXtableViewShell configFinished】使用
 @param header section的header数据
 @param row section的row数据，为nil时，该section的row数量为0
 @param footer section的footer数据
 */
- (void)configSectionWithHeader:(nullable id)header row:(nullable NSArray*)row footer:(nullable id)footer;

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
- (void)addSectionWithHeader:(nullable id)header row:(nullable NSArray*)row footer:(nullable id)footer;

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
 重置指定indexPath的row数据
 @param data 单个row（cell）的数据
 @param indexPath 需要重置row（cell）的位置
 */
- (void)resetData:(id)data atIndexPath:(NSIndexPath*)indexPath;

/**
 移除指定indexPath的row
 @param indexPath 需要删除row的位置
 */
- (void)removeRowAtIndexPath:(NSIndexPath*)indexPath;

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
@property (nonatomic,weak) XXtableViewShell *tableViewShell;
@property (nonatomic,strong) NSIndexPath *indexPath;
- (void)resetData:(id)data;
- (void)doSomething:(NSString*)event info:(nullable id)info;
@end
NS_ASSUME_NONNULL_END
