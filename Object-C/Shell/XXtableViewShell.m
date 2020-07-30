#import "XXtableViewShell.h"

#define kReuseRowDefault    @"ReuseRowDefault"
#define kReuseHeaderDefault @"ReuseHeaderDefault"
#define kReuseFooterDefault @"ReuseFooterDefault"

#define kHeader @"Header"
#define kRow @"Row"
#define kFooter @"Footer"

#define kTitle @"Title"
#define kDetail @"Detail"
#define kImage @"Image"
#define kAccessoryType @"AccessoryType"


@implementation XXtableViewShell
#pragma mark - <Init>
- (instancetype)init{
    self = [super init];
    if (self) {
        _sectionDatas   = [NSMutableArray new];
        _cellSystemStyle = UITableViewCellStyleDefault;
        _cellClass        = nil;
        _canSlideDelete = NO;
        _cellSelectionStyle = UITableViewCellSelectionStyleNone;
    }
    return self;
}

#pragma mark - <Config>
- (void)shell:(UITableView*)tableView{
    if(nil != _tableView) return;
    _tableView = tableView;
    _tableView.delegate = self;
    _tableView.dataSource = self;
    _tableView.estimatedRowHeight = 30;//_rowHeight;
}
- (void)configCellClass:(NSString*)cls loadType:(XXtableViewShellLoadType)loadType height:(CGFloat)height{
    _cellClass = cls;
    _cellLoadType = loadType;
    
    if(XXtableViewShellLoadTypeNib == loadType){
        [_tableView registerNib:[UINib nibWithNibName:_cellClass bundle:nil] forCellReuseIdentifier:_cellClass];
    }
    else if(XXtableViewShellLoadTypeCode == loadType){
        [_tableView registerClass:NSClassFromString(_cellClass) forCellReuseIdentifier:_cellClass];
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
- (void)configCellSystemStyle:(UITableViewCellStyle)style height:(CGFloat)height{
    _cellSystemStyle = style;
    _cellClass = nil;
    if(height <= 0){
        _tableView.estimatedRowHeight = 30;
    }
    else{
        _tableView.rowHeight = height;
    }
}
- (void)configHeaderClass:(NSString*)cls loadType:(XXtableViewShellLoadType)loadType height:(CGFloat)height{
    self.headerClass = cls;
    _headerLoadType = loadType;
    _headerHeight = height;
}
- (void)configFooterClass:(NSString*)cls loadType:(XXtableViewShellLoadType)loadType height:(CGFloat)height{
    self.footerClass = cls;
    _footerLoadType = loadType;
    _footerHeight = height;
}

- (void)configSectionHeaders:(nullable NSArray*)headers rows:(NSArray*)rows footers:(nullable NSArray*)footers{
    int sectionCount = (int)rows.count;
    for (int index = 0; index < sectionCount; index++) {
        NSMutableDictionary *section = [NSMutableDictionary new];
        if(nil != headers) [section setObject:headers[index] forKey:kHeader];
        if(nil != footers) [section setObject:footers[index] forKey:kFooter];
        
        NSMutableArray *rowsDataOfOneSection = [[NSMutableArray alloc] initWithArray:rows[index]];
        [section setObject:rowsDataOfOneSection forKey:kRow];
        [_sectionDatas addObject:section];
    }
        
    [_tableView reloadData];
}
- (void)configSectionHeader:(nullable id)header row:(nullable NSArray*)row footer:(nullable id)footer{
    NSMutableDictionary *section = [NSMutableDictionary new];
    if(nil != header) [section setObject:header forKey:kHeader];
    if(nil != footer) [section setObject:footer forKey:kFooter];
    if(nil != row) {
        NSMutableArray *rowsDataOfOneSection = [[NSMutableArray alloc] initWithArray:row];
        [section setObject:rowsDataOfOneSection forKey:kRow];
    }
    [_sectionDatas addObject:section];
}
- (void)configFinished{
    [_tableView reloadData];
}

#pragma mark - <Section>
- (void)addSectionHeader:(nullable id)header row:(nullable NSArray*)row footer:(nullable id)footer{
    NSMutableDictionary *section = [NSMutableDictionary new];
    if(nil != header) [section setObject:header forKey:kHeader];
    if(nil != footer) [section setObject:footer forKey:kFooter];
    if(nil != row) [section setObject:row forKey:kRow];
    [_sectionDatas addObject:section];
    [_tableView reloadData];
}
- (void)insertSectionWithHeader:(nullable id)header row:(nullable NSArray*)row footer:(id)footer atIndex:(int)index{
    NSMutableDictionary *section = [NSMutableDictionary new];
    if(nil != header) [section setObject:header forKey:kHeader];
    if(nil != footer) [section setObject:footer forKey:kFooter];
    if(nil != row) [section setObject:row forKey:kRow];
    [_sectionDatas insertObject:section atIndex:index];
    [_tableView reloadData];
}
- (void)removeSectionAtIndex:(int)index{
    [_sectionDatas removeObjectAtIndex:index];
    [_tableView reloadData];
}

#pragma mark - <Row>
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
        [sectionData removeObjectForKey:kRow];
    }
    else{
        [sectionData setObject:row forKey:kRow];
    }
    [_tableView reloadData];
}
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
    
    NSMutableArray *localRow = sectionData[kRow];
    if(nil == localRow){
        sectionData[kRow] = [[NSMutableArray alloc] initWithArray:row];
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

#pragma mark - <Private>
- (nullable NSMutableArray*)getRowWithSection:(NSUInteger)section{
    return [[_sectionDatas objectAtIndex:section] objectForKey:kRow];
}
- (id)getRowDataWithSection:(NSUInteger)section row:(NSUInteger)row{
    NSMutableArray *rows = [self getRowWithSection:section];
    return [rows objectAtIndex:row];
}
- (nullable id)getHeaderWithSection:(NSUInteger)section{
    return [[_sectionDatas objectAtIndex:section] objectForKey:kHeader];
}
- (nullable id)getFooterWithSection:(NSUInteger)section{
    return [[_sectionDatas objectAtIndex:section] objectForKey:kFooter];
}

#pragma mark - <UITableViewDataSource>
- (nonnull UITableViewCell *)tableView:(nonnull UITableView *)tableView cellForRowAtIndexPath:(nonnull NSIndexPath *)indexPath {
    UITableViewCell *cell       = nil;
    BOOL isSystem               = nil == _cellClass;
    
    /// 获取重用cell
    if (isSystem) {
        cell = [tableView dequeueReusableCellWithIdentifier:kReuseRowDefault];
    }
    else{
        id<XXtableViewCellDelegate> xxcell  = [tableView dequeueReusableCellWithIdentifier:_cellClass forIndexPath:indexPath];
        xxcell.tableViewShell       = self;
        xxcell.indexPath            = indexPath;
        cell                        = (UITableViewCell*)xxcell;
    }
    
    /// 无可重用的cell
    if (nil == cell) {
        if(isSystem){
            cell = [[UITableViewCell alloc] initWithStyle:_cellSystemStyle reuseIdentifier:kReuseRowDefault];
        }
        else{
            id<XXtableViewCellDelegate> xxcell  = [[NSClassFromString(_cellClass) alloc] initWithReuseIdentifier:_cellClass];
            xxcell.tableViewShell       = self;
            xxcell.indexPath            = indexPath;
            cell                        = (UITableViewCell*)xxcell;
        }
        cell.selectionStyle = _cellSelectionStyle;
    }
    if(nil == cell){
        NSLog(@"[XXtableViewShell] Failure to create cell.");
        return cell;
    }
    
    /// 获取对应的row（cell）的数据
    NSMutableArray *rowsData = [[_sectionDatas objectAtIndex:indexPath.section] objectForKey:kRow];
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
    if(nil!=_headerClass){
        /// 如果有使用自定义header，则直接返回nil
        return nil;
    }
    
    NSDictionary *sectionData = _sectionDatas[section];
    id headerData = [sectionData objectForKey:kHeader];
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
    if(nil!=_headerClass){
        /// 如果有使用自定义footer，则直接返回nil
        return nil;
    }
    
    NSDictionary *sectionData = _sectionDatas[section];
    id headerData = [sectionData objectForKey:kFooter];
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
    NSArray *rowsData = [sectionData objectForKey:kRow];
    if(nil == rowsData){
        return 0;
    }
    return rowsData.count;
}
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView{
    return _sectionDatas.count;
}
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath{
    return _canSlideDelete || tableView.isEditing;
}
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath{
    if(editingStyle == UITableViewCellEditingStyleDelete){
        BOOL toDelete = YES;
        if(_onRowEditingDelete){
            id data = [self getRowDataWithSection:(int)indexPath.section row:(int)indexPath.row];
            toDelete = _onRowEditingDelete(self,indexPath,data);
        }
        
        if(toDelete){
            [self removeRowAtIndexPath:indexPath];
        }
    }
}
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath{
    return YES;
}

#pragma mark - <UITableViewDelegate>
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    if(_onRowClicked){
        NSMutableArray *rowsData    = [[_sectionDatas objectAtIndex:indexPath.section] objectForKey:kRow];
        id rowData                  = [rowsData objectAtIndex:indexPath.row];
        _onRowClicked(self, indexPath, rowData);
    }
}
- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section{
    if(nil==_headerClass){
        return nil;
    }
    
    /// 取出对应header的数据
    id headerData = [self getHeaderWithSection:section];
    
    /// 获取复用header或者创建新的view
    UIView *view = [tableView dequeueReusableHeaderFooterViewWithIdentifier:_headerClass];
    if(nil==view){
        if(XXtableViewShellLoadTypeNib == _headerLoadType){
            view = [[[NSBundle mainBundle] loadNibNamed:_headerClass owner:nil options:nil].lastObject initWithReuseIdentifier:_headerClass];
        }
        else if(XXtableViewShellLoadTypeCode == _headerLoadType){
            view = [[NSClassFromString(_headerClass) alloc] initWithReuseIdentifier:_headerClass];
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
    [xxheader resetData:headerData];
    xxheader.indexPath = [NSIndexPath indexPathForRow:-1 inSection:section];
    
    return view;
}
- (UIView *)tableView:(UITableView *)tableView viewForFooterInSection:(NSInteger)section{
    if(nil==_footerClass){
        return nil;
    }
    
    /// 取出对应header的数据
    id footerData = [self getFooterWithSection:section];
    
    /// 获取复用header或者创建新的view
    UIView *view = [tableView dequeueReusableHeaderFooterViewWithIdentifier:_footerClass];
    if(nil==view){
        if(XXtableViewShellLoadTypeNib == _footerLoadType){
            view = [[[NSBundle mainBundle] loadNibNamed:_footerClass owner:nil options:nil].lastObject initWithReuseIdentifier:_footerClass];
        }
        else if(XXtableViewShellLoadTypeCode == _footerLoadType){
            view = [[NSClassFromString(_footerClass) alloc] initWithReuseIdentifier:_footerClass];
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
    [xxfooter resetData:footerData];
    xxfooter.indexPath = [NSIndexPath indexPathForRow:-1 inSection:section];
    
    return view;
}
- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    /// TODO: 不同的Row又不同的高度，其中返回-1可以使用自适应
    return -1;
}
- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section{
    /// TODO: 不同的Header又不同的高度，其中返回-1可以使用自适应
    return -1;
}
- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section{
    /// TODO: 不同的Footer又不同的高度，其中返回-1可以使用自适应
    return -1;
}
@end

