#import "XXtableViewShell.h"

#define kReuseRowDefault    @"ReuseRowDefault"
#define kReuseHeaderDefault @"ReuseHeaderDefault"
#define kReuseFooterDefault @"ReuseFooterDefault"

NSString * const kXXtableViewShellKeySectionHeader    = @"Header";
NSString * const kXXtableViewShellKeySectionRow       = @"Row";
NSString * const kXXtableViewShellKeySectionFooter    = @"Footer";

NSString * const kXXtableViewShellKeyTitle              = @"Title";
NSString * const kXXtableViewShellKeyDetail             = @"Detail";
NSString * const kXXtableViewShellKeyImage              = @"Image";
NSString * const kXXtableViewShellKeyAccessoryType      = @"AccessoryType";
NSString * const kXXtableViewShellKeyHeight             = @"Height";

@implementation XXtableViewShell
#pragma mark - shell的初始化
- (instancetype)init{
    self = [super init];
    if (self) {
        _sectionDatas   = [NSMutableArray new];
        _sectionRowSystemStyle = UITableViewCellStyleDefault;
        _sectionRowClass        = nil;
        _sectionRowSlideDeletable = NO;
        _sectionRowSelectionStyle = UITableViewCellSelectionStyleNone;
        
        _sectionFooterHeight = -1;
        _sectionHeaderHeight = -1;
        _sectionRowHeight = -1;
    }
    return self;
}
- (void)dealloc{
    if(self.noContentView){
        [self.noContentView removeFromSuperview];
    }
}
- (void)shell:(UITableView*)tableView{
    if(nil != _tableView) return;
    _tableView = tableView;
    _tableView.delegate = self;
    _tableView.dataSource = self;
    _tableView.estimatedRowHeight = 30;
}

#pragma mark - SectionRow的配置
- (void)configSectionRowClass:(NSString*)cls loadType:(XXtableViewShellLoadType)loadType height:(CGFloat)height{
    _sectionRowClass = cls;
    _sectionRowLoadType = loadType;
    _sectionRowHeight = height;
    
    if(XXtableViewShellLoadTypeNib == loadType){
        [_tableView registerNib:[UINib nibWithNibName:_sectionRowClass bundle:nil] forCellReuseIdentifier:_sectionRowClass];
    }
    else if(XXtableViewShellLoadTypeCode == loadType){
        [_tableView registerClass:NSClassFromString(_sectionRowClass) forCellReuseIdentifier:_sectionRowClass];
    }
    else{
        
    }
    
    if(height <= 0){
        _tableView.estimatedRowHeight = 30;
    }
    else{
        _tableView.rowHeight = height;
    }
}
- (void)configSectionRowSystemStyle:(UITableViewCellStyle)style height:(CGFloat)height{
    _sectionRowSystemStyle = style;
    _sectionRowClass = nil;
    _sectionRowHeight = height;
    
    if(height <= 0){
        _tableView.estimatedRowHeight = 30;
    }
    else{
        _tableView.rowHeight = height;
    }
}

#pragma mark - SectionHeader的配置
- (void)configSectionHeaderClass:(NSString*)cls loadType:(XXtableViewShellLoadType)loadType height:(CGFloat)height{
    self.sectionHeaderClass = cls;
    _sectionHeaderLoadType = loadType;
    _sectionHeaderHeight = height;
    
    if(XXtableViewShellLoadTypeNib == loadType){
        [_tableView registerNib:[UINib nibWithNibName:cls bundle:nil] forHeaderFooterViewReuseIdentifier:cls];
    }
    else if(XXtableViewShellLoadTypeCode == loadType){
        [_tableView registerClass:NSClassFromString(cls) forHeaderFooterViewReuseIdentifier:cls];
    }
    else{
        
    }
    if(height <= 0){
        _tableView.estimatedSectionHeaderHeight = UITableViewAutomaticDimension;
    }
    else{
        _tableView.estimatedSectionHeaderHeight = height;
        _tableView.sectionHeaderHeight = height;
    }
}

#pragma mark - SectionFooter的配置
- (void)configSectionFooterClass:(NSString*)cls loadType:(XXtableViewShellLoadType)loadType height:(CGFloat)height{
    self.sectionFooterClass = cls;
    _sectionFooterLoadType = loadType;
    _sectionFooterHeight = height;
    
    if(XXtableViewShellLoadTypeNib == loadType){
        [_tableView registerNib:[UINib nibWithNibName:cls bundle:nil] forHeaderFooterViewReuseIdentifier:cls];
    }
    else if(XXtableViewShellLoadTypeCode == loadType){
        [_tableView registerClass:NSClassFromString(cls) forHeaderFooterViewReuseIdentifier:cls];
    }
    else{
        
    }
    if(height <= 0){
        _tableView.estimatedSectionFooterHeight = UITableViewAutomaticDimension;
    }
    else{
        _tableView.estimatedSectionFooterHeight = height;
        _tableView.sectionFooterHeight = height;
    }
}

#pragma mark - 空内容时显示的配置
- (void)configNoContentImage:(nullable UIImage*)image
                   imageSize:(CGSize)imageSize
                       title:(nullable NSString*)title
                  titleColor:(nullable UIColor*)titleColor
                   titleFont:(nullable UIFont*)titleFont{
    UIView *view = nil;
    if(nil == image){
        
    }
    else if(nil == title){
        view = [self setupDefaultNoContentViewWithImage:image imageSize:imageSize];
    }
    else{
        if(nil == titleColor) titleColor = UIColor.grayColor;
        if(nil == titleFont) titleFont = [UIFont systemFontOfSize:13];
        view = [self setupDefaultNoContentViewWithImage:image
                                              imageSize:imageSize
                                                  title:title
                                             titleColor:titleColor
                                              titleFont:titleFont];
    }
    [self configNoContentView:view];
}
- (void)configNoContentClass:(NSString*)cls loadType:(XXtableViewShellLoadType)loadType{
    UIView *view = nil;
    switch (loadType) {
        case XXtableViewShellLoadTypeNib:
            view = [[NSBundle mainBundle] loadNibNamed:cls owner:nil options:nil].firstObject;
            break;
        case XXtableViewShellLoadTypeCode:
            view = [NSClassFromString(cls) new];
            break;
        default:
            break;
    }
    [self configNoContentView:view];
}
- (void)configNoContentView:(UIView*)view{
    if(nil == view || nil == self.tableView){
        return;
    }
    [self.tableView addSubview:view];
    view.translatesAutoresizingMaskIntoConstraints = NO;
    self.tableView.translatesAutoresizingMaskIntoConstraints = NO;
    [view.centerXAnchor constraintEqualToAnchor:self.tableView.centerXAnchor].active = YES;
    [view.centerYAnchor constraintEqualToAnchor:self.tableView.centerYAnchor].active = YES;
    [view.widthAnchor constraintEqualToAnchor:self.tableView.widthAnchor multiplier:0.5].active = YES;
    view.hidden = 0 != self.sectionDatas.count;
    _noContentView = view;
}

#pragma mark - 初始数据的配置
- (void)configSectionHeaders:(nullable NSArray*)headers rows:(NSArray*)rows footers:(nullable NSArray*)footers{
    int sectionCount = (int)rows.count;
    for (int index = 0; index < sectionCount; index++) {
        NSMutableDictionary *section = [NSMutableDictionary new];
        if(nil != headers) [section setObject:headers[index] forKey:kXXtableViewShellKeySectionHeader];
        if(nil != footers) [section setObject:footers[index] forKey:kXXtableViewShellKeySectionFooter];
        
        NSMutableArray *rowsDataOfOneSection = [[NSMutableArray alloc] initWithArray:rows[index]];
        [section setObject:rowsDataOfOneSection forKey:kXXtableViewShellKeySectionRow];
        [_sectionDatas addObject:section];
    }
        
    [_tableView reloadData];
}
- (void)configSectionHeader:(nullable id)header row:(nullable NSArray*)row footer:(nullable id)footer{
    NSMutableDictionary *section = [NSMutableDictionary new];
    if(nil != header) [section setObject:header forKey:kXXtableViewShellKeySectionHeader];
    if(nil != footer) [section setObject:footer forKey:kXXtableViewShellKeySectionFooter];
    if(nil != row) {
        NSMutableArray *rowsDataOfOneSection = [[NSMutableArray alloc] initWithArray:row];
        [section setObject:rowsDataOfOneSection forKey:kXXtableViewShellKeySectionRow];
    }
    [_sectionDatas addObject:section];
}
- (void)configFinished{
    [_tableView reloadData];
}
- (void)configDataUsingBlock:(id(^)(NSString *type, NSIndexPath *indexPath))block{
    [self.sectionDatas removeAllObjects];
    NSInteger section = 0;
    NSInteger row     = 0;
    
    while (1) {
        NSMutableDictionary *sectionData = [NSMutableDictionary new];
        
        // header
        row = 0;
        id data = block(kXXtableViewShellKeySectionHeader, [NSIndexPath indexPathForRow:row inSection:section]);
        if(nil == data){
            break;
        }
        if([NSNull null] != data){
            sectionData[kXXtableViewShellKeySectionHeader] = data;
        }
        
        // row
        row = 0;
        NSMutableArray *rows = [NSMutableArray new];
        BOOL isNil = NO;
        while (1) {
            id data = block(kXXtableViewShellKeySectionRow, [NSIndexPath indexPathForRow:row inSection:section]);
            if(nil == data){
                isNil = YES;
                break;
            }
            if([NSNull null] == data){
                break;
            }
            [rows addObject:data];
            row++;
        }
        if(isNil){
            break;
        }
        sectionData[kXXtableViewShellKeySectionRow] = rows;
        
        // footer
        row = 0;
        data = block(kXXtableViewShellKeySectionFooter, [NSIndexPath indexPathForRow:row inSection:section]);
        if(nil == data){
            break;
        }
        if([NSNull null] != data){
            sectionData[kXXtableViewShellKeySectionFooter] = data;
        }
        
        [self.sectionDatas addObject:sectionData];
        ++section;
    }
}

#pragma mark - Section的增删改查
- (void)addSectionHeader:(nullable id)header row:(nullable NSArray*)row footer:(nullable id)footer{
    NSMutableDictionary *section = [NSMutableDictionary new];
    if(nil != header) [section setObject:header forKey:kXXtableViewShellKeySectionHeader];
    if(nil != footer) [section setObject:footer forKey:kXXtableViewShellKeySectionFooter];
    if(nil != row) [section setObject:row forKey:kXXtableViewShellKeySectionRow];
    [_sectionDatas addObject:section];
    [_tableView reloadData];
}
- (void)insertSectionWithHeader:(nullable id)header row:(nullable NSArray*)row footer:(id)footer atIndex:(int)index{
    NSMutableDictionary *section = [NSMutableDictionary new];
    if(nil != header) [section setObject:header forKey:kXXtableViewShellKeySectionHeader];
    if(nil != footer) [section setObject:footer forKey:kXXtableViewShellKeySectionFooter];
    if(nil != row) [section setObject:row forKey:kXXtableViewShellKeySectionRow];
    [_sectionDatas insertObject:section atIndex:index];
    [_tableView reloadData];
}
- (void)removeSectionAtIndex:(int)index{
    [_sectionDatas removeObjectAtIndex:index];
    [_tableView reloadData];
}
- (void)removeAllSections{
    [self.sectionDatas removeAllObjects];
    [self.tableView reloadData];
}
- (void)setSectionHeader:(nullable id)header row:(nullable NSArray*)row footer:(nullable id)footer atIndex:(NSInteger)index{
    if(self.sectionDatas.count<=index){
        NSLog(@"[XXtableViewShell] [setSectionHeader] 无效index。（count:%d index:%d）", (int)self.sectionDatas.count, (int)index);
        return;
    }
    
    NSMutableDictionary *section = [NSMutableDictionary new];
    if(nil != header) [section setObject:header forKey:kXXtableViewShellKeySectionHeader];
    if(nil != footer) [section setObject:footer forKey:kXXtableViewShellKeySectionFooter];
    if(nil != row) [section setObject:row forKey:kXXtableViewShellKeySectionRow];
    
    self.sectionDatas[index] = section;
}
- (void)getSectionSyncAtIndex:(NSInteger)index ret:(void (^)(id _Nonnull, NSArray * _Nonnull, id _Nonnull))ret{
    if(self.sectionDatas.count<=index){
        NSLog(@"[XXtableViewShell] [setSectionHeader] 无效index。（count:%d index:%d）", (int)self.sectionDatas.count, (int)index);
        return;
    }
    
    NSMutableDictionary *section = self.sectionDatas[index];
    id header = section[kXXtableViewShellKeySectionHeader];
    NSArray *row = section[kXXtableViewShellKeySectionRow];
    id footer = section[kXXtableViewShellKeySectionFooter];
    
    if(ret){
        ret(header, row, footer);
    }
}

#pragma mark - Section中某一个或者多个Row的增删改查
- (void)addSectionRows:(NSArray*)row atIndex:(NSInteger)index{
    NSMutableDictionary *sectionData = index < self.sectionDatas.count?self.sectionDatas[index]:nil;
    if(nil == sectionData && row && row.count>0){
        if(index == self.sectionDatas.count){
            /// 指定的Section为当前最大Section编号+1，则直接创建一个新Section（Section是有序的）
            sectionData = [NSMutableDictionary new];
            self.sectionDatas[index] = sectionData;
        }
        else{
            /// 指定的section并不是最大section编号+1，暂时不支持创建多个中间section
            return;
        }
    }
    if(nil == sectionData){
        /// 当SectionData为nil，表明了Shell中没有index对应的Section，并当前参数row为nil，无需对数据进行修改
        return;
    }
    
    NSMutableArray *localRow = sectionData[kXXtableViewShellKeySectionRow];
    if(nil == localRow){
        sectionData[kXXtableViewShellKeySectionRow] = [[NSMutableArray alloc] initWithArray:row];
    }
    else{
        [localRow addObjectsFromArray:row];
    }
    [self.tableView reloadData];
}
- (void)removeSectionRowsAtIndex:(NSInteger)index{
    if(self.sectionDatas.count<=index){
        NSLog(@"[XXtableViewShell] [setSectionHeader] 无效index。（count:%d index:%d）", (int)self.sectionDatas.count, (int)index);
        return;
    }
    
    NSMutableDictionary *sectionData = self.sectionDatas[index];
    [sectionData removeObjectForKey:kXXtableViewShellKeySectionRow];
    if(0 == sectionData.count){
        /// 当前SectionData移除了‘Row’的键值之后，已经再没有任何键值，则应该将该SectionData在列表中移除
        [self.sectionDatas removeObject:sectionData];
    }
    [self.tableView reloadData];
}
- (void)removeSectionRowAtIndexPath:(NSIndexPath*)indexPath{
    NSMutableArray *rows = [self getSectionRowsAtIndex:(int)indexPath.section];
    if(nil == rows){
        return;
    }
    if(indexPath.row>=0 && indexPath.row<rows.count){
        [rows removeObjectAtIndex:indexPath.row];
        [_tableView reloadData];
    }
}
- (void)setSectionRow:(id)data atIndexPath:(NSIndexPath*)indexPath{
    NSMutableArray *rows = [self getSectionRowsAtIndex:(int)indexPath.section];
    if(nil == rows){
        return;
    }
    [rows replaceObjectAtIndex:indexPath.row withObject:data];
    [_tableView reloadRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationAutomatic];
}
- (void)updateSectionRow:(NSDictionary*)row atIndexPath:(NSIndexPath*)indexPath{
    id rowData = [self getSectionRowAtIndexPath:indexPath];
    if(nil == rowData || ![rowData isKindOfClass:NSMutableDictionary.class]){
        return;
    }
    
    NSMutableDictionary *rowDataDict = rowData;
    [rowDataDict addEntriesFromDictionary:row];
    [self.tableView reloadRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationAutomatic];
}
- (void)updateSectionRow:(NSDictionary*)row key:(NSString*)key equelTo:(id)value atSectionIndex:(NSInteger)sectionIndex{
    NSMutableArray *rows = [self getSectionRowsAtIndex:sectionIndex];
    if(nil == rows){
        return;
    }
    
    BOOL needReload = NO;
    NSEnumerator *rowEnumer = rows.objectEnumerator;
    id obj = nil;
    while (nil != (obj = rowEnumer.nextObject)) {
        if(![obj isKindOfClass:NSMutableDictionary.class]){
            continue;
        }
        
        NSMutableDictionary *rowData = obj;
        [rowData addEntriesFromDictionary:row];
        needReload = YES;
    }
    
    if(needReload){
        [self.tableView reloadSections:[NSIndexSet indexSetWithIndex:sectionIndex] withRowAnimation:UITableViewRowAnimationAutomatic];
    }
}
- (void)updateSectionRow:(NSDictionary*)row key:(NSString*)key equelTo:(id)value{
    BOOL needReload = NO;
    NSEnumerator *sectionDataEnumer     = self.sectionDatas.objectEnumerator;
    NSMutableDictionary *sectionData    = nil;
    while (nil != (sectionData = sectionDataEnumer.nextObject)) {
        if(nil == sectionData[kXXtableViewShellKeySectionRow]){
            continue;
        }
        
        NSMutableArray *rowDatas = sectionData[kXXtableViewShellKeySectionRow];
        for (id rowData in rowDatas) {
            if(![rowData isKindOfClass:NSMutableDictionary.class]){
                continue;
            }
            
            NSMutableDictionary *rowDataDict = rowData;
            if(nil == rowDataDict[key] || ![rowDataDict[key] isEqual:value]){
                continue;
            }
            
            [rowDataDict addEntriesFromDictionary:row];
            needReload = YES;
        }
    }
    
    if(needReload){
        [self.tableView reloadData];
    }
}
- (void)setSectionRows:(nullable NSArray*)row atIndex:(NSInteger)section{
    NSMutableDictionary *sectionData = section < _sectionDatas.count?_sectionDatas[section]:nil;
    if(nil == sectionData){
        if(section == _sectionDatas.count && row && row.count>0){
            /// 指定的Section为当前最大Section编号+1，则直接创建一个新Section（Section是有序的）
            sectionData = [NSMutableDictionary new];
            _sectionDatas[section] = sectionData;
        }
        else{
            /// 指定的section并不是最大section编号+1，暂时不支持创建多个中间section
            return;
        }
    }
    if(nil == sectionData){
        /// 当SectionData为nil，表明了Shell中没有index对应的Section，并当前参数row为nil，无需对数据进行修改
        return;
    }
    
    if(nil == row){
        [sectionData removeObjectForKey:kXXtableViewShellKeySectionRow];
        if(0 == sectionData.count){
            /// 当前SectionData移除了‘Row’的键值之后，已经再没有任何键值，则应该将该SectionData在列表中移除
            [self.sectionDatas removeObject:sectionData];
        }
    }
    else{
        [sectionData setObject:row forKey:kXXtableViewShellKeySectionRow];
    }
    [self.tableView reloadData];
}
- (id)getSectionRowAtIndexPath:(NSIndexPath*)indexPath{
    NSMutableArray *rows = [self getSectionRowsAtIndex:(int)indexPath.section];
    if(nil == rows){
        return nil;
    }
    if(rows.count <= indexPath.row){
        return nil;
    }
    return rows[indexPath.row];
}

- (void)rowDoSomething:(NSString*)event info:(nullable id)info atIndex:(NSIndexPath*)indexPath{
    UITableViewCell *cell = [_tableView cellForRowAtIndexPath:indexPath];
    if(nil == cell){
        NSLog(@"[XXtableViewShell] [rowDoSomething] indexPath（%@）对应的cell为nil。", indexPath);
        return;
    }
    
    if(![cell conformsToProtocol:@protocol(XXtableViewCellDelegate)]){
        NSLog(@"[XXtableViewShell] [rowDoSomething] cell没有遵循协议‘XXtableViewCellDelegate’。");
        return;
    }
    
    id<XXtableViewCellDelegate> xxcell = (id<XXtableViewCellDelegate>)cell;
    [xxcell event:event info:info];
}

#pragma mark - 一些私有函数
- (nullable NSMutableArray*)getSectionRowsAtIndex:(NSUInteger)index{
    if(self.sectionDatas.count <= index){
        return nil;
    }
    return self.sectionDatas[index][kXXtableViewShellKeySectionRow];
}
- (id)getSectionRowAtSection:(NSUInteger)section row:(NSUInteger)row{
    NSMutableArray *rows = [self getSectionRowsAtIndex:section];
    return [rows objectAtIndex:row];
}
- (nullable id)getHeaderWithSection:(NSUInteger)section{
    return [[_sectionDatas objectAtIndex:section] objectForKey:kXXtableViewShellKeySectionHeader];
}
- (nullable id)getFooterWithSection:(NSUInteger)section{
    return [[_sectionDatas objectAtIndex:section] objectForKey:kXXtableViewShellKeySectionFooter];
}

#pragma mark - 一些私有函数
- (UIView*)setupDefaultNoContentViewWithImage:(UIImage*)image imageSize:(CGSize)imageSize{
    UIImageView *imageView = [[UIImageView alloc] initWithImage:image];
    imageView.translatesAutoresizingMaskIntoConstraints = NO;
    [imageView.widthAnchor constraintEqualToConstant:imageSize.width].active = YES;
    [imageView.heightAnchor constraintEqualToConstant:imageSize.height].active = YES;
    return imageView;
}
- (UIView*)setupDefaultNoContentViewWithImage:(UIImage*)image
                                    imageSize:(CGSize)imageSize
                                        title:(NSString*)title
                                   titleColor:(UIColor*)titleColor
                                    titleFont:(UIFont*)titleFont{
    UIView *view = [UIView new];
    view.translatesAutoresizingMaskIntoConstraints = NO;
    
    UIImageView *imageView = [[UIImageView alloc] initWithImage:image];
    imageView.translatesAutoresizingMaskIntoConstraints = NO;
    [imageView.widthAnchor constraintEqualToConstant:imageSize.width].active = YES;
    [imageView.heightAnchor constraintEqualToConstant:imageSize.height].active = YES;
    [view addSubview:imageView];
    [imageView.centerXAnchor constraintEqualToAnchor:view.centerXAnchor].active = YES;
    [imageView.topAnchor constraintEqualToAnchor:view.topAnchor].active = YES;
//    [imageView.leadingAnchor constraintGreaterThanOrEqualToAnchor:view.leadingAnchor].active = YES;
//    [imageView.trailingAnchor constraintGreaterThanOrEqualToAnchor:view.trailingAnchor].active = YES;
    
    UILabel *titleLabel = [UILabel new];
    titleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    titleLabel.font = titleFont;
    titleLabel.textColor = titleColor;
    titleLabel.text = title;
    titleLabel.numberOfLines = 0;
    [view addSubview:titleLabel];
    [titleLabel.topAnchor constraintEqualToAnchor:imageView.bottomAnchor constant:10].active = YES;
    [titleLabel.centerXAnchor constraintEqualToAnchor:view.centerXAnchor].active = YES;
    [titleLabel.bottomAnchor constraintEqualToAnchor:view.bottomAnchor].active = YES;
    [titleLabel.leadingAnchor constraintLessThanOrEqualToAnchor:view.leadingAnchor].active = YES;
    [titleLabel.trailingAnchor constraintLessThanOrEqualToAnchor:view.trailingAnchor].active = YES;
    return view;
}

#pragma mark - UITableViewDataSource
- (nonnull UITableViewCell *)tableView:(nonnull UITableView *)tableView cellForRowAtIndexPath:(nonnull NSIndexPath *)indexPath {
    UITableViewCell *cell       = nil;
    BOOL isSystem               = nil == _sectionRowClass;
    
    /// 获取重用cell
    if (isSystem) {
        cell = [tableView dequeueReusableCellWithIdentifier:kReuseRowDefault];
    }
    else{
        id<XXtableViewCellDelegate> xxcell  = [tableView dequeueReusableCellWithIdentifier:_sectionRowClass forIndexPath:indexPath];
        xxcell.tableViewShell       = self;
        xxcell.indexPath            = indexPath;
        cell                        = (UITableViewCell*)xxcell;
    }
    
    /// 无可重用的cell
    if (nil == cell) {
        if(isSystem){
            cell = [[UITableViewCell alloc] initWithStyle:_sectionRowSystemStyle reuseIdentifier:kReuseRowDefault];
        }
        else{
            id<XXtableViewCellDelegate> xxcell  = [[NSClassFromString(_sectionRowClass) alloc] initWithReuseIdentifier:_sectionRowClass];
            xxcell.tableViewShell       = self;
            xxcell.indexPath            = indexPath;
            cell                        = (UITableViewCell*)xxcell;
        }
        cell.selectionStyle = _sectionRowSelectionStyle;
    }
    if(nil == cell){
        NSLog(@"[XXtableViewShell] [cellForRowAtIndexPath] 无效的Cell。");
        return cell;
    }
    
    /// 获取对应的row（cell）的数据
    NSMutableArray *rowsData = [[_sectionDatas objectAtIndex:indexPath.section] objectForKey:kXXtableViewShellKeySectionRow];
    id rowData = [rowsData objectAtIndex:indexPath.row];
    if(nil == rowData){
        return cell;
    }
    
    /// 设置数据
    if(isSystem){
        if([rowData isKindOfClass:[NSString class]]){
            cell.textLabel.text = rowData;
        }
        else if([rowData isKindOfClass:[NSDictionary class]]){
            NSDictionary *dict = rowData;
            if(nil != [dict objectForKey:kXXtableViewShellKeyTitle]){
                cell.textLabel.text = dict[kXXtableViewShellKeyTitle];
            }
            if(nil != [dict objectForKey:kXXtableViewShellKeyDetail]){
                cell.detailTextLabel.text = dict[kXXtableViewShellKeyDetail];
            }
            if(nil != [dict objectForKey:kXXtableViewShellKeyImage]){
                cell.imageView.image = dict[kXXtableViewShellKeyImage];
            }
            if(nil != [dict objectForKey:kXXtableViewShellKeyAccessoryType]){
                cell.accessoryType = [dict[kXXtableViewShellKeyAccessoryType] intValue];
            }
        }
        else{
            NSLog(@"[XXtableViewShell] [cellForRowAtIndexPath] 未知SectionRow的数据类型（%@）。", NSStringFromClass([rowData class]));
        }
    }
    else{
        id<XXtableViewCellDelegate> xxcell = (id<XXtableViewCellDelegate>)cell;
        [xxcell resetData:rowData];
    }
    return cell;
}
- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section{
    if(nil!=_sectionHeaderClass){
        /// 如果有使用自定义则SectionHeader，则直接返回nil
        return nil;
    }
    
    NSDictionary *sectionData = _sectionDatas[section];
    id headerData = [sectionData objectForKey:kXXtableViewShellKeySectionHeader];
    if(nil == headerData){
        return nil;
    }
    else if([headerData isKindOfClass:[NSString class]]){
        return headerData;
    }
    else if([headerData isKindOfClass:[NSDictionary class]]){
        return [headerData objectForKey:kXXtableViewShellKeyTitle];
    }
    else{
        return nil;
    }
}
- (NSString *)tableView:(UITableView *)tableView titleForFooterInSection:(NSInteger)section{
    if(nil!=_sectionFooterClass){
        /// 如果有使用自定义SecionFooter，则直接返回nil
        return nil;
    }
    
    NSDictionary *sectionData = _sectionDatas[section];
    id headerData = [sectionData objectForKey:kXXtableViewShellKeySectionFooter];
    if(nil == headerData){
        return nil;
    }
    else if([headerData isKindOfClass:[NSString class]]){
        return headerData;
    }
    else if([headerData isKindOfClass:[NSDictionary class]]){
        return [headerData objectForKey:kXXtableViewShellKeyTitle];
    }
    else{
        return nil;
    }
}

- (NSInteger)tableView:(nonnull UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    NSMutableDictionary *sectionData = [_sectionDatas objectAtIndex:section];
    if(nil == sectionData){
        return 0;
    }
    NSArray *rowsData = [sectionData objectForKey:kXXtableViewShellKeySectionRow];
    if(nil == rowsData){
        return 0;
    }
    return rowsData.count;
}
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView{
    NSInteger count = nil==self.sectionDatas?0:self.sectionDatas.count;
    if(self.noContentView){
        self.noContentView.hidden = 0!=count;
    }
    return count;
}
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath{
    return _sectionRowSlideDeletable || tableView.isEditing;
}
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath{
    if(editingStyle == UITableViewCellEditingStyleDelete){
        BOOL toDelete = YES;
        if(_onSectionRowEditingDelete){
            id data = [self getSectionRowAtIndexPath:indexPath];
            toDelete = _onSectionRowEditingDelete(self,indexPath,data);
        }
        
        if(toDelete){
            [self removeSectionRowAtIndexPath:indexPath];
        }
    }
}
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath{
    return YES;
}

#pragma mark - UITableViewDelegate
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    if(_onSectionRowClicked){
        NSMutableArray *rowsData    = [[_sectionDatas objectAtIndex:indexPath.section] objectForKey:kXXtableViewShellKeySectionRow];
        id rowData                  = [rowsData objectAtIndex:indexPath.row];
        _onSectionRowClicked(self, indexPath, rowData);
    }
}
- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section{
    if(nil==_sectionHeaderClass){
        return nil;
    }
    
    /// 取出对应header的数据
    id headerData = [self getHeaderWithSection:section];
    
    /// 获取复用header或者创建新的view
    UIView *view = [tableView dequeueReusableHeaderFooterViewWithIdentifier:_sectionHeaderClass];
    if(nil==view){
        if(XXtableViewShellLoadTypeNib == _sectionHeaderLoadType){
            view = [[[NSBundle mainBundle] loadNibNamed:_sectionHeaderClass owner:nil options:nil].lastObject initWithReuseIdentifier:_sectionHeaderClass];
        }
        else if(XXtableViewShellLoadTypeCode == _sectionHeaderLoadType){
            view = [[NSClassFromString(_sectionHeaderClass) alloc] initWithReuseIdentifier:_sectionHeaderClass];
        }
        else{
            
        }
    }
    if(nil==view){
        NSLog(@"[XXtableViewShell] [viewForHeaderInSection] 创建HeaderView失败。");
        return nil;
    }
    
    /// view协议转换
    if(![view conformsToProtocol:@protocol(XXtableViewCellDelegate)]){
        NSLog(@"[XXtableViewShell] [viewForFooterInSection] cell没有遵循协议‘XXtableViewCellDelegate’。");
        return view;
    }
    id<XXtableViewCellDelegate> xxheader = (id<XXtableViewCellDelegate>)view;
    xxheader.indexPath = [NSIndexPath indexPathForRow:-1 inSection:section];
    [xxheader resetData:headerData];
    
    return view;
}
- (UIView *)tableView:(UITableView *)tableView viewForFooterInSection:(NSInteger)section{
    if(nil==_sectionFooterClass){
        return nil;
    }
    
    /// 取出对应header的数据
    id footerData = [self getFooterWithSection:section];
    
    /// 获取复用header或者创建新的view
    UIView *view = [tableView dequeueReusableHeaderFooterViewWithIdentifier:_sectionFooterClass];
    if(nil==view){
        if(XXtableViewShellLoadTypeNib == _sectionFooterLoadType){
            view = [[[NSBundle mainBundle] loadNibNamed:_sectionFooterClass owner:nil options:nil].lastObject initWithReuseIdentifier:_sectionFooterClass];
        }
        else if(XXtableViewShellLoadTypeCode == _sectionFooterLoadType){
            view = [[NSClassFromString(_sectionFooterClass) alloc] initWithReuseIdentifier:_sectionFooterClass];
        }
        else{
            
        }
    }
    if(nil==view){
        NSLog(@"[XXtableViewShell] [viewForFooterInSection] 创建FooterView失败。");
        return nil;
    }
    
    /// view协议转换
    if(![view conformsToProtocol:@protocol(XXtableViewCellDelegate)]){
        NSLog(@"[XXtableViewShell] [viewForFooterInSection] cell没有遵循协议‘XXtableViewCellDelegate’。");
        return view;
    }
    id<XXtableViewCellDelegate> xxfooter = (id<XXtableViewCellDelegate>)view;
    xxfooter.indexPath = [NSIndexPath indexPathForRow:-1 inSection:section];
    [xxfooter resetData:footerData];
    
    return view;
}
- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    /// 判断SectionRow中的Data是否为NSDictionary，且有‘Height’的键值时，直接使用该值，否则使用配置时的高度
    id rowData = [self getSectionRowAtIndexPath:indexPath];
    if(nil == rowData){
        return self.sectionRowHeight;
    }
    
    if([rowData isKindOfClass:[NSDictionary class]]){
        NSDictionary *data = rowData;
        if(nil != data[kXXtableViewShellKeyHeight]){
            return [data[kXXtableViewShellKeyHeight] floatValue];
        }
    }
    
    return self.sectionRowHeight;
}
- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section{
    /// 判断SectionHeader中的Data是否为NSDictionary，且有‘Height’的键值时，直接使用该值，否则使用配置时的高度
    id headerData = [self getHeaderWithSection:section];
    if(nil == headerData){
        return _sectionHeaderHeight;
    }
    
    if([headerData isKindOfClass:[NSDictionary class]]){
        NSDictionary *data = headerData;
        if(nil != data[kXXtableViewShellKeyHeight]){
            return [data[kXXtableViewShellKeyHeight] floatValue];
        }
    }
    return _sectionHeaderHeight;
}
- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section{
    /// 判断SectionFooter中的Data是否为NSDictionary，且有‘Height’的键值时，直接使用该值，否则使用配置时的高度
    id footerData = [self getFooterWithSection:section];
    if(nil == footerData){
        return _sectionFooterHeight;
    }
    if([footerData isKindOfClass:[NSDictionary class]]){
        NSDictionary *data = footerData;
        if(nil != data[kXXtableViewShellKeyHeight]){
            return [data[kXXtableViewShellKeyHeight] floatValue];
        }
    }
    return _sectionFooterHeight;
}
@end

