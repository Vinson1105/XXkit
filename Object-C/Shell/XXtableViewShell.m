#import "XXtableViewShell.h"

#define kReuseRowDefault    @"ReuseRowDefault"
#define kReuseHeaderDefault @"ReuseHeaderDefault"
#define kReuseFooterDefault @"ReuseFooterDefault"

//#define kHeader @"Header"
//#define kRow @"Row"
//#define kFooter @"Footer"

#define kTitle @"Title"
#define kDetail @"Detail"
#define kImage @"Image"
#define kAccessoryType @"AccessoryType"
#define kHeight @"Height"

NSString * const kXXtableViewShellKeySectionHeader    = @"Header";
NSString * const kXXtableViewShellKeySectionRow       = @"Row";
NSString * const kXXtableViewShellKeySectionFooter    = @"Footer";


@implementation XXtableViewShell
#pragma mark - <Init>
- (instancetype)init{
    self = [super init];
    if (self) {
        _sectionDatas   = [NSMutableArray new];
        _sectionRowSystemStyle = UITableViewCellStyleDefault;
        _sectionRowClass        = nil;
        _sectionRowSlideDeletable = NO;
        _sectionRowSelectionStyle = UITableViewCellSelectionStyleNone;
        
        _footerHeight = -1;
        _headerHeight = -1;
    }
    return self;
}

#pragma mark - shell的初始化
- (void)shell:(UITableView*)tableView{
    if(nil != _tableView) return;
    _tableView = tableView;
    _tableView.delegate = self;
    _tableView.dataSource = self;
    _tableView.estimatedRowHeight = 30;//_rowHeight;
}

#pragma mark - SectionRow的配置
- (void)configSectionRowClass:(NSString*)cls loadType:(XXtableViewShellLoadType)loadType height:(CGFloat)height{
    _sectionRowClass = cls;
    _sectionRowLoadType = loadType;
    
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
    _headerHeight = height;
    
    if(XXtableViewShellLoadTypeNib == loadType){
        [_tableView registerNib:[UINib nibWithNibName:cls bundle:nil] forHeaderFooterViewReuseIdentifier:cls];
    }
    else if(XXtableViewShellLoadTypeCode == loadType){
        [_tableView registerClass:NSClassFromString(cls) forHeaderFooterViewReuseIdentifier:cls];
    }
    else{
        
    }
    if(height <= 0){
        _tableView.estimatedSectionHeaderHeight = 0.01;
    }
    else{
        _tableView.sectionHeaderHeight = height;
    }
}

#pragma mark - SectionFooter的配置
- (void)configSectionFooterClass:(NSString*)cls loadType:(XXtableViewShellLoadType)loadType height:(CGFloat)height{
    self.sectionFooterClass = cls;
    _sectionFooterLoadType = loadType;
    _footerHeight = height;
    
    if(XXtableViewShellLoadTypeNib == loadType){
        [_tableView registerNib:[UINib nibWithNibName:cls bundle:nil] forHeaderFooterViewReuseIdentifier:cls];
    }
    else if(XXtableViewShellLoadTypeCode == loadType){
        [_tableView registerClass:NSClassFromString(cls) forHeaderFooterViewReuseIdentifier:cls];
    }
    else{
        
    }
    if(height <= 0){
        _tableView.estimatedSectionFooterHeight = 0.01;
    }
    else{
        _tableView.sectionFooterHeight = height;
    }
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

#pragma mark - Section中某一个或者多个Row的增删改查
- (void)addRow:(NSArray*)row atSection:(int)section{
    NSMutableDictionary *sectionData = section < _sectionDatas.count?_sectionDatas[section]:nil;
    if(nil == sectionData){
        if(section == _sectionDatas.count){
            /// 指定的section为当前最大section编号+1，则直接创建一个新section（section是有序的）
            sectionData = [NSMutableDictionary new];
            _sectionDatas[section] = sectionData;
        }
        else{
            /// 指定的section并不是最大section编号+1，暂时不支持创建多个中间section
            return;
        }
    }
    
    NSMutableArray *localRow = sectionData[kXXtableViewShellKeySectionRow];
    if(nil == localRow){
        sectionData[kXXtableViewShellKeySectionRow] = [[NSMutableArray alloc] initWithArray:row];
    }
    else{
        [localRow addObjectsFromArray:row];
    }
    [_tableView reloadData];
}
- (void)removeRowAtIndexPath:(NSIndexPath*)indexPath{
    NSMutableArray *rows = [self getRowWithSection:(int)indexPath.section];
    if(nil == rows){
        return;
    }
    if(indexPath.row>=0 && indexPath.row<rows.count){
        [rows removeObjectAtIndex:indexPath.row];
        [_tableView reloadData];
    }
}
- (void)resetData:(id)data atIndexPath:(NSIndexPath*)indexPath{
    NSMutableArray *rows = [self getRowWithSection:(int)indexPath.section];
    if(nil == rows){
        return;
    }
    [rows replaceObjectAtIndex:indexPath.row withObject:data];
    [_tableView reloadRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationNone];
}
- (void)resetRow:(nullable NSArray*)row atSection:(int)section{
    NSMutableDictionary *sectionData = section < _sectionDatas.count?_sectionDatas[section]:nil;
    if(nil == sectionData){
        if(section == _sectionDatas.count){
            /// 指定的section为当前最大section编号+1，则直接创建一个新section（section是有序的）
            sectionData = [NSMutableDictionary new];
            _sectionDatas[section] = sectionData;
        }
        else{
            /// 指定的section并不是最大section编号+1，暂时不支持创建多个中间section
            return;
        }
    }
    
    if(nil == row){
        [sectionData removeObjectForKey:kXXtableViewShellKeySectionRow];
    }
    else{
        [sectionData setObject:row forKey:kXXtableViewShellKeySectionRow];
    }
    [_tableView reloadData];
}
- (id)getDataAtIndexPath:(NSIndexPath*)indexPath{
    NSMutableArray *rows = [self getRowWithSection:(int)indexPath.section];
    if(nil == rows){
        return nil;
    }
    return [rows objectAtIndex:indexPath.row];
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
    [xxcell doSomething:event info:info];
}

#pragma mark - 一些私有函数
- (nullable NSMutableArray*)getRowWithSection:(NSUInteger)section{
    return [[_sectionDatas objectAtIndex:section] objectForKey:kXXtableViewShellKeySectionRow];
}
- (id)getRowDataWithSection:(NSUInteger)section row:(NSUInteger)row{
    NSMutableArray *rows = [self getRowWithSection:section];
    return [rows objectAtIndex:row];
}
- (nullable id)getHeaderWithSection:(NSUInteger)section{
    return [[_sectionDatas objectAtIndex:section] objectForKey:kXXtableViewShellKeySectionHeader];
}
- (nullable id)getFooterWithSection:(NSUInteger)section{
    return [[_sectionDatas objectAtIndex:section] objectForKey:kXXtableViewShellKeySectionFooter];
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
        NSLog(@"[XXtableViewShell] Failure to create cell.");
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
            if(nil != [dict objectForKey:kTitle]){
                cell.textLabel.text = dict[kTitle];
            }
            if(nil != [dict objectForKey:kDetail]){
                cell.detailTextLabel.text = dict[kDetail];
            }
            if(nil != [dict objectForKey:kImage]){
                cell.imageView.image = dict[kImage];
            }
            if(nil != [dict objectForKey:kAccessoryType]){
                cell.accessoryType = [dict[kAccessoryType] intValue];
            }
        }
        else{
            NSLog(@"[XXtableViewShell] Unknown type(%@) of row data(%@).", NSStringFromClass([rowData class]), rowData);
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
        /// 如果有使用自定义header，则直接返回nil
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
        return [headerData objectForKey:kTitle];
    }
    else{
        return nil;
    }
}
- (NSString *)tableView:(UITableView *)tableView titleForFooterInSection:(NSInteger)section{
    if(nil!=_sectionFooterClass){
        /// 如果有使用自定义footer，则直接返回nil
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
        return [headerData objectForKey:kTitle];
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
    return _sectionDatas.count;
}
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath{
    return _sectionRowSlideDeletable || tableView.isEditing;
}
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath{
    if(editingStyle == UITableViewCellEditingStyleDelete){
        BOOL toDelete = YES;
        if(_onSectionRowEditingDelete){
            id data = [self getRowDataWithSection:(int)indexPath.section row:(int)indexPath.row];
            toDelete = _onSectionRowEditingDelete(self,indexPath,data);
        }
        
        if(toDelete){
            [self removeRowAtIndexPath:indexPath];
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
        NSLog(@"[XXtableViewShell] [viewForHeaderInSection] failure to VIEW");
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
        NSLog(@"[XXtableViewShell] [viewForFooterInSection] failure to VIEW");
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
    return -1;
}
- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section{
    id headerData = [self getHeaderWithSection:section];
    if(nil == headerData){
        return _headerHeight;
    }
    if([headerData isKindOfClass:[NSDictionary class]]){
        NSDictionary *data = headerData;
        if(nil != data[kHeight]){
            return [data[kHeight] floatValue];
        }
    }
    return _headerHeight;
}
- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section{
    id footerData = [self getFooterWithSection:section];
    if(nil == footerData){
        return _footerHeight;
    }
    if([footerData isKindOfClass:[NSDictionary class]]){
        NSDictionary *data = footerData;
        if(nil != data[kHeight]){
            return [data[kHeight] floatValue];
        }
    }
    return _footerHeight;
}
@end

