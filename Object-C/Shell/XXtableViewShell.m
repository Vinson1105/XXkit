#import "XXtableViewShell.h"
#import "../XXocDefine.h"

@interface XXtableViewShell ()
@property (weak,nonatomic) UITableView          *tableView;
@property (nonatomic,copy) NSString             *sectionsType;      // section的类型
@property (nonatomic,assign) NSInteger          sectionCount;           // section的数量
@property (nonatomic,strong) NSMutableArray     *rowCountInSection;     // 每一个section中row的数量
@property (nonatomic,strong) NSMutableArray     *localData;            // 整个table的数据
@end

@implementation XXtableViewShell

- (void) initContext{
    _tableView = self.context;
    _tableView.delegate = self;
    _tableView.dataSource = self;
    
    _tableView.estimatedRowHeight           = 30;           // 预测行高
    _tableView.estimatedSectionHeaderHeight = 0;            // 预测页头高度
    _tableView.estimatedSectionFooterHeight = 0;            // 预测页脚高度
    _tableView.rowHeight = UITableViewAutomaticDimension;   // 自适应cell的高度
    
    switch (self.cellLoadType) {
        case XXcellLoadType_Default:
            break;
        case XXcellLoadType_Class:
            [_tableView registerClass:NSClassFromString(self.cellType) forCellReuseIdentifier:self.cellType];
            break;
        case XXcellLoadType_Nib:
            [_tableView registerNib:[UINib nibWithNibName:self.cellType bundle:nil] forCellReuseIdentifier:self.cellType];
            break;
        default:
            break;
    }
}
- (void) setClassHeaderType:(NSString*)headerType{
    // 自定义header
}
- (void) setNIbHeaderType:(NSString*)headerType{
    // 自定义header
}


#pragma mark - 数据设置
- (void) setData:(NSMutableArray *)xxdata{
    // [0] 重置数据
    _localData = nil;
    _sectionCount = 0;
    [_rowCountInSection removeAllObjects];
    
    // [1] 表数据
    if(nil == xxdata)
        return;
    _localData = [xxdata mutableCopy];
    
    // [2] section的数量
    _sectionCount = _localData.count;
    
    // [3] section下row的数量
    if (nil == _rowCountInSection) {
        _rowCountInSection = [NSMutableArray array];
    }
    
    NSDictionary *section  = nil;
    NSArray     *rows      = nil;
    for(NSInteger i = 0; i < _sectionCount; i++){
        section = [_localData objectAtIndex:i];
        if(nil == section){
            [_rowCountInSection addObject:@"0"];
        }
        else{
            rows = [section objectForKey:XXSHELL_SECTION_ROW];
            if(nil == rows){
                [_rowCountInSection addObject:@"0"];
            }
            else{
                [_rowCountInSection addObject:[NSString stringWithFormat:@"%lu", (unsigned long)rows.count]];
            }
        }
    }
    [self.tableView reloadData];
}

#pragma mark - section的数量和section中row的数量
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return _sectionCount;
}
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    
    if (nil == [_rowCountInSection objectAtIndex:section]) {
        return 0;
    }
    else{
        return [[_rowCountInSection objectAtIndex:section] integerValue];
    }
}

#pragma mark - header
- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section{
    id data = [self getSectionHeaderData:section];
    if (XXOC_IS_STRING(data)) {
        return data;
    }
    else if(XXOC_IS_DICTIONARY(data)){
        NSDictionary *dict = data;
        if (XXOC_IS_STRING([dict objectForKey:XXSHELL_HEADER_TITLE])) {
            return dict[XXSHELL_HEADER_TITLE];
        }
        else{
            return @"";
        }
    }
    else{
        return @"";
    }
}

#pragma mark - delegate相关函数
//- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
//    return _rowsHeight_;
//}
- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section{
    return _headerHeight;
}
- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section{
    return _footerHeight;
}
- (NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    // 选择功能未开放
    return nil;
}
- (void)tableView:(UITableView *)tableView willDisplayHeaderView:(UIView *)view forSection:(NSInteger)section{
    if (nil == _sectionsType) {
        // 使用默认header时,可以对其配置
        UITableViewHeaderFooterView *header = (UITableViewHeaderFooterView *)view;
        // 字体颜色
        if (nil != _headerTextColor){
            header.textLabel.textColor = _headerTextColor;
        }
        // 字体格式
        if(nil != _headerTextFont){
            header.textLabel.font = _headerTextFont;
        }
    }
}

#pragma mark - cell的生成
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    NSString *cellType      = self.cellType;
    UITableViewCell *cell   = nil;
    if (XXcellLoadType_Default == self.cellLoadType) {
        cell = [_tableView dequeueReusableCellWithIdentifier:cellType];
    }
    else{
        cell = [_tableView dequeueReusableCellWithIdentifier:cellType forIndexPath:indexPath];
    }
    
    // [2] 无可用cell,重新分配
    if(nil == cell){
        if (XXcellLoadType_Default == self.cellLoadType) {
            // 创建默认
            cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellType];
        }
        else{
            // 创建自定义cell
            cell = [[NSClassFromString(cellType) alloc] initWithReuseIdentifier:cellType];
        }
    }
    if (nil == cell) {
        XXNSLog(@"CellType=%@, 没有找对应的自定义cell类型", cellType);
        return nil;
    }
    
    // row对应的数据
    NSInteger section       = indexPath.section;
    NSInteger row           = indexPath.row;
    NSArray *rowDataArray   = [self getSectionRowsData:section];
    if (nil == rowDataArray) {
        return cell;
    }
    NSDictionary *rowData = [rowDataArray objectAtIndex:row];

    // [3] cell数据设置
    if (XXcellLoadType_Default == self.cellLoadType) {
        if (XXOC_IS_STRING([rowData objectForKey:XXSHELL_ROW_TEXT])) {
            cell.textLabel.text = rowData[XXSHELL_ROW_TEXT];
        }
    }
    else {
        // 设置数据 判断自定义的cell有没有实现约定的setData方法
        if ([cell respondsToSelector:@selector(setIndexPath:Data:)]) {
            id<XXcellProtocol> xxcell = (id<XXcellProtocol>)cell;
            [xxcell setIndexPath:indexPath Data:rowData];
        }
        
        // 设置事件处理block
        if(nil != self.onCellEvent && [cell respondsToSelector:@selector(setOnEvent:)]){
            [cell performSelector:@selector(setOnEvent:) withObject:self.onCellEvent];
        }
    }
    return cell;
}


- (id) getSectionData:(NSInteger)section Key:(NSString*)key{
    id obj = [_localData objectAtIndex:section];
    if (!XXOC_IS_DICTIONARY(obj)) {
        return nil;
    }
    
    NSDictionary *dict = obj;
    return [dict valueForKey:key];
}
- (id) getSectionHeaderData:(NSInteger)section{
    return [self getSectionData:section Key:XXSHELL_SECTION_HEADER];
}
- (id) getSectionFooterData:(NSInteger)section{
    return [self getSectionData:section Key:XXSHELL_SECTION_FOOTER];
}
- (NSArray*) getSectionRowsData:(NSInteger)section{
    return [self getSectionData:section Key:XXSHELL_SECTION_ROW];
}
@end
