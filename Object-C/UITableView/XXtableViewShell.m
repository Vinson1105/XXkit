#import "XXtableViewShell.h"

#define kReuseRowDefault @"ReuseRowDefault"
#define kReuseHeaderDefault @"ReuseHeaderDefault"
#define kReuseFooterDefault @"ReuseFooterDefault"

#define kHeader @"Header"
#define kRow @"Row"
#define kFooter @"Footer"

#define kTitle @"Title"
#define kDetail @"Detail"
#define kImage @"Image"
#define kAccessoryType @"AccessoryType"

@interface XXtableViewShell()
@end

@implementation XXtableViewShell
#pragma mark - <初始化>
- (instancetype)init{
    self = [super init];
    if (self) {
        _sectionDatas   = [NSMutableArray new];
        _rowSystemStyle = UITableViewCellStyleDefault;
        _rowType        = nil;
        //_rowHeight = -1;
    }
    return self;
}

#pragma mark - <Public>
- (void)shell:(UITableView*)target{
    if(nil != _target) return;
    _target = target;
    _target.delegate = self;
    _target.dataSource = self;
    _target.estimatedRowHeight = 30;//_rowHeight;
}
- (void)configRowType:(nullable NSString*)type loadType:(XXtableViewShellRowLoadType)loadType systemStyle:(UITableViewCellStyle)systemStyle height:(CGFloat)height{
    _rowType = type;
    _rowLoadType = loadType;
    _rowSystemStyle = systemStyle;
    
    if(nil != type){
        if(XXtableViewShellRowLoadTypeNib == loadType){
            [_target registerNib:[UINib nibWithNibName:_rowType bundle:nil] forCellReuseIdentifier:_rowType];
        }
        else if(XXtableViewShellRowLoadTypeCode == loadType){
            [_target registerClass:NSClassFromString(_rowType) forCellReuseIdentifier:_rowType];
        }
        else{
            
        }
    }
    
    if(height <= 0){
        _target.estimatedRowHeight = 30;
    }
    else{
        _target.rowHeight = height;
    }
}
- (void)configSectionWithHeaders:(nullable NSArray*)headers rows:(NSArray*)rows footers:(nullable NSArray*)footers{
    
    int sectionCount = (int)rows.count;
    for (int index = 0; index < sectionCount; index++) {
        NSMutableDictionary *section = [NSMutableDictionary new];
        if(nil != headers) [section setObject:headers[index] forKey:kHeader];
        if(nil != footers) [section setObject:footers[index] forKey:kFooter];
        
        NSMutableArray *rowsDataOfOneSection = [[NSMutableArray alloc] initWithArray:rows[index]];
        [section setObject:rowsDataOfOneSection forKey:kRow];
        [_sectionDatas addObject:section];
    }
        
    [_target reloadData];
}
- (void)configSectionWithHeader:(nullable id)header row:(nullable NSArray*)row footer:(nullable id)footer{
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
    [_target reloadData];
}

#pragma mark - <Public>
- (void)addSectionWithHeader:(nullable id)header row:(nullable NSArray*)row footer:(nullable id)footer{
    NSMutableDictionary *section = [NSMutableDictionary new];
    if(nil != header) [section setObject:header forKey:kHeader];
    if(nil != footer) [section setObject:footer forKey:kFooter];
    if(nil != row) [section setObject:row forKey:kRow];
    [_sectionDatas addObject:section];
    [_target reloadData];
}
- (void)insertSectionWithHeader:(nullable id)header row:(nullable NSArray*)row footer:(id)footer atIndex:(int)index{
    NSMutableDictionary *section = [NSMutableDictionary new];
    if(nil != header) [section setObject:header forKey:kHeader];
    if(nil != footer) [section setObject:footer forKey:kFooter];
    if(nil != row) [section setObject:row forKey:kRow];
    [_sectionDatas insertObject:section atIndex:index];
    [_target reloadData];
}
- (void)removeSectionAtIndex:(int)index{
    [_sectionDatas removeObjectAtIndex:index];
    [_target reloadData];
}
- (void)resetRow:(nullable NSArray*)row atSection:(int)section{
    NSMutableDictionary *sectionData = [_sectionDatas objectAtIndex:section];
    if(nil == sectionData){
        return;
    }
    if(nil == row){
        [sectionData removeObjectForKey:kRow];
    }
    else{
        [sectionData setObject:row forKey:kRow];
    }
    [_target reloadData];
}

#pragma mark - <UITableViewDataSource>
- (nonnull UITableViewCell *)tableView:(nonnull UITableView *)tableView cellForRowAtIndexPath:(nonnull NSIndexPath *)indexPath {
    UITableViewCell *cell       = nil;
    BOOL isSystem               = nil == _rowType;
    
    /// 获取重用cell
    if (isSystem) {
        cell = [tableView dequeueReusableCellWithIdentifier:kReuseRowDefault];
    }
    else{
        id<XXtableViewCellDelegate> xxcell  = [tableView dequeueReusableCellWithIdentifier:_rowType forIndexPath:indexPath];
        xxcell.tableViewShell       = self;
        xxcell.indexPath            = indexPath;
        cell                        = (UITableViewCell*)xxcell;
    }
    
    /// 无可重用的cell
    if (nil == cell) {
        if(isSystem){
            cell = [[UITableViewCell alloc] initWithStyle:_rowSystemStyle reuseIdentifier:kReuseRowDefault];
        }
        else{
            id<XXtableViewCellDelegate> xxcell  = [[NSClassFromString(_rowType) alloc] initWithReuseIdentifier:_rowType];
            xxcell.tableViewShell       = self;
            xxcell.indexPath            = indexPath;
            cell                        = (UITableViewCell*)xxcell;
        }
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
        [xxcell reset:rowData];
    }
    return cell;
}
- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section{
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

#pragma mark - <UITableViewDelegate>
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    if(_onRowClicked){
        NSMutableArray *rowsData    = [[_sectionDatas objectAtIndex:indexPath.section] objectForKey:kRow];
        id rowData                  = [rowsData objectAtIndex:indexPath.row];
        _onRowClicked(indexPath, rowData);
    }
}
@end

