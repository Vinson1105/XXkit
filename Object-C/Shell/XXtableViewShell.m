#import "XXtableViewShell.h"

#define DEFAULTE_CELL   @"_Cell_"    // 默认cell类型
#define SECTION_HEADER  @"_Header_"
#define SECTION_FOOTER  @"_Footer_"
#define SECTION_TITLE   @"_Title_"  // section中header/footer的默认标题
#define SECTION_ROW     @"_Row_"    // section中row数据

typedef void(^CellEventHandleBlock)(NSString *event, NSDictionary *params);

@interface XXtableViewShell ()

@property (nonatomic,weak) UITableView          *tableView;
@property (nonatomic,copy) NSString             *sectionsType;      // section的类型
@property (nonatomic,copy) NSString             *rowType;           // row的类型
@property (nonatomic,assign) CGFloat            headerHeight;       // header的高度
@property (nonatomic,assign) CGFloat            footerHeight;       // footer的高度

@property (nonatomic,strong) UIFont             *headerFont;       //
@property (nonatomic,strong) UIColor            *headerTextColor;  //

@property (nonatomic,assign) NSInteger          sectionCount;           // section的数量
@property (nonatomic,strong) NSMutableArray     *rowCountInSection;     // 每一个section中row的数量
@property (nonatomic,strong) NSMutableArray     *localData;            // 整个table的数据

@property (nonatomic,copy) CellEventHandleBlock cellEventHandleBlock;
@end

@implementation XXtableViewShell

#pragma mark - shell配置
- (void) shell:(UITableView*)tabelView{
    _tableView              = tabelView;
    _tableView.delegate     = self;
    _tableView.dataSource   = self;
    
    _tableView.estimatedRowHeight = 44.0f;                  // 预测高度
    _tableView.rowHeight = UITableViewAutomaticDimension;   // 自适应cell的高度
}

#pragma mark - section/row配置
- (void) setRowType:(NSString*)type{
    _rowType = type;
    [self.tableView registerNib:[UINib nibWithNibName:_rowType bundle:nil] forCellReuseIdentifier:_rowType];
}
- (void) setSectionType:(NSString*)type{
    _sectionsType = type;
}
- (void) setHeaderHeight:(CGFloat)height{
    _headerHeight = height;
}
- (void) setfooterHeight:(CGFloat)height{
    _footerHeight = height;
}
- (void) setRowHeight:(CGFloat)height{
    _tableView.rowHeight = height;
}
- (void) setHeaderHeight:(CGFloat)headerHeight RowHeight:(CGFloat)rowHeight FooterHeight:(CGFloat)footerHeight{
    _headerHeight           = headerHeight;
    _tableView.rowHeight    = rowHeight;
    _footerHeight           = footerHeight;
}

#pragma mark - section/row配置(次要配置)
- (void) setHeaderTextColor:(UIColor *)textColor Font:(UIFont *)font{
    _headerTextColor = textColor;
    _headerFont = font;
}

#pragma mark - 设置事件回调
- (void) setCellEventHandle:(void(^)(NSString *event, NSDictionary *params))block{
    _cellEventHandleBlock = block;
}

#pragma mark - 数据设置
- (void) setTableData:(NSArray*)data{
    // [0] 重置数据
    _localData = nil;
    _sectionCount = 0;
    [_rowCountInSection removeAllObjects];
    
    // [1] 表数据
    if(nil == data)
        return;
    _localData = [data mutableCopy];
    
    // [2] section的数量
    _sectionCount = _localData.count;
    
    // [3] section下row的数量
    if (nil == _rowCountInSection) {
        _rowCountInSection = [NSMutableArray array];
    }
    
    NSDictionary *pSection  = nil;
    NSArray     *pRows      = nil;
    for(NSInteger i = 0; i < _sectionCount; i++){
        pSection = [_localData objectAtIndex:i];
        if(nil == pSection){
            [_rowCountInSection addObject:@"0"];
        }
        else{
            pRows = [pSection objectForKey:SECTION_ROW];
            if(nil == pRows){
                [_rowCountInSection addObject:@"0"];
            }
            else{
                [_rowCountInSection addObject:[NSString stringWithFormat:@"%ld", pRows.count]];
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
    NSDictionary *pHeaderData = [self getSectionHeaderData:section];
    if (nil == pHeaderData) {
        return @"";
    }
    
    if (nil == [pHeaderData valueForKey:SECTION_TITLE]) {
        return @"";
    }
    return [pHeaderData valueForKey:SECTION_TITLE];
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
        if(nil != _headerFont){
            header.textLabel.font = _headerFont;
        }
    }
}

#pragma mark - cell的生成
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *pCell = nil;
    
    // [1] 取出已经生成的cell
    pCell = [tableView dequeueReusableCellWithIdentifier:_rowType forIndexPath:indexPath];
    
    // [2] 无可用cell,重新分配
    if(nil == pCell){
        
        if ([_rowType  isEqual:DEFAULTE_CELL]) {
            // [2-1] 默认的cell
            pCell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:DEFAULTE_CELL];
            
            // 默认cell的参数设置,如text,image之类的,需要进一步定义key
            [pCell.textLabel setText:[NSString stringWithFormat:@"cell%ld", (long)indexPath.row]];
        }
        else{
            // [2-2] 自定义cell
            Class rowClass  = NSClassFromString(_rowType);
            pCell           = [[rowClass alloc] initWithReuseIdentifier:_rowType];
        }
    }
    
    // [3] cell数据设置
    if (nil != pCell) {
        // 设置数据 判断自定义的cell有没有实现约定的setData方法
        if([pCell respondsToSelector:@selector(setCellData:)]){
            // NSLog
            // 获取对应row的数据路径为:section/row
            NSInteger section   = indexPath.section;
            NSInteger row       = indexPath.row;
            
            // rows的数据
            NSArray *pRowDatas = [self getSectionRowsData:section];
            if(nil != pRowDatas){
                
                // 指定row的数据
                NSDictionary *pRowData = [pRowDatas objectAtIndex:row];
                if (nil != pRowData) {
                    [pCell performSelector:@selector(setCellData:) withObject:pRowData];
                }
            }
        }
        
        // 设置事件处理block
        if(nil != self.cellEventHandleBlock && [pCell respondsToSelector:@selector(setEventHandle:)]){
            [pCell performSelector:@selector(setEventHandle:) withObject:self.cellEventHandleBlock];
        }
        
//        __weak XTableViewController *weakSelf = self;
//        [[NSOperationQueue mainQueue] addOperationWithBlock:^{
//            [UIView performWithoutAnimation:^{
//                [weakSelf.tableView reloadRowsAtIndexPaths:[NSArray arrayWithObjects:indexPath, nil] withRowAnimation:UITableViewRowAnimationNone];
//            }];
//        }];
    }
    
    return pCell;
}


/*
 // Override to support conditional editing of the table view.
 - (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
 // Return NO if you do not want the specified item to be editable.
 return YES;
 }
 */

/*
 // Override to support editing the table view.
 - (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
 if (editingStyle == UITableViewCellEditingStyleDelete) {
 // Delete the row from the data source
 [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];
 } else if (editingStyle == UITableViewCellEditingStyleInsert) {
 // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
 }
 }
 */

/*
 // Override to support rearranging the table view.
 - (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath {
 }
 */

/*
 // Override to support conditional rearranging of the table view.
 - (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath {
 // Return NO if you do not want the item to be re-orderable.
 return YES;
 }
 */

/*
 #pragma mark - Navigation
 
 // In a storyboard-based application, you will often want to do a little preparation before navigation
 - (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
 // Get the new view controller using [segue destinationViewController].
 // Pass the selected object to the new view controller.
 }
 */

- (id) getSectionData:(NSInteger)section Key:(NSString*)key{
    NSDictionary *p = [_localData objectAtIndex:section];
    if (nil == p) {
        return nil;
    }
    
    return [p valueForKey:key];
}
- (NSDictionary*) getSectionHeaderData:(NSInteger)section{
    return [self getSectionData:section Key:SECTION_HEADER];
}
- (NSDictionary*) getSectionFooterData:(NSInteger)section{
    return [self getSectionData:section Key:SECTION_FOOTER];
}
- (NSArray*) getSectionRowsData:(NSInteger)section{
    return [self getSectionData:section Key:SECTION_ROW];
}
@end
